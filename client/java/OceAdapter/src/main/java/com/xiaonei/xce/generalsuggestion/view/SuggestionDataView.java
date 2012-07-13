package com.xiaonei.xce.generalsuggestion.view;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import xce.generalsuggestion.IndexItem;
import xce.generalsuggestion.SuggestionResult;

import com.xiaonei.xce.generalsuggestion.condition.SuggestionType;

public class SuggestionDataView {

	private Map<String, List<Data>> type2Data;
	private String[] fields;

	public SuggestionDataView() {
		this(null, new String[0]);
	}

	public SuggestionDataView(SuggestionResult result, String[] fields) {
		type2Data = new HashMap<String, List<Data>>();
		if (result != null) {
			for (IndexItem item : result.results) {
				String type = item.type;
				List<Data> datas = type2Data.get(type);
				if (datas == null) {
					datas = new ArrayList<Data>();
					type2Data.put(type, datas);
				}
				for (Map<String, String> doc : item.docs) {
					int id = -1;
					String v = doc.get(SuggestionType.RESULT_ID);
					if (v != null && !v.isEmpty()) {
						id = Integer.parseInt(v);
					}
					String name = doc.get(SuggestionType.RESULT_NAME);
					Data data = new Data(id, name, type);
					datas.add(data);
				}
			}
		} 
		
		for (String field : fields) {
			if (!type2Data.containsKey(field)) {
				type2Data.put(field, new ArrayList<Data>());
			}
		}
		this.fields = fields;
	}
	
	public List<Data> getFriendData() {
		return getData(SuggestionType.SUGGESTION_FRIEND);
	}
	
	public List<Data> getRecommendData() {
		return getData(SuggestionType.SUGGESTION_RECOMMEND_USER);
	}
	
	public List<Data> getPageData() {
		return getData(SuggestionType.SUGGESTION_PAGE);
	}

	public List<Data> getAppData() {
		return getData(SuggestionType.SUGGESTION_APP);
	}
	
	public List<Data> getUniversityData() {
		return getData(SuggestionType.SUGGESTION_UNIVERSITY);
	}
	
	public List<Data> getWorkplaceData() {
		return getData(SuggestionType.SUGGESTION_WORKPLACE);
	}
	
	private List<Data> getData(String type) {
		List<Data> res = type2Data.get(type);
		if (res == null) {
			res = new ArrayList<Data>();
		}
		return res;
	}
	
	public List<Data> getAllData() {
		List<Data> result = new ArrayList<Data>();
		for(String type : fields) {
			result.addAll(type2Data.get(type));
		}
		return result;
	}
	
	public static class Data {
		private int id;
		private String name;
		private String type;

		public String getType() {
			return type;
		}

		public void setType(String type) {
			this.type = type;
		}

		public Data(int id, String name, String type) {
			this.id = id;
			this.name = name;
			this.type = type;
		}

		public int getId() {
			return this.id;
		}

		public String getName() {
			return this.name;
		}

		@Override
		public String toString() {
			return "ID: " + id + "  Name: " + name;
		}
	}
}
