package engine.search;

import java.io.IOException;

public abstract class TopScoreDocCollector extends TopDocsCollector<ScoreDoc> {

	// Assumes docs are scored in order.
	private static class InOrderTopScoreDocCollector extends
			TopScoreDocCollector {
		private InOrderTopScoreDocCollector(int numHits) {
			super(numHits);
		}

		@Override
		public void collect(DocTf docTf) throws IOException {
			float score = scorer.score();

			// This collector cannot handle these scores:
			assert score != Float.NEGATIVE_INFINITY;
			assert !Float.isNaN(score);

			totalHits++;
			if (score <= pqTop.score) {
				// Since docs are returned in-order (i.e., increasing doc Id), a
				// document
				// with equal score to pqTop.score cannot compete since HitQueue
				// favors
				// documents with lower doc Ids. Therefore reject those docs
				// too.
				return;
			}
			pqTop.doc = docTf.doc + docBase;
			pqTop.score = score;
			pqTop = pq.updateTop();
		}

		@Override
		public boolean acceptsDocsOutOfOrder() {
			return false;
		}

	}

	// Assumes docs are scored out of order.
	private static class OutOfOrderTopScoreDocCollector extends
			TopScoreDocCollector {
		private OutOfOrderTopScoreDocCollector(int numHits) {
			super(numHits);
		}

		@Override
		public void collect(DocTf docTf) throws IOException {
			float score = scorer.score();

			// This collector cannot handle NaN
			assert !Float.isNaN(score);

			totalHits++;
			docTf.doc += docBase;
			if (score < pqTop.score
					|| (score == pqTop.score && docTf.doc > pqTop.doc)) {
				return;
			}
			pqTop.doc = docTf.doc;
			pqTop.score = score;
			pqTop = pq.updateTop();
		}

		@Override
		public boolean acceptsDocsOutOfOrder() {
			return true;
		}
	}

	/**
	 * Creates a new {@link TopScoreDocCollector} given the number of hits to
	 * collect and whether documents are scored in order by the input
	 * {@link Scorer} to {@link #setScorer(Scorer)}.
	 * 
	 * <p>
	 * <b>NOTE</b>: The instances returned by this method pre-allocate a full
	 * array of length <code>numHits</code>, and fill the array with sentinel
	 * objects.
	 */
	public static TopScoreDocCollector create(int numHits,
			boolean docsScoredInOrder) {

		if (docsScoredInOrder) {
			return new InOrderTopScoreDocCollector(numHits);
		} else {
			return new OutOfOrderTopScoreDocCollector(numHits);
		}

	}

	ScoreDoc pqTop;
	int docBase = 0;
	Scorer scorer;

	// prevents instantiation
	private TopScoreDocCollector(int numHits) {
		super(new HitQueue(numHits, true));
		// HitQueue implements getSentinelObject to return a ScoreDoc, so we
		// know
		// that at this point top() is already initialized.
		pqTop = pq.top();
	}

	@Override
	protected TopDocs newTopDocs(ScoreDoc[] results, int start) {
		if (results == null) {
			return EMPTY_TOPDOCS;
		}

		// We need to compute maxScore in order to set it in TopDocs. If start
		// == 0,
		// it means the largest element is already in results, use its score as
		// maxScore. Otherwise pop everything else, until the largest element is
		// extracted and use its score as maxScore.
		float maxScore = Float.NaN;
		if (start == 0) {
			maxScore = results[0].score;
		} else {
			for (int i = pq.size(); i > 1; i--) {
				pq.pop();
			}
			maxScore = pq.pop().score;
		}

		return new TopDocs(totalHits, results, maxScore);
	}

	@Override
	public void setNextReader(IndexReader reader, int base) {
		docBase = base;
	}

	@Override
	public void setScorer(Scorer scorer) throws IOException {
		this.scorer = scorer;
	}
}
