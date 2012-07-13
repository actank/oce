// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1

package externalsearch;

import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicInteger;

import com.xiaonei.search2.Config;
import com.xiaonei.search2.model.IndexResult;
import com.xiaonei.search2.model.IndexResultItem;
import com.xiaonei.search2.model.IndexType;

import engine.search.BooleanQuery;
import engine.search.ConditionToQuery;
import engine.search.IndexReader;
import engine.search.IndexSearcher;
import engine.search.TermQuery;
import engine.search.TransformCondition;

public final class ExternalSearcherI extends _ExternalSearcherDisp {
	private static Map<String, String> keymap = new HashMap<String, String>();

	private IndexSearcher[] searchers = new IndexSearcher[2];
	private AtomicInteger cur = new AtomicInteger(-1);

	private String curIndex = "";

	private static String dir = "/data/xce/XiaoNeiSearch/index3";
	private CheckThread thread = new CheckThread(dir, this);

	public void reloadIndex(String path) throws IOException,
			InterruptedException {
		if (path.equals(curIndex)) {
			return;
		}
		long start = System.currentTimeMillis();
		System.out.println("加载索引 " + dir);
		File file = new File(dir + "/" + path);
		String[] indexs = file.list();
		System.out.println(indexs[0]);
		List<IndexReader> readers = new ArrayList<IndexReader>();
		for (int i = 0; i < indexs.length; i++) {
			readers.add(i, new IndexReader(dir + "/" + path + "/" + indexs[i]));
		}
		int pos = (cur.get() + 1) % 2;
		if (readers.size() == 0) {
			searchers[pos] = null;
		} else {
			searchers[pos] = new IndexSearcher(
					readers.toArray(new IndexReader[readers.size()]));
		}

		cur.incrementAndGet();
		curIndex = path;
		long end = System.currentTimeMillis();
		long cost = end - start;
		System.out.println("加载结束：cost：" + cost);
		System.out.println("keymap size:" + keymap.size());
		for (Map.Entry<String, String> entry : keymap.entrySet()) {
			System.out.println("keymap key:" + entry.getKey() + " value:"
					+ entry.getValue());
		}
		Thread.sleep(10000);
		searchers[1 - pos] = null;
		System.gc();
	}

	public ExternalSearcherI() {
		thread.start();
	}

	static {
		keymap.put("GENERAL.gender", "gender");
		keymap.put("GENERAL.range.age", "age");
		keymap.put("WORKPLACE_INFO.workplace_name", "workplace");
		keymap.put("JS_INFO.junior_high_school_info.junior_high_school_year",
				"junior.year");
		keymap.put("JS_INFO.junior_high_school_info.junior_high_school_name",
				"junior.name");
		keymap.put("UNIVERSITY_INFO.enroll_year", "univ.year");
		keymap.put("UNIVERSITY_INFO.university_id", "univ.name");
		keymap.put("GENERAL.home_province", "homeprovince");
		keymap.put("GENERAL.home_city", "homecity");
		keymap.put("HIGH_SCHOOL_INFO.high_school_name", "high.name");
		keymap.put("HIGH_SCHOOL_INFO.enroll_year", "high.year");
	}

	public static class Result implements Serializable {
		public int resultHits = 0;
		public int totalHits = 0;
		public Vector<IndexResultItem> hitsResult = new Vector<IndexResultItem>();
	}

	@SuppressWarnings("unused")
	public IndexResult search(
			java.util.Map<java.lang.String, java.lang.String> condition,
			int begin, int limit, Ice.Current __current) {
		if (cur.get() < 0) {
			System.out.println("Index is Not ready");
			return null;
		}
		IndexSearcher searcher = searchers[cur.get() % 2];
		if (searcher == null) {
			System.out.println("Index is Not ready");
			return null;
		}
		SimpleDateFormat dateFm = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");

		String dateTime = dateFm.format(new java.util.Date());
		System.out.println(dateTime);

		long start = System.currentTimeMillis();

		int realBegin = Math.min(Config.HARD_LIMIT, begin);
		if (realBegin + limit > Config.HARD_LIMIT) {
			realBegin = Config.HARD_LIMIT - limit;
		}

		System.out.println("begin:" + begin + " limit:" + limit + " realBegin:"
				+ realBegin);

		int needsTotal = realBegin + limit;

		TransformCondition transformer = new TransformCondition(condition,
				keymap);

		Map<String, String> query = transformer.transform();

		for (Map.Entry<String, String> entry : query.entrySet()) {
			System.out.println("AFTER TRANSFORM key:" + entry.getKey()
					+ " value:" + entry.getValue());
		}

		Result result_user = new Result();
		IndexResult indexResult = new IndexResult();
		indexResult.typedCount = new HashMap<IndexType, Integer>();

		long getQueryStart = System.currentTimeMillis();
		ConditionToQuery termQuery = new ConditionToQuery(
				searcher.getReaders()[0]);
		BooleanQuery booleanQuery = new BooleanQuery();
		try {
			booleanQuery = termQuery.getQuery(query);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		long getQueryEnd = System.currentTimeMillis();
		long searchStart = System.currentTimeMillis();
		ArrayList<Integer> ids = null;
		try {
			ids = searcher.search(booleanQuery, needsTotal);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		long searchEnd = System.currentTimeMillis();
		long getUserIdStart = System.currentTimeMillis();

		if (ids == null || realBegin >= ids.size() || realBegin < 0) {
			System.out.println("result is null, return");
			return indexResult;
		}

		for (int id : ids) {
			System.out.println("id:" + id);
		}

		long getUserIdEnd = System.currentTimeMillis();
		long getIndexResultStart = System.currentTimeMillis();
		for (int id : ids) {
			HashMap<String, String> content = new HashMap<String, String>();
			// content.put("is_friend", "0");
			result_user.hitsResult.add(new IndexResultItem(IndexType.User, id,
					content));
		}
		System.out.println("Searcher: " + cur.get() + " begin:" + realBegin
				+ " limit:" + limit + " result_user.size:"
				+ result_user.hitsResult.size());
		result_user.resultHits = result_user.hitsResult.size();
		result_user.totalHits = ids.size();

		List<IndexResultItem> result_app = null;
		List<IndexResultItem> result_page = null;
		int result_app_size = 0;
		int result_page_size = 0;
		int result_app_totalsize = 0;
		int result_page_totalsize = 0;

		List<IndexResultItem> hitsResult = new Vector<IndexResultItem>(
				needsTotal);

		hitsResult.addAll(result_user.hitsResult);

		long getIndexResultEnd = System.currentTimeMillis();
		long end = System.currentTimeMillis();
		indexResult.currentPos = realBegin;
		indexResult.timeCost = end - start;
		indexResult.totalCount = result_user.totalHits;
		indexResult.typedCount.put(IndexType.User, result_user.totalHits);
		indexResult.contents = hitsResult
				.toArray(new IndexResultItem[hitsResult.size()]);

		StringBuilder builder = new StringBuilder();
		builder.append("KEY:");

		for (Map.Entry<String, String> entry : condition.entrySet()) {
			// System.out.println("AFTER TRANSFORM key:" + entry.getKey()
			// + " value:" + entry.getValue());
			builder.append(entry.getKey() + ":" + entry.getValue() + ",");
		}

		// System.out.println("hitsResult.size:" + hitsResult.size()
		// + " contents.size:" + indexResult.contents.length);

		builder.append("; hitsResult.size:" + hitsResult.size()
				+ "; contents.size:" + indexResult.contents.length);

		// System.out.println("totalCount:" + indexResult.totalCount);
		// System.out.println("totalCost:" + (end - start) + "ms; getQueryCost:"
		// + (getQueryEnd - getQueryStart) + "ms; searchCost:"
		// + (searchEnd - searchStart) + "ms; getUserIdCost:"
		// + (getUserIdEnd - getUserIdStart) + "ms; getIndexResultCost:"
		// + (getIndexResultEnd - getIndexResultStart));
		builder.append("; totalCost:" + (end - start) + "; getQueryCost:"
				+ (getQueryEnd - getQueryStart) + "; searchCost:"
				+ (searchEnd - searchStart) + "; getUserIdCost:"
				+ (getUserIdEnd - getUserIdStart) + "; getIndexResultCost:"
				+ (getIndexResultEnd - getIndexResultStart));
		System.out.println(builder.toString());
		return indexResult;
	}
}
