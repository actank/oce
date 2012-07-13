package engine.search;

import org.apache.lucene.util.PriorityQueue;

public abstract class TopDocsCollector<T extends ScoreDoc> extends Collector {

	// This is used in case topDocs() is called with illegal parameters, or
	// there
	// simply aren't (enough) results.
	protected static final TopDocs EMPTY_TOPDOCS = new TopDocs(0,
			new ScoreDoc[0], Float.NaN);

	protected PriorityQueue<T> pq;

	/** The total number of documents that the collector encountered. */
	protected int totalHits;

	protected TopDocsCollector(PriorityQueue<T> pq) {
		this.pq = pq;
	}

	protected void populateResults(ScoreDoc[] results, int howMany) {
		for (int i = howMany - 1; i >= 0; i--) {
			results[i] = pq.pop();
		}
	}

	protected TopDocs newTopDocs(ScoreDoc[] results, int start) {
		return results == null ? EMPTY_TOPDOCS
				: new TopDocs(totalHits, results);
	}

	/** The total number of documents that matched this query. */
	public int getTotalHits() {
		return totalHits;
	}

	/** Returns the top docs that were collected by this collector. */
	public final TopDocs topDocs() {
		return topDocs(0, totalHits < pq.size() ? totalHits : pq.size());
	}

	public final TopDocs topDocs(int start) {
		return topDocs(start, totalHits < pq.size() ? totalHits : pq.size());
	}

	public final TopDocs topDocs(int start, int howMany) {

		int size = totalHits < pq.size() ? totalHits : pq.size();

		if (start < 0 || start >= size || howMany <= 0) {
			return newTopDocs(null, start);
		}

		howMany = Math.min(size - start, howMany);
		ScoreDoc[] results = new ScoreDoc[howMany];

		for (int i = pq.size() - start - howMany; i > 0; i--) {
			pq.pop();
		}

		populateResults(results, howMany);

		return newTopDocs(results, start);
	}

}
