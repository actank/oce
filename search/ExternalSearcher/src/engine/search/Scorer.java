package engine.search;

import java.io.IOException;
import java.util.ArrayList;

public abstract class Scorer extends DocIdSetIterator {
	private Similarity similarity;

	protected Scorer(Similarity similarity) {
		this.similarity = similarity;
	}

	public Similarity getSimilarity() {
		return this.similarity;
	}

	protected Scorer() {
	}

	public void score(Collector collector) throws IOException {
		collector.setScorer(this);
		DocTf docTf;
		while ((docTf = nextDocTf()) != NO_MORE_DOCS) {
			collector.collect(docTf);
		}
	}

	protected boolean score(Collector collector, int max, int firstDocID)
			throws IOException {
		collector.setScorer(this);
		DocTf docTf = docTf();
		int doc = docTf.doc;
		while (doc < max) {
			collector.collect(docTf);
			docTf = nextDocTf();
		}
		return docTf != NO_MORE_DOCS;
	}

	public abstract float score() throws IOException;

}
