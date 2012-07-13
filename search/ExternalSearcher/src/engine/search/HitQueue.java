package engine.search;

import org.apache.lucene.util.PriorityQueue;

final class HitQueue extends PriorityQueue<ScoreDoc> {

	private boolean prePopulate;

	HitQueue(int size, boolean prePopulate) {
		this.prePopulate = prePopulate;
		initialize(size);
	}

	@Override
	protected ScoreDoc getSentinelObject() {
		return !prePopulate ? null : new ScoreDoc(Integer.MAX_VALUE,
				Float.NEGATIVE_INFINITY);
	}

	@Override
	protected final boolean lessThan(ScoreDoc hitA, ScoreDoc hitB) {
		if (hitA.score == hitB.score)
			return hitA.doc > hitB.doc;
		else
			return hitA.score < hitB.score;
	}
}
