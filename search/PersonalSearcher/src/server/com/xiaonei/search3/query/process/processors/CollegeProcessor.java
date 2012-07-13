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
public class CollegeProcessor {
	/**
	 * @param map
	 *            <p>
	 *            [college|3504-->[id-->1331]</br>
	 *            ------------------[name-->蓝翔技校]</br>
	 *            ------------------[department-->西点厨师学院]</br>
	 *            ------------------[year-->2006]]
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
		return "college";
	}

	private static List<BooleanQuery> parseBasic(
			Map<String, Map<String, String>> map) {
		// 存放所有的中专条件,每个中专条件都是一个BooleanQuery
		List<BooleanQuery> queryList = new ArrayList<BooleanQuery>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [college|3504-->[id-->1331]
			// -------------[name-->蓝翔技校]
			// -------------[department-->西点厨师学院]
			// -------------[year-->2006]]
			String typeOuter = outerEntry.getKey().split("\\|")[0];
			if (typeOuter.equals("college")) {// 对每所中专条件
				// [id-->1331]
				// [name-->蓝翔技校]
				// [department-->西点厨师学院]
				// [year-->2006]
				// 每个中专的每个条件都是BooleanQuery里面的一个Query
				BooleanQuery booleanQuery = new BooleanQuery();
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {// 取出每所中专条件里面的数据项
					String typeInner = innerEntry.getKey();
					// booleanQuery = new BooleanQuery();
					if (typeInner.equals("id")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.COLLEGE_INFO_COLLEGE_ID,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("name")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.COLLEGE_INFO_COLLEGE_NAME,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("department")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.COLLEGE_INFO_DEPARTMENT,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("year")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.COLLEGE_INFO_ENROLL_YEAR,
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
		innerMap.put("name", "蓝翔技校");
		innerMap.put("department", "计算机科学与技术");
		innerMap.put("year", "2006");
		outerMap.put("college|222", innerMap);
		innerMap.put("id", "56456");
		innerMap.put("name", "北京朝阳社区学院");
		innerMap.put("department", "软件学院");
		innerMap.put("year", "2007");
		outerMap.put("college|231", innerMap);
		List<BooleanQuery> queryAll = CollegeProcessor.process(outerMap, 2.0f);
		for (BooleanQuery query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
