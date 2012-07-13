package engine.search;

import java.io.IOException;
import java.util.List;

class DisjunctionSumScorer extends Scorer {
	private final int nrScorers;
	protected final List<Scorer> subScorers;

	private final int minimumNrMatchers;

	private ScorerDocQueue scorerDocQueue;

	private DocTf currentDocTf = new DocTf(-1, -1);

	protected int nrMatchers = -1;

	private float currentScore = Float.NaN;

	public DisjunctionSumScorer(List<Scorer> subScorers, int minimumNrMatchers)
			throws IOException {
		super(null);

		nrScorers = subScorers.size();

		if (minimumNrMatchers <= 0) {
			throw new IllegalArgumentException(
					"Minimum nr of matchers must be positive");
		}
		if (nrScorers <= 1) {
			throw new IllegalArgumentException(
					"There must be at least 2 subScorers");
		}

		this.minimumNrMatchers = minimumNrMatchers;
		this.subScorers = subScorers;

		initScorerDocQueue();
	}

	public DisjunctionSumScorer(List<Scorer> subScorers) throws IOException {
		this(subScorers, 1);
	}

	private void initScorerDocQueue() throws IOException {
		scorerDocQueue = new ScorerDocQueue(nrScorers);
		for (Scorer se : subScorers) {
			if (se.nextDocTf() != NO_MORE_DOCS) {
				scorerDocQueue.insert(se);
			}
		}
	}

	@Override
	public void score(Collector collector) throws IOException {
		collector.setScorer(this);
		while (nextDocTf() != NO_MORE_DOCS) {
			collector.collect(currentDocTf);
		}
	}

	/**
	 * Expert: Collects matching documents in a range. Hook for optimization.
	 * Note that {@link #next()} must be called once before this method is
	 * called for the first time.
	 * 
	 * @param collector
	 *            The collector to which all matching documents are passed
	 *            through.
	 * @param max
	 *            Do not score documents past this.
	 * @return true if more matching documents may remain.
	 */
	@Override
	protected boolean score(Collector collector, int max, int firstDocID)
			throws IOException {
		// firstDocID is ignored since nextDoc() sets 'currentDoc'
		collector.setScorer(this);
		while (currentDocTf.doc < max) {
			collector.collect(currentDocTf);
			if (nextDocTf() == NO_MORE_DOCS) {
				return false;
			}
		}
		return true;
	}

	@Override
	public DocTf nextDocTf() throws IOException {
		if (scorerDocQueue.size() < minimumNrMatchers || !advanceAfterCurrent()) {
			currentDocTf = NO_MORE_DOCS;
		}
		return currentDocTf;
	}

	/**
	 * Advance all subscorers after the current document determined by the top
	 * of the <code>scorerDocQueue</code>. Repeat until at least the minimum
	 * number of subscorers match on the same document and all subscorers are
	 * after that document or are exhausted. <br>
	 * On entry the <code>scorerDocQueue</code> has at least
	 * <code>minimumNrMatchers</code> available. At least the scorer with the
	 * minimum document number will be advanced.
	 * 
	 * @return true iff there is a match. <br>
	 *         In case there is a match, </code>currentDoc</code>,
	 *         </code>currentSumScore</code>, and </code>nrMatchers</code>
	 *         describe the match.
	 * 
	 *         TODO: Investigate whether it is possible to use skipTo() when the
	 *         minimum number of matchers is bigger than one, ie. try and use
	 *         the character of ConjunctionScorer for the minimum number of
	 *         matchers. Also delay calling score() on the sub scorers until the
	 *         minimum number of matchers is reached. <br>
	 *         For this, a Scorer array with minimumNrMatchers elements might
	 *         hold Scorers at currentDoc that are temporarily popped from
	 *         scorerQueue.
	 */
	protected boolean advanceAfterCurrent() throws IOException {
		do { // repeat until minimum nr of matchers
			currentDocTf.doc = scorerDocQueue.topDoc();
			System.out.println("currentDocTf.doc:" + currentDocTf.doc + " tf"
					+ currentDocTf.tf);
			currentScore = scorerDocQueue.topScore();
			nrMatchers = 1;
			do { // Until all subscorers are after currentDoc
				if (!scorerDocQueue.topNextAndAdjustElsePop()) {
					if (scorerDocQueue.size() == 0) {
						break; // nothing more to advance, check for last match.
					}
				}
				if (scorerDocQueue.topDoc() != currentDocTf.doc) {
					break; // All remaining subscorers are after currentDoc.
				}
				currentScore += scorerDocQueue.topScore();
				nrMatchers++;
			} while (true);

			if (nrMatchers >= minimumNrMatchers) {
				return true;
			} else if (scorerDocQueue.size() < minimumNrMatchers) {
				return false;
			}
		} while (true);
	}

	/**
	 * Returns the score of the current document matching the query. Initially
	 * invalid, until {@link #next()} is called the first time.
	 */
	@Override
	public float score() throws IOException {
		return currentScore;
	}

	@Override
	public DocTf docTf() {
		return currentDocTf;
	}

	/**
	 * Returns the number of subscorers matching the current document. Initially
	 * invalid, until {@link #next()} is called the first time.
	 */
	public int nrMatchers() {
		return nrMatchers;
	}

	/**
	 * Advances to the first match beyond the current whose document number is
	 * greater than or equal to a given target. <br>
	 * The implementation uses the skipTo() method on the subscorers.
	 * 
	 * @param target
	 *            The target document number.
	 * @return the document whose number is greater than or equal to the given
	 *         target, or -1 if none exist.
	 */
	@Override
	public DocTf advance(DocTf target) throws IOException {
		if (scorerDocQueue.size() < minimumNrMatchers) {
			return currentDocTf = NO_MORE_DOCS;
		}
		if (target.doc <= currentDocTf.doc) {
			return currentDocTf;
		}
		do {
			if (scorerDocQueue.topDoc() >= target.doc) {
				return advanceAfterCurrent() ? currentDocTf
						: (currentDocTf = NO_MORE_DOCS);
			} else if (!scorerDocQueue.topSkipToAndAdjustElsePop(target)) {
				if (scorerDocQueue.size() < minimumNrMatchers) {
					return currentDocTf = NO_MORE_DOCS;
				}
			}
		} while (true);
	}

}
