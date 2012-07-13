package com.xiaonei.search3.server;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.Searcher;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.LockObtainFailedException;
import org.apache.lucene.store.RAMDirectory;

import com.xiaonei.search2.util.AnalyzerFactory;
import com.xiaonei.search3.indexer.BuildIndex;
import com.xiaonei.search3.indexer.BuildRunable;
import com.xiaonei.search3.query.FofIdTool;
import com.xiaonei.search3.query.process.ProcessorDespatcher;
import com.xiaonei.search3.tools.Database;
import com.xiaonei.search3.tools.Tools;

public class PersonalSearcherI extends _PersonalSearcherDisp {
	private Database database = new Database(Tools.getInstance()
			.getConnections());
	// 保存每个人小索引的map
	private Map<Integer, IndexReader> id2Reader = new ConcurrentHashMap<Integer, IndexReader>();
	// 保存每个人二度好友大索引的map
	// private Map<Integer, IndexReader> id2MergeReader = new
	// ConcurrentHashMap<Integer, IndexReader>();

	private Set<Integer> blockId = new HashSet<Integer>();
	private BuildIndex index = new BuildIndex(database, id2Reader);
	private BlockingQueue<Runnable> queue = new LinkedBlockingQueue<Runnable>(
			1000000);
	private ThreadPoolExecutor executor = new ThreadPoolExecutor(8, 16, 60,
			TimeUnit.DAYS, queue, new ThreadPoolExecutor.CallerRunsPolicy());
	private Logger logger = null;

	public PersonalSearcherI() {
		DOMConfigurator
				.configure("/data/xce/AddressBook/etc/classes/log4j.xml");
		logger = Logger.getLogger("my.log");
	}

	// 进行索引的合并
	private IndexReader getMergeIndexReader(int userId,
			List<IndexReader> readers) {
		IndexWriter writer = null;
		try {
			writer = new IndexWriter(new RAMDirectory(), AnalyzerFactory
					.getInstance().getUserAnalyzer(), true,
					IndexWriter.MaxFieldLength.UNLIMITED);
			writer.addIndexes(readers.toArray(new IndexReader[readers.size()]));
			return writer.getReader();
		} catch (CorruptIndexException e) {
			e.printStackTrace();
		} catch (LockObtainFailedException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	// 从内存取到二度好友的IndexReader，如果内存没有则异步去加载用户索引
	private List<IndexReader> getReader(Set<Integer> idSet) {
		List<IndexReader> readers = new ArrayList<IndexReader>(idSet.size());
		List<Integer> other = new ArrayList<Integer>();
		for (Integer id : idSet) {
			IndexReader reader = id2Reader.get(id);
			if (reader != null) {
				readers.add(reader);
				continue;
			}
			if (!blockId.contains(id))
				other.add(id);
		}
		// 异步去构建没有在内存的用户的索引
		buildIndexes(other, blockId);
		System.out.println("getReader : " + readers.size()
				+ " in memory; build size:" + other.size());
		return readers;
	}

	public void buildIndexes(List<Integer> userIds, Set<Integer> idSet) {
		int bucket = 10;
		int step = 20;
		int sum = 0;

		// 为了防止runable过多，将20个人放到一个runable里面去同步创建索引
		while (sum < userIds.size()) {
			int end = sum + step >= userIds.size() ? userIds.size() : sum
					+ step;
			executor.execute(new BuildRunable(userIds.subList(sum, end), index,
					idSet));
			sum += step;
		}
	}

	// 进行搜索并将用户id返回
	private List<Integer> search(Searcher search, Query q, int limit) {
		TopDocs docs = null;
		try {
			docs = search.search(q, 2000);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		ScoreDoc[] sdocs = docs.scoreDocs;
		List<Integer> ids = new ArrayList<Integer>(sdocs.length);
		for (ScoreDoc sdoc : sdocs) {
			try {
				ids.add(Integer.valueOf(search.doc(sdoc.doc).get(
						"user_basic.id")));
			} catch (NumberFormatException e) {
				e.printStackTrace();
			} catch (CorruptIndexException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return ids;
	}

	public List<Integer> getResult(List<Integer> res, int begin, int limit) {
		if (limit < 0)
			return res;
		if (begin > 0 && begin + limit <= res.size())
			return res.subList(begin, begin + limit);
		else
			return new ArrayList<Integer>();
	}

	// public static Log logger = LogFactory.getLog("Personal");

	public List<Integer> search(int userId,
			Map<String, Map<String, String>> query, int begin, int limit,
			Ice.Current __current) {
		Query q = ProcessorDespatcher.process(query);
		long t1 = System.currentTimeMillis();
		List<Integer> res = null;

		Set<Integer> idSet = FofIdTool.getFOFID(userId);

		long t2 = System.currentTimeMillis();
		logger.info("id: " + userId + " getFoFSet getsize:" + idSet.size()
				+ " cost:" + (t2 - t1));

		List<IndexReader> readers = getReader(idSet);

		long t3 = System.currentTimeMillis();
		logger.info("id: " + userId + " getReader getsize:" + readers.size()
				+ " cost:" + (t3 - t2));

		IndexReader mergeReader = getMergeIndexReader(userId, readers);

		IndexSearcher searcher = new IndexSearcher(mergeReader);
		long t4 = System.currentTimeMillis();
		logger.info("id: " + userId + " mergeIndex cost: " + (t4 - t3));

		res = search(searcher, q, 2000);

		long t5 = System.currentTimeMillis();
		logger.info("id: " + userId + " search find size:" + res.size()
				+ " cost: " + (t5 - t4));
		return getResult(res, begin, limit);
	}
}
