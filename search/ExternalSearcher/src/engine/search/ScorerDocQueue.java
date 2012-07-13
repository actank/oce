package engine.search;

import java.io.IOException;

public class ScorerDocQueue { // later: SpansQueue for spans with doc and term
								// positions
	private final HeapedScorerDoc[] heap;
	private final int maxSize;
	private int size;

	private class HeapedScorerDoc {
		Scorer scorer;
		DocTf docTf;

		HeapedScorerDoc(Scorer s) {
			this(s, s.docTf());
		}

		HeapedScorerDoc(Scorer scorer, DocTf docTf) {
			this.scorer = scorer;
			this.docTf = docTf;
		}

		void adjust() {
			docTf = scorer.docTf();
		}
	}

	private HeapedScorerDoc topHSD; // same as heap[1], only for speed

	/** Create a ScorerDocQueue with a maximum size. */
	public ScorerDocQueue(int maxSize) {
		// assert maxSize >= 0;
		size = 0;
		int heapSize = maxSize + 1;
		heap = new HeapedScorerDoc[heapSize];
		this.maxSize = maxSize;
		topHSD = heap[1]; // initially null
	}

	/**
	 * Adds a Scorer to a ScorerDocQueue in log(size) time. If one tries to add
	 * more Scorers than maxSize a RuntimeException (ArrayIndexOutOfBound) is
	 * thrown.
	 */
	public final void put(Scorer scorer) {
		size++;
		heap[size] = new HeapedScorerDoc(scorer);
		upHeap();
	}

	/**
	 * Adds a Scorer to the ScorerDocQueue in log(size) time if either the
	 * ScorerDocQueue is not full, or not lessThan(scorer, top()).
	 * 
	 * @param scorer
	 * @return true if scorer is added, false otherwise.
	 */
	public boolean insert(Scorer scorer) {
		if (size < maxSize) {
			put(scorer);
			return true;
		} else {
			DocTf docNr = scorer.docTf();
			if ((size > 0) && (!(docNr.doc < topHSD.docTf.doc))) { // heap[1] is
																	// top()
				heap[1] = new HeapedScorerDoc(scorer, docNr);
				downHeap();
				return true;
			} else {
				return false;
			}
		}
	}

	/**
	 * Returns the least Scorer of the ScorerDocQueue in constant time. Should
	 * not be used when the queue is empty.
	 */
	public final Scorer top() {
		// assert size > 0;
		return topHSD.scorer;
	}

	/**
	 * Returns document number of the least Scorer of the ScorerDocQueue in
	 * constant time. Should not be used when the queue is empty.
	 */
	public final int topDoc() {
		// assert size > 0;
		return topHSD.docTf.doc;
	}

	public final float topScore() throws IOException {
		// assert size > 0;
		return topHSD.scorer.score();
	}

	public final boolean topNextAndAdjustElsePop() throws IOException {
		return checkAdjustElsePop(!topHSD.scorer.nextDocTf().equals(
				DocIdSetIterator.NO_MORE_DOCS));
		// return checkAdjustElsePop(!topHSD.scorer.nextDocTf().equals(
		// topHSD.scorer.docTf(), DocIdSetIterator.NO_MORE_DOCS));
	}

	public final boolean topSkipToAndAdjustElsePop(DocTf target)
			throws IOException {
		return checkAdjustElsePop(!topHSD.scorer.advance(target).equals(
				DocIdSetIterator.NO_MORE_DOCS));
		// return checkAdjustElsePop(!topHSD.scorer.docTf().equals(
		// topHSD.scorer.advance(target), DocIdSetIterator.NO_MORE_DOCS));
		// return checkAdjustElsePop(topHSD.scorer.advance(target) !=
		// DocIdSetIterator.NO_MORE_DOCS);
	}

	private boolean checkAdjustElsePop(boolean cond) {
		if (cond) { // see also adjustTop
			topHSD.docTf.doc = topHSD.scorer.docTf().doc;
			topHSD.docTf.tf = topHSD.scorer.docTf().tf;
		} else { // see also popNoResult
			heap[1] = heap[size]; // move last to first
			heap[size] = null;
			size--;
		}
		downHeap();
		return cond;
	}

	/**
	 * Removes and returns the least scorer of the ScorerDocQueue in log(size)
	 * time. Should not be used when the queue is empty.
	 */
	public final Scorer pop() {
		// assert size > 0;
		Scorer result = topHSD.scorer;
		popNoResult();
		return result;
	}

	/**
	 * Removes the least scorer of the ScorerDocQueue in log(size) time. Should
	 * not be used when the queue is empty.
	 */
	private final void popNoResult() {
		heap[1] = heap[size]; // move last to first
		heap[size] = null;
		size--;
		downHeap(); // adjust heap
	}

	/**
	 * Should be called when the scorer at top changes doc() value. Still log(n)
	 * worst case, but it's at least twice as fast to
	 * 
	 * <pre>
	 * {
	 * 	pq.top().change();
	 * 	pq.adjustTop();
	 * }
	 * </pre>
	 * 
	 * instead of
	 * 
	 * <pre>
	 * {
	 * 	o = pq.pop();
	 * 	o.change();
	 * 	pq.push(o);
	 * }
	 * </pre>
	 */
	public final void adjustTop() {
		// assert size > 0;
		topHSD.adjust();
		downHeap();
	}

	/** Returns the number of scorers currently stored in the ScorerDocQueue. */
	public final int size() {
		return size;
	}

	/** Removes all entries from the ScorerDocQueue. */
	public final void clear() {
		for (int i = 0; i <= size; i++) {
			heap[i] = null;
		}
		size = 0;
	}

	private final void upHeap() {
		int i = size;
		HeapedScorerDoc node = heap[i]; // save bottom node
		int j = i >>> 1;
		while ((j > 0) && (node.docTf.doc < heap[j].docTf.doc)) {
			heap[i] = heap[j]; // shift parents down
			i = j;
			j = j >>> 1;
		}
		heap[i] = node; // install saved node
		topHSD = heap[1];
	}

	private final void downHeap() {
		int i = 1;
		HeapedScorerDoc node = heap[i]; // save top node
		int j = i << 1; // find smaller child
		int k = j + 1;
		if ((k <= size) && (heap[k].docTf.doc < heap[j].docTf.doc)) {
			j = k;
		}
		while ((j <= size) && (heap[j].docTf.doc < node.docTf.doc)) {
			heap[i] = heap[j]; // shift up child
			i = j;
			j = i << 1;
			k = j + 1;
			if (k <= size && (heap[k].docTf.doc < heap[j].docTf.doc)) {
				j = k;
			}
		}
		heap[i] = node; // install saved node
		topHSD = heap[1];
	}
}
