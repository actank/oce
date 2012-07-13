import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.Collector;
import org.apache.lucene.search.DocIdSet;
import org.apache.lucene.search.DocIdSetIterator;
import org.apache.lucene.search.Filter;
import org.apache.lucene.search.Scorer;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.Weight;

public class UserSearcher extends RenrenSearcher {
	// private static Log logger_ = LogFactory.getLog(UserSearcher.class);

	private int ids_[];
	private Map<Integer, Integer> uid2Id_ = new HashMap<Integer, Integer>();

	public UserSearcher(IndexReader reader, String filepath)
			throws CorruptIndexException, IOException {
		super(reader);
		ids_ = loadIDs(filepath);// 加载ID
	}

	@Override
	public void search(Weight weight, Filter filter, Collector collector)
			throws IOException {
		if (collector instanceof RelationCollector) {
			((RelationCollector) collector).setRelation(uid2Id_, ids_);
		}
		if (filter == null) {
			for (int i = 0; i < subReaders.length; i++) { // search each
															// subreader
				collector.setNextReader(subReaders[i], docStarts[i]);
				Scorer scorer = weight.scorer(subReaders[i],
						!collector.acceptsDocsOutOfOrder(), true);
				if (scorer != null) {
					scorer.score(collector);
				}
			}
		} else {
			for (int i = 0; i < subReaders.length; i++) { // search each
															// subreader
				collector.setNextReader(subReaders[i], docStarts[i]);
				searchWithFilter(subReaders[i], weight, filter, collector);
			}
		}
	}

	private void searchWithFilter(IndexReader reader, Weight weight,
			final Filter filter, final Collector collector) throws IOException {

		assert filter != null;

		Scorer scorer = weight.scorer(reader, true, false);
		if (scorer == null) {
			return;
		}

		int docID = scorer.docID();
		assert docID == -1 || docID == DocIdSetIterator.NO_MORE_DOCS;

		// CHECKME: use ConjunctionScorer here?
		DocIdSet filterDocIdSet = filter.getDocIdSet(reader);
		if (filterDocIdSet == null) {
			// this means the filter does not accept any documents.
			return;
		}

		DocIdSetIterator filterIter = filterDocIdSet.iterator();
		if (filterIter == null) {
			// this means the filter does not accept any documents.
			return;
		}
		int filterDoc = filterIter.nextDoc();
		int scorerDoc = scorer.advance(filterDoc);

		collector.setScorer(scorer);
		while (true) {
			if (scorerDoc == filterDoc) {
				// Check if scorer has exhausted, only before collecting.
				if (scorerDoc == DocIdSetIterator.NO_MORE_DOCS) {
					break;
				}
				collector.collect(scorerDoc);
				filterDoc = filterIter.nextDoc();
				scorerDoc = scorer.advance(filterDoc);
			} else if (scorerDoc > filterDoc) {
				filterDoc = filterIter.advance(scorerDoc);
			} else {
				scorerDoc = scorer.advance(filterDoc);
			}
		}
	}

	public Document doc(int docid) throws CorruptIndexException, IOException {
		if (0 <= docid && docid < ids_.length) {
			Document doc = new Document();
			doc.add(new Field("user_basic.id", String.valueOf(ids_[docid]),
					Field.Store.YES, Field.Index.NOT_ANALYZED));
			return doc;
		}

		throw new IOException("Cannot find userid for docid " + docid);
	}

	public Document[] docidsToDocuments(int[] docids) throws IOException {
		Document[] documents = new Document[docids.length];
		for (int i = 0; i < docids.length; ++i) {
			int docid = docids[i];

			documents[i] = this.doc(docid);
		}

		return documents;
	}

	public int[] docidsToIDs(int[] docids) throws IOException {
		int[] ids = new int[docids.length];

		for (int i = 0; i < docids.length; ++i) {
			int docid = docids[i];

			if (0 <= docid && docid < ids_.length) {
				ids[i] = ids_[docid];
			} else {
				throw new IOException("Cannot find userid for docid " + docid);
			}
		}

		return ids;
	}

	/** 加载ID **/
	private int[] loadIDs(String filepath) throws IOException {
		System.out.println("loading docmap " + filepath);
		try {
			RandomAccessFile docfile = new RandomAccessFile(filepath, "r");
			long filelength = docfile.length();

			int length = (int) (filelength / 8 + 1);
			int[] ids = new int[length];
			byte[] docid_bytes = new byte[4];
			byte[] userid_bytes = new byte[4];
			int docid = 0;
			int userid = 0;
			int res = 0;

			for (;;) {
				// 读取前四个字节到docid_bytes,返回读取到的字节数
				res = docfile.read(docid_bytes, 0, 4);
				if (res <= 0) {
					break;
				}
				// 再读取四个字节到userid_bytes,返回读取到的字节数
				res = docfile.read(userid_bytes, 0, 4);
				if (res <= 0) {
					break;
				}
				// 将他们转为int值
				docid = ((docid_bytes[0] & 0xFF) << 24)
						| ((docid_bytes[1] & 0xFF) << 16)
						| ((docid_bytes[2] & 0xFF) << 8)
						| (docid_bytes[3] & 0xFF);

				userid = ((userid_bytes[0] & 0xFF) << 24)
						| ((userid_bytes[1] & 0xFF) << 16)
						| ((userid_bytes[2] & 0xFF) << 8)
						| (userid_bytes[3] & 0xFF);

				if (docid < ids.length) {
					ids[docid] = userid;
					uid2Id_.put(userid, docid);
					//System.out.println(userid + " " + docid);
				} else {
					throw new IOException("docid >= length: " + docid + " >= "
							+ ids.length);
				}
			}
			System.out.println("loading docmap " + filepath + "...done");
			return ids;
		} catch (FileNotFoundException e) {
			// ExceptionUtil.logException(e, logger_);

			throw new IOException("File cannot be open: " + filepath);
		}
	}

	@Override
	public TopDocs search(Weight weight, Filter filter, int nDocs,
			List<Integer> fids, List<Integer> commons) throws IOException {
		RelationCollector collector = new RelationCollector(nDocs, fids,
				commons);
		try {
			search(weight, filter, collector);
			return collector.topDocs();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;

	}

}
