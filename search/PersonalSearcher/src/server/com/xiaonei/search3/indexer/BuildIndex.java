package com.xiaonei.search3.indexer;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.LogDocMergePolicy;
import org.apache.lucene.index.SerialMergeScheduler;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.store.RAMDirectory;

import com.xiaonei.search.util.db.DatabaseI;

import com.xiaonei.search2.model.Constraints;
import com.xiaonei.search2.model.info.UserInfo;
import com.xiaonei.search2.model.info.UserInfoUnserializer;
import com.xiaonei.search2.util.AnalyzerFactory;
import com.xiaonei.search3.model.UserIndexView;
import com.xiaonei.search3.tools.Database;
import com.xiaonei.search3.tools.Tools;

public class BuildIndex {
	private Database database_;

	private Map<Integer, IndexReader> id2Reader;
	private Set<Integer> idSet;

	public BuildIndex(Database database, Map<Integer, IndexReader> id2Reader) {
		this.database_ = database;
		this.id2Reader = id2Reader;
	}

	private boolean addIndexMem(int userId) {
		try {
			File file = new File(Tools.getInstance().getIndexPath(userId));
			RAMDirectory ram = new RAMDirectory(FSDirectory.open(file));
			IndexReader reader = IndexReader.open(ram);
			id2Reader.put(userId, reader);
		} catch (Exception ex) {
			System.out.println(userId);
			ex.printStackTrace();
			return false;
		}
		return true;
	}

	public void removeSet(int userId) {
		idSet.remove(userId);
	}

	public boolean buildindex(int userId) {
		if (id2Reader.containsKey(userId))
			return true;
		String workingPath_ = Tools.getInstance().getIndexPath(userId);
		File file = new File(workingPath_);
		if (file.exists() && file.list().length != 0) {
			return addIndexMem(userId);
		}
		Object obj;
		obj = database_.query(userId);
		if (obj == null) {
			return false;
		}
		byte[] bytes = (byte[]) obj;
		UserInfo info = UserInfoUnserializer.getInstance().unserialize(bytes,
				true);
		// System.out.println(info.toString());

		FSDirectory fsdir_info = null;
		IndexWriter writer_info = null;
		try {
			fsdir_info = FSDirectory.open(file);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		try {
			writer_info = new IndexWriter(fsdir_info, AnalyzerFactory
					.getInstance().getUserAnalyzer(), true,
					IndexWriter.MaxFieldLength.UNLIMITED);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		UserIndexView user = new UserIndexView(info);
		if (user == null)
			return false;
		try {
			Document doc = user.toDocument();
			if (doc == null) {
				return false;
			}
			// System.out.println(doc);
			writer_info.addDocument(doc);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}

		// 关闭索引
		try {
			writer_info.close();
		} catch (CorruptIndexException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}

		try {
			fsdir_info.close();
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		return addIndexMem(userId);
		// System.out.println("BuildIndex get indexwriter : " + userId);
	}
}