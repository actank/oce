package com.xiaonei.search3.query.process.processors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;

import com.xiaonei.search2.model.UserFieldNames;

/**
 * 
 * @author xuemin.luo xuemin.luo@opi-corp.com
 **/
public class WorkPlaceProcessor {
	public static List<Query> process(Map<String, Map<String, String>> map,
			float boost) {
		// 解析成QueryListMap
		List<Query> queryList = parseBasic(map);
		// 添加boost
		for (Query query : queryList) {
			addBoost(query, boost);
		}
		return queryList;
	}

	public static String getType() {
		return "workplace";
	}

	private static List<Query> parseBasic(Map<String, Map<String, String>> map) {
		ArrayList<Query> queryList = new ArrayList<Query>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [workplace-->[workplace-->人人网]]
			String typeOuter = outerEntry.getKey();
			if (typeOuter.equals("workplace")) {
				// [workplace-->人人网]
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {
					String typeInner = innerEntry.getKey();
					if (typeInner.equals("workplace")) {
						Query query = new TermQuery(new Term(
								UserFieldNames.WORKPLACE_INFO_NAME,
								innerEntry.getValue()));
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
		innerMap.put("workplace", "You Raise Me Up");
		innerMap.put("workplace", "You Raise Me Up");
		outerMap.put("workplace", innerMap);
		List<Query> queryAll = WorkPlaceProcessor.process(outerMap, 2.0f);
		for (Query query : queryAll) {
			System.out.println(query.toString());
		}
	}
}
