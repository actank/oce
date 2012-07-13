package engine.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

class BooleanScorer2 extends Scorer {

	private final List<Scorer> requiredScorers;
	private final List<Scorer> optionalScorers;
	private final List<Scorer> prohibitedScorers;

	private final Scorer countingSumScorer;

	private final int minNrShouldMatch;

	private DocTf docTf = new DocTf(-1, -1);

	public BooleanScorer2(int minNrShouldMatch, List<Scorer> required,
			List<Scorer> prohibited, List<Scorer> optional) throws IOException {
		if (minNrShouldMatch < 0) {
			throw new IllegalArgumentException(
					"Minimum number of optional scorers should not be negative");
		}
		this.minNrShouldMatch = minNrShouldMatch;

		optionalScorers = optional;

		requiredScorers = required;

		prohibitedScorers = prohibited;

		countingSumScorer = makeCountingSumScorer();
	}

	private Scorer countingDisjunctionSumScorer(final List<Scorer> scorers,
			int minNrShouldMatch) throws IOException {
		// each scorer from the list counted as a single matcher
		return new DisjunctionSumScorer(scorers, minNrShouldMatch) {
			private int lastScoredDoc = -1;
			// Save the score of lastScoredDoc, so that we don't compute it more
			// than
			// once in score().
			private float lastDocScore = Float.NaN;

			@Override
			public float score() throws IOException {
				int doc = docTf().doc;
				if (doc >= lastScoredDoc) {
					if (doc > lastScoredDoc) {
						lastDocScore = super.score();
						lastScoredDoc = doc;
					}
				}
				return lastDocScore;
			}
		};
	}

	private static final Similarity defaultSimilarity = Similarity.getDefault();

	private Scorer countingConjunctionSumScorer(List<Scorer> requiredScorers)
			throws IOException {
		// each scorer from the list counted as a single matcher
		final int requiredNrMatchers = requiredScorers.size();
		return new ConjunctionScorer(defaultSimilarity, requiredScorers) {
			private int lastScoredDoc = -1;
			private float lastDocScore = Float.NaN;

			@Override
			public float score() throws IOException {
				int doc = docTf().doc;
				if (doc >= lastScoredDoc) {
					if (doc > lastScoredDoc) {
						lastDocScore = super.score();
						lastScoredDoc = doc;
					}
				}
				return lastDocScore;
			}
		};
	}

	private class SingleMatchScorer extends Scorer {
		private Scorer scorer;
		private int lastScoredDoc = -1;
		private float lastDocScore = Float.NaN;

		SingleMatchScorer(Scorer scorer) {
			super(scorer.getSimilarity());
			this.scorer = scorer;
		}

		@Override
		public float score() throws IOException {
			int doc = docTf().doc;
			if (doc >= lastScoredDoc) {
				if (doc > lastScoredDoc) {
					lastDocScore = scorer.score();
					lastScoredDoc = doc;
				}
			}
			return lastDocScore;
		}

		@Override
		public DocTf docTf() {
			return scorer.docTf();
		}

		@Override
		public DocTf nextDocTf() throws IOException {
			return scorer.nextDocTf();
		}

		@Override
		public DocTf advance(DocTf target) throws IOException {
			return scorer.advance(target);
		}
	}

	@SuppressWarnings("unused")
	private Scorer dualConjunctionSumScorer(Scorer req1, Scorer req2)
			throws IOException { // non counting.
		return new ConjunctionScorer(defaultSimilarity, new Scorer[] { req1,
				req2 });
		// All scorers match, so defaultSimilarity always has 1 as
		// the coordination factor.
		// Therefore the sum of the scores of two scorers
		// is used as score.
	}

	/**
	 * Returns the scorer to be used for match counting and score summing. Uses
	 * requiredScorers, optionalScorers and prohibitedScorers.
	 */
	public Scorer makeCountingSumScorer() throws IOException {
		return (requiredScorers.size() == 0) ? makeCountingSumScorerNoReq()
				: makeCountingSumScorerSomeReq();
	}

	private Scorer makeCountingSumScorerNoReq() throws IOException {
		int nrOptRequired = (minNrShouldMatch < 1) ? 1 : minNrShouldMatch;
		Scorer requiredCountingSumScorer;
		if (optionalScorers.size() > nrOptRequired)
			requiredCountingSumScorer = countingDisjunctionSumScorer(
					optionalScorers, nrOptRequired);
		else if (optionalScorers.size() == 1)
			requiredCountingSumScorer = new SingleMatchScorer(
					optionalScorers.get(0));
		else
			requiredCountingSumScorer = countingConjunctionSumScorer(optionalScorers);
		return requiredCountingSumScorer;
	}

	private Scorer makeCountingSumScorerSomeReq() throws IOException {
		if (optionalScorers.size() == minNrShouldMatch) {
			ArrayList<Scorer> allReq = new ArrayList<Scorer>(requiredScorers);
			allReq.addAll(optionalScorers);
			return countingConjunctionSumScorer(allReq);
		} else { // optionalScorers.size() > minNrShouldMatch, and at least one
					// required scorer
			Scorer requiredCountingSumScorer = requiredScorers.size() == 1 ? new SingleMatchScorer(
					requiredScorers.get(0))
					: countingConjunctionSumScorer(requiredScorers);
			if (minNrShouldMatch > 0) { // use a required disjunction scorer
										// over the optional scorers
				return dualConjunctionSumScorer(
						// non counting
						requiredCountingSumScorer,
						countingDisjunctionSumScorer(optionalScorers,
								minNrShouldMatch));
			} else { // minNrShouldMatch == 0
				return new ReqOptSumScorer(requiredCountingSumScorer,
						optionalScorers.size() == 1 ? new SingleMatchScorer(
								optionalScorers.get(0))
						// require 1 in combined, optional scorer.
								: countingDisjunctionSumScorer(optionalScorers,
										1));
			}
		}
	}

	@Override
	public void score(Collector collector) throws IOException {
		collector.setScorer(this);
		while ((docTf = countingSumScorer.nextDocTf()) != NO_MORE_DOCS) {
			collector.collect(docTf);
		}
	}

	@Override
	protected boolean score(Collector collector, int max, int firstDocID)
			throws IOException {
		collector.setScorer(this);
		docTf = docTf();
		int doc = docTf.doc;
		while (doc < max) {
			collector.collect(docTf);
			docTf = nextDocTf();
		}
		return docTf != NO_MORE_DOCS;
	}

	@Override
	public DocTf docTf() {
		return docTf;
	}

	@Override
	public DocTf nextDocTf() throws IOException {
		return docTf = countingSumScorer.nextDocTf();
	}

	@Override
	public DocTf advance(DocTf target) throws IOException {
		return docTf = countingSumScorer.advance(target);
	}

	@Override
	public float score() throws IOException {
		float sum = countingSumScorer.score();
		return sum;
	}
}
