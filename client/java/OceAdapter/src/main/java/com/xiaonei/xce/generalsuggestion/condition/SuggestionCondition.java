package com.xiaonei.xce.generalsuggestion.condition;


public class SuggestionCondition {
	private int userId;
	private String[] fields;
	private int begin;
	private int limit;
	private String condition;

	public SuggestionCondition(int userId, String condition, int offset, int limit) {
		this.userId = userId;
		this.condition = condition;
		this.begin = offset;
		this.limit = limit;
		this.fields = new String[] { SuggestionType.SUGGESTION_FRIEND,
				SuggestionType.SUGGESTION_RECOMMEND_USER,
				SuggestionType.SUGGESTION_UNIVERSITY,
				SuggestionType.SUGGESTION_WORKPLACE, 
				SuggestionType.SUGGESTION_PAGE,
				SuggestionType.SUGGESTION_APP };
	}

	public int getUserId() {
		return userId;
	}

	public void setUserId(int userId) {
		this.userId = userId;
	}

	public int getBegin() {
		return begin;
	}

	public void setBegin(int begin) {
		this.begin = begin;
	}

	public int getLimit() {
		return limit;
	}

	public void setLimit(int limit) {
		this.limit = limit;
	}

	public String getCondition() {
		return condition;
	}

	public void setCondition(String condition) {
		this.condition = condition;
	}

	public String[] getFields() {
		return fields;
	}
}
