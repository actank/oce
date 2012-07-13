package engine.search;

import java.io.IOException;

class ReqOptSumScorer extends Scorer {

	private Scorer reqScorer;
	private Scorer optScorer;

	public ReqOptSumScorer(Scorer reqScorer, Scorer optScorer) {
		super(null); // No similarity used.
		this.reqScorer = reqScorer;
		this.optScorer = optScorer;
	}

	@Override
	public DocTf nextDocTf() throws IOException {
		return reqScorer.nextDocTf();
	}

	@Override
	public DocTf advance(DocTf target) throws IOException {
		return reqScorer.advance(target);
	}

	@Override
	public DocTf docTf() {
		return reqScorer.docTf();
	}

	@Override
	public float score() throws IOException {
		DocTf curDoc = reqScorer.docTf();
		float reqScore = reqScorer.score();
		if (optScorer == null) {
			return reqScore;
		}

		DocTf optScorerDoc = optScorer.docTf();
		if (optScorerDoc.doc < curDoc.doc
				&& (optScorerDoc = optScorer.advance(curDoc)) == NO_MORE_DOCS) {
			optScorer = null;
			return reqScore;
		}

		return optScorerDoc.doc == curDoc.doc ? reqScore + optScorer.score()
				: reqScore;
	}

}
