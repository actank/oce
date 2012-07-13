import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeMap;

import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.Collector;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.Scorer;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.util.PriorityQueue;

import com.xiaonei.xce.log.TimeCost;

public class RelationCollector extends Collector {

	protected static final TopDocs EMPTY_TOPDOCS = new TopDocs(0,
			new ScoreDoc[0], Float.NaN);

	ScoreDoc pqTop;
	Scorer scorer;
	int docBase = 0;
	int totalHits = 0;

	// FriendCommonInfo infos;
	List<Integer> fids;
	List<Integer> commons;
	Map<Integer, Integer> relationMap = new TreeMap<Integer, Integer>();
	PriorityQueue<ScoreDoc> pq;
	int[] ids;

	Iterator<Entry<Integer, Integer>> iter = null;
	Entry<Integer, Integer> curIter = null;

	public RelationCollector(int numHits, List<Integer> fids, List<Integer> commons) {
		pq = new RenrenHitQueue(numHits, true);
		this.fids = fids;
		this.commons = commons;
		this.pqTop = pq.pop();
	}

	public void setRelation(Map<Integer, Integer> uid2Id, int[] doc2Uid) {
		//TimeCost tc = TimeCost.begin("setRelationXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		for (int i = 0; i < fids.size(); ++i) {
			Integer docId = uid2Id.get(fids.get(i));
			if (docId != null) {
				relationMap.put(docId, commons.get(i));
			}
		}
		//System.out.println("test getRelationMap : " + relationMap.size());
		//tc.endStep("FUCKKingnnnnnnnnnnnnnn");
		ids = doc2Uid;
		iter = relationMap.entrySet().iterator();
		if (iter.hasNext()) {
			curIter = iter.next();
		}
	}

	public TopDocs topDocs() {
		return topDocs(0, totalHits < pq.size() ? totalHits : pq.size());
		// if (pq.size() == 0) {
		// return new TopDocs(0, new ScoreDoc[0], Float.NaN);
		// }
		// System.out.println("TopDocs : " + pq.size());
		// ScoreDoc[] scoreDocs = new ScoreDoc[pq.size()];
		// for (int i = pq.size() - 1; i > 0; --i) {
		// scoreDocs[i] = pq.pop();
		// if (scoreDocs[i].doc >= ids.length) {
		// System.out.println("out array : " + i + " " + scoreDocs[i]);
		// scoreDocs[i].doc = -1;
		// } else
		// scoreDocs[i].doc = ids[scoreDocs[i].doc];
		// }
		// return new TopDocs(totalHits, scoreDocs, Float.NaN);
	}

	protected TopDocs newTopDocs(ScoreDoc[] results, int start) {
		return results == null ? EMPTY_TOPDOCS : new TopDocs(totalHits,
				results, Float.NaN);
	}

	public final TopDocs topDocs(int start, int howMany) {

		// In case pq was populated with sentinel values, there might be less
		// results than pq.size(). Therefore return all results until either
		// pq.size() or totalHits.
		int size = totalHits < pq.size() ? totalHits : pq.size();

		// Don't bother to throw an exception, just return an empty TopDocs in
		// case
		// the parameters are invalid or out of range.
		if (start < 0 || start >= size || howMany <= 0) {
			return newTopDocs(null, start);
		}

		// We know that start < pqsize, so just fix howMany.
		howMany = Math.min(size - start, howMany);
		ScoreDoc[] results = new ScoreDoc[howMany];

		// pq's pop() returns the 'least' element in the queue, therefore need
		// to discard the first ones, until we reach the requested range.
		// Note that this loop will usually not be executed, since the common
		// usage
		// should be that the caller asks for the last howMany results. However
		// it's
		// needed here for completeness.
		for (int i = pq.size() - start - howMany; i > 0; i--) {
			pq.pop();
		}

		// Get the requested results from pq.
		populateResults(results, howMany);

		return newTopDocs(results, start);
	}

	@Override
	public void setScorer(Scorer scorer) throws IOException {
		// TODO Auto-generated method stub
		this.scorer = scorer;
	}

	@Override
	public void collect(int doc) throws IOException {
		// TODO Auto-generated method stub
		float score = scorer.score();

		// This collector cannot handle these scores:
		assert score != Float.NEGATIVE_INFINITY;
		assert !Float.isNaN(score);
		//System.out.println(doc + " " + curIter.getKey());
		totalHits++;

		while (curIter != null && curIter.getKey() < doc) {
			if (iter.hasNext()) {
				curIter = iter.next();
			} else {
				curIter = null;
			}
		}
		if (curIter != null && curIter.getKey() == doc) {
			int type = (curIter.getValue() >> 16) & 0xFFFFF;
			int common = curIter.getValue() & (0xFFFF);
			// System.out.println("RelationCollector collector: " + doc + " : "
			// + score);
			score += common * 10 + (3-type)*1*10000;
			if (iter.hasNext()) {
				curIter = iter.next();
			} else {
				curIter = null;
			}
		}

		if (score <= pqTop.score) {
			// Since docs are returned in-order (i.e., increasing doc Id), a
			// document
			// with equal score to pqTop.score cannot compete since HitQueue
			// favors
			// documents with lower doc Ids. Therefore reject those docs too.
			return;
		}
		pqTop.doc = ids[doc];
		pqTop.score = score;
		pqTop = pq.updateTop();
	}

	@Override
	public void setNextReader(IndexReader reader, int docBase)
			throws IOException {
		// TODO Auto-generated method stub
		this.docBase = docBase;
	}

	@Override
	public boolean acceptsDocsOutOfOrder() {
		// TODO Auto-generated method stub
		return false;
	}

	protected void populateResults(ScoreDoc[] results, int howMany) {
		for (int i = howMany - 1; i >= 0; i--) {
			results[i] = pq.pop();
		}
	}

}
