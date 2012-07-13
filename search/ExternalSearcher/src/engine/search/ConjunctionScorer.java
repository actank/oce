package engine.search;

import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;
import java.util.Comparator;

class ConjunctionScorer extends Scorer {

	private final Scorer[] scorers;
	private DocTf lastDocTf = new DocTf(-1, -1);

	public ConjunctionScorer(Similarity similarity, Collection<Scorer> scorers)
			throws IOException {
		this(similarity, scorers.toArray(new Scorer[scorers.size()]));
	}

	public ConjunctionScorer(Similarity similarity, Scorer... scorers)
			throws IOException {
		super(similarity);
		System.out.println("scorers.size:" + scorers.length);
		this.scorers = scorers;

		for (int i = 0; i < scorers.length; i++) {
			if (scorers[i].nextDocTf() == NO_MORE_DOCS) {
				// If even one of the sub-scorers does not have any documents,
				// this
				// scorer should not attempt to do any more work.
				lastDocTf = NO_MORE_DOCS;
				return;
			}
		}

		// Sort the array the first time...
		// We don't need to sort the array in any future calls because we know
		// it will already start off sorted (all scorers on same doc).

		// note that this comparator is not consistent with equals!
		Arrays.sort(scorers, new Comparator<Scorer>() { // sort the array
					public int compare(Scorer o1, Scorer o2) {
						return o1.docTf().doc - o2.docTf().doc;
					}
				});

		// NOTE: doNext() must be called before the re-sorting of the array
		// later on.
		// The reason is this: assume there are 5 scorers, whose first docs are
		// 1,
		// 2, 3, 5, 5 respectively. Sorting (above) leaves the array as is.
		// Calling
		// doNext() here advances all the first scorers to 5 (or a larger doc ID
		// they all agree on).
		// However, if we re-sort before doNext() is called, the order will be
		// 5, 3,
		// 2, 1, 5 and then doNext() will stop immediately, since the first
		// scorer's
		// docs equals the last one. So the invariant that after calling
		// doNext()
		// all scorers are on the same doc ID is broken.
		if (doNext() == NO_MORE_DOCS) {
			// The scorers did not agree on any document.
			lastDocTf = NO_MORE_DOCS;
			return;
		}

		// If first-time skip distance is any predictor of
		// scorer sparseness, then we should always try to skip first on
		// those scorers.
		// Keep last scorer in it's last place (it will be the first
		// to be skipped on), but reverse all of the others so that
		// they will be skipped on in order of original high skip.
		int end = scorers.length - 1;
		int max = end >> 1;
		for (int i = 0; i < max; i++) {
			Scorer tmp = scorers[i];
			int idx = end - i - 1;
			scorers[i] = scorers[idx];
			scorers[idx] = tmp;
		}
	}

	private DocTf doNext() throws IOException {
		int first = 0;
		DocTf docTf = scorers[scorers.length - 1].docTf();
		if (docTf.equals(NO_MORE_DOCS)) {
			return NO_MORE_DOCS;
		}
		Scorer firstScorer;
		while ((firstScorer = scorers[first]).docTf().doc < docTf.doc) {
			System.out.println("first doc:" + firstScorer.docTf().doc
					+ " last doc:" + docTf.doc);
			docTf = firstScorer.advance(docTf);
			System.out.println("first doc:" + firstScorer.docTf().doc
					+ " last doc:" + docTf.doc);
			first = first == scorers.length - 1 ? 0 : first + 1;
		}
		return docTf;
	}

	@Override
	public DocTf advance(DocTf target) throws IOException {
		if (lastDocTf == NO_MORE_DOCS) {
			return lastDocTf;
		} else if (scorers[(scorers.length - 1)].docTf().doc < target.doc) {
			scorers[(scorers.length - 1)].advance(target);
		}
		return lastDocTf = doNext();
	}

	@Override
	public DocTf docTf() {
		return lastDocTf;
	}

	@Override
	public DocTf nextDocTf() throws IOException {
		if (lastDocTf == NO_MORE_DOCS) {
			return lastDocTf;
		} else if (lastDocTf.doc == -1) {
			return lastDocTf = scorers[scorers.length - 1].docTf();
		}
		scorers[(scorers.length - 1)].nextDocTf();
		return lastDocTf = doNext();
	}

	@Override
	public float score() throws IOException {
		float sum = 0.0f;
		for (int i = 0; i < scorers.length; i++) {
			sum += scorers[i].score();
		}
		return sum;
	}

}
