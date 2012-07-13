package com.xiaonei.search3.query.process.processors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause.Occur;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;

import com.xiaonei.search2.model.UserFieldNames;

/**
 * 
 * @author xuemin.luo xuemin.luo@opi-corp.com
 **/
public class RegionProcessor {
	/**
	 * @param map
	 *            <p>
	 *            [region---></br> ------------------[province_name-->北京市]</br>
	 *            ------------------[province_id-->123]</br>
	 *            ------------------[city_name-->朝阳区]</br>
	 *            ------------------[city_id-->123]]</br>
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
		return "region";
	}

	private static List<BooleanQuery> parseBasic(
			Map<String, Map<String, String>> map) {
		// 存放所有的地区条件,每个地区条件都是一个BooleanQuery
		List<BooleanQuery> queryList = new ArrayList<BooleanQuery>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [region---></br>
			// ------------------[province_name-->北京市]</br>
			// ------------------[province_id-->123]</br>
			// ------------------[city_name-->朝阳区]</br>
			// ------------------[city_id-->123]]</br>
			String typeOuter = outerEntry.getKey().split("\\|")[0];
			if (typeOuter.equals("region")) {// 对每所大学条件
				// ------------------[province_name-->北京市]</br>
				// ------------------[province_id-->123]</br>
				// ------------------[city_name-->朝阳区]</br>
				// ------------------[city_id-->123]]</br>
				// 每个地区的每个条件都是BooleanQuery里面的一个Query
				BooleanQuery booleanQuery = new BooleanQuery();
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {// 取出每所大学条件里面的数据项
					String typeInner = innerEntry.getKey();
					// booleanQuery = new BooleanQuery();
					if (typeInner.equals("province_id")) {
						Query query = new TermQuery(new Term("province_id",
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("province_name")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.HOME_PROVINCE,
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("city_id")) {
						Query query = new TermQuery(new Term("city_id",
								innerEntry.getValue()));
						booleanQuery.add(query, Occur.MUST);
					} else if (typeInner.equals("city_name")) {
						Query query = new TermQuery(
								new Term(UserFieldNames.HOME_CITY,
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
		innerMap.put("province_id", "1234");
		innerMap.put("province_name", "哈工大");
		innerMap.put("city_id", "1234");
		innerMap.put("city_name", "哈工大");
		outerMap.put("region|222", innerMap);
		innerMap.put("province_id", "1234");
		innerMap.put("province_name", "哈工大");
		innerMap.put("city_id", "1234");
		innerMap.put("city_name", "哈工大");
		outerMap.put("region|222", innerMap);
		List<BooleanQuery> queryAll = RegionProcessor.process(outerMap, 2.0f);
		for (BooleanQuery query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
