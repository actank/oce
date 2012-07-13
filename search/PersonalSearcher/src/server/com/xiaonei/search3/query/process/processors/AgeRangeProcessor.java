package com.xiaonei.search3.query.process.processors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.search.NumericRangeQuery;
import org.apache.lucene.search.Query;

import com.xiaonei.search2.model.UserFieldNames;

/**
 * 
 * @author xuemin.luo xuemin.luo@opi-corp.com
 **/
public class AgeRangeProcessor {
	public static List<NumericRangeQuery<Integer>> process(
			Map<String, Map<String, String>> map, float boost) {
		// 解析成QueryListMap
		List<NumericRangeQuery<Integer>> queryList = parseBasic(map);
		// 添加boost
		for (Query query : queryList) {
			addBoost(query, boost);
		}
		return queryList;
	}

	public static String getType() {
		return "ageRange";
	}

	private static List<NumericRangeQuery<Integer>> parseBasic(
			Map<String, Map<String, String>> map) {
		ArrayList<NumericRangeQuery<Integer>> queryList = new ArrayList<NumericRangeQuery<Integer>>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [ageRange-->[ageRange-->0]]
			String typeOuter = outerEntry.getKey();
			if (typeOuter.equals("ageRange")) {
				// [ageRange-->0]
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {
					String typeInner = innerEntry.getKey();
					if (typeInner.equals("ageRange")) {
						int range = Integer.parseInt(innerEntry.getValue());
						int min = 0;
						int max = 0;
						switch (range) {
						case 0:
							min = 1;
							max = 100;
							break;
						case 1:
							min = 1;
							max = 15;
							break;
						case 2:
							min = 16;
							max = 22;
							break;
						case 3:
							min = 23;
							max = 30;
							break;
						case 4:
							min = 31;
							max = 40;
							break;
						case 5:
							min = 41;
							max = 100;
							break;
						default:
							min = 1;
							max = 100;
							break;
						}
						NumericRangeQuery<Integer> query = NumericRangeQuery
								.newIntRange(UserFieldNames.AGE, min, max, true, true);
						queryList.add(query);
					}
					break;// 很重要
				}
				break;// 很重要
			}
		}
		return queryList;
	}

	private static Query addBoost(Query query, float boost) {
		query.setBoost(boost);
		return query;
	}

	public static void main(String args[]) {
		Map<String, Map<String, String>> outerMap = new HashMap<String, Map<String, String>>();
		Map<String, String> innerMap = new HashMap<String, String>();
		innerMap.put("ageRange", "3");
		outerMap.put("ageRange", innerMap);
		List<NumericRangeQuery<Integer>> queryAll = AgeRangeProcessor.process(
				outerMap, 2.0f);
		for (Query query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
