package com.xiaonei.search3.query.process.processors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.BooleanClause.Occur;

import com.xiaonei.search2.model.UserFieldNames;

/**
 * 
 * @author xuemin.luo xuemin.luo@opi-corp.com
 **/
public class HighSchoolProcessor {
	/**
	 * @param map
	 *            <p>
	 *            [highscl-->[id-->1331]</br>
	 *            ------------------[name-->厦门英才学校]</br>
	 *            ------------------[clazz1-->6]</br>
	 *            ------------------[calzz2-->7]</br>
	 *            ------------------[clazz3-->8]</br>
	 *            ------------------[year-->2003]]
	 *            </p>
	 * @param boost
	 *            float
	 **/
	public static List<BooleanQuery> process(
			Map<String, Map<String, String>> map, float boost) {
		// 解析成QueryListMap
		List<BooleanQuery> queryList = parseBasic(map);
		// 添加boost
		for (BooleanQuery query : queryList) {
			addBoost(query, boost);
		}
		return queryList;
	}

	public static String getType() {
		return "highScl";
	}

	private static List<BooleanQuery> parseBasic(
			Map<String, Map<String, String>> map) {
		// 存放所有的高中条件,每个高中条件都是一个BooleanQuery
		List<BooleanQuery> queryList = new ArrayList<BooleanQuery>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [highscl-->[id-->1331]</br>
			// ------------------[name-->厦门英才学校]</br>
			// ------------------[clazz1-->6]</br>
			// ------------------[calzz2-->7]</br>
			// ------------------[clazz3-->8]</br>
			// ------------------[year-->2003]]
			String typeOuter = outerEntry.getKey().split("\\|")[0];
			if (typeOuter.equals("highScl")) {// 对每所高中条件
				// [name-->厦门英才学校]
				// [clazz1-->6]
				// [calzz2-->7]
				// [clazz3-->8]
				// [year-->2003]
				// 每个大学的每个条件都是BooleanQuery里面的一个Query
				BooleanQuery booleanQuery = new BooleanQuery();
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {// 取出每所高中条件里面的数据项
					String typeInner = innerEntry.getKey();
					// booleanQuery = new BooleanQuery();
					if (typeInner.equals("id")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_ID,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("name")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_NAME,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("clazz1")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_CLASS1,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("clazz2")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_CLASS2,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("clazz3")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_CLASS3,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("year")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HIGH_SCHOOL_INFO_ENROLL_YEAR,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					}
				}
				queryList.add(booleanQuery);
				break;// 很重要
			}
		}
		return queryList;
	}

	private static BooleanQuery addBoost(BooleanQuery query, float boost) {
		query.setBoost(boost);
		return query;
	}

	public static void main(String args[]) {
		Map<String, Map<String, String>> outerMap = new HashMap<String, Map<String, String>>();
		Map<String, String> innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "厦门英才学校");
		innerMap.put("clazz1", "3");
		innerMap.put("clazz2", "3");
		innerMap.put("clazz3", "7");
		innerMap.put("year", "1998");
		outerMap.put("highScl|222", innerMap);
		innerMap.put("id", "23423");
		innerMap.put("name", "英才学校");
		innerMap.put("clazz1", "5");
		innerMap.put("clazz2", "5");
		innerMap.put("clazz3", "2");
		innerMap.put("year", "1999");
		outerMap.put("highScl|234", innerMap);
		List<BooleanQuery> queryAll = HighSchoolProcessor.process(outerMap,
				2.0f);
		for (BooleanQuery query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
