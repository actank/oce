package engine.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.lucene.store.IndexInput;

import engine.search.BooleanClause.Occur;

public class IndexSearcher {
	// @SuppressWarnings("unused")
	private IndexReader[] readers;

	public IndexReader[] getReaders() {
		return readers;
	}

	public IndexSearcher(IndexReader... readers) {
		this.readers = readers;
	}

	public ArrayList<Integer> search(BooleanQuery query, int needTotal)
			throws IOException {
		// System.out.println("ArrayList<TermQuery> querys.size" +
		// querys.size());
		ArrayList<Integer> ids = new ArrayList<Integer>();
		if (query == null) {
			return ids;
		}

		// System.out.println("ArrayList<Term> terms.size" + terms.size());

		for (int k = 0; k < readers.length; k++) {
			// ArrayList<FieldOffset> fieldOffsets = new
			// ArrayList<FieldOffset>();
			// BooleanQuery query = new BooleanQuery();
			// for (int i = 0; i < terms.size(); i++) {
			// TermQuery termQuery = new TermQuery(terms.get(i), readers[k]);
			// query.add(termQuery, Occur.MUST);
			// System.out.println("query.field:" + terms.get(i).getField_()
			// + " query.word:" + terms.get(i).getWord_());
			// }
			TopScoreDocCollector collector = TopScoreDocCollector.create(
					needTotal, true);
			Scorer scorer = query.scorer(readers[k]);
			if (scorer != null) {
				scorer.score(collector);
			}

			TopDocs results = collector.topDocs();
			System.out.println("results.scoreDocs.size:"
					+ results.scoreDocs.length);
			System.out.println("resultstotalHits:" + results.totalHits);
			ids.addAll(readers[k].mappingId(results.scoreDocs));
			System.out.println("ids.size:" + ids.size());

		}
		if (ids.size() > 500) {
			ArrayList<Integer> newids = new ArrayList<Integer>();
			newids.addAll(ids.subList(0, 500));
			return newids;
		} else {
			return ids;
		}
	}

}
