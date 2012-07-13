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
public class AstrologyProcessor {
	public static Query process(Map<String, Map<String, String>> map,
			float boost) {
		// 解析成QueryListMap
		List<Query> queryList = parseBasic(map);
		// 添加boost
		for (Query queryTemp : queryList) {
			Query query = addBoost(queryTemp, boost);
			return query;
		}
		return new TermQuery(new Term("astrology", ""));
	}

	public static String getType() {
		return "astrology";
	}

	private static List<Query> parseBasic(Map<String, Map<String, String>> map) {
		ArrayList<Query> queryList = new ArrayList<Query>();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			// [astrology-->[astrology-->天秤]]
			String typeOuter = outerEntry.getKey();
			if (typeOuter.equals("astrology")) {
				// [astrology-->天秤]
				for (Entry<String, String> innerEntry : outerEntry.getValue()
						.entrySet()) {
					String typeInner = innerEntry.getKey();
					if (typeInner.equals("astrology")) {
						Query query = new TermQuery(new Term(UserFieldNames.ASTROLOGY,
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
		innerMap.put("astrology", "白羊座");
		innerMap.put("astrology", "处女座");
		outerMap.put("astrology", innerMap);
		Query query = AstrologyProcessor.process(outerMap, 2.0f);
		System.out.println(query.toString());
	}
}
