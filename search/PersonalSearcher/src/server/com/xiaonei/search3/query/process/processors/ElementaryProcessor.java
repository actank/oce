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
public class ElementaryProcessor {
	/**
	 * @param map
	 *            <p>
	 *            [elementary|3504-->[id-->1331]</br>
	 *            ------------------[name-->福建省沙县实验小学]</br>
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
		return "elementary";
	}

	private static List<BooleanQuery> parseBasic(
			Map<String, Map<String, String>> map) {
		// 存放所有的小学条件,每个小学条件都是一个BooleanQuery
		List<BooleanQuery> queryList = new ArrayList<BooleanQuery>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [elementary|3504-->[id-->1331]
			// -------------[name-->福建省沙县实验小学]
			// -------------[year-->2006]]
			String typeOuter = outerEntry.getKey().split("\\|")[0];
			if (typeOuter.equals("elementary")) {// 对每所小学条件
				// [id-->1331]
				// [name-->福建省沙县实验小学]
				// [year-->2006]
				// 每个小学的每个条件都是BooleanQuery里面的一个Query
				BooleanQuery booleanQuery = new BooleanQuery();
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {// 取出每所小学条件里面的数据项
					String typeInner = innerEntry.getKey();
					// booleanQuery = new BooleanQuery();
					if (typeInner.equals("id")) {
						Query query = new TermQuery(
								new Term(
										UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_ID,
										innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("name")) {
						Query query = new TermQuery(
								new Term(
										UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_NAME,
										innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("year")) {
						Query query = new TermQuery(
								new Term(
										UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_YEAR,
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
		innerMap.put("name", "福建省沙县实验小学");
		innerMap.put("year", "2006");
		outerMap.put("elementary|222", innerMap);
		innerMap.put("id", "56456");
		innerMap.put("name", "厦门市英才学校");
		innerMap.put("year", "2007");
		outerMap.put("elementary|231", innerMap);
		List<BooleanQuery> queryAll = ElementaryProcessor.process(outerMap,
				2.0f);
		for (BooleanQuery query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
