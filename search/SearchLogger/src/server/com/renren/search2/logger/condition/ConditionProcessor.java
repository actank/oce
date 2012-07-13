package com.renren.search2.logger.condition;

import java.util.Map;

import com.xiaonei.search2.condition.AbstractCondition;
import com.xiaonei.search2.condition.OpenPeopleSearchCondition;
import com.xiaonei.search2.model.IndexCondition;

public class ConditionProcessor {
	public static IndexCondition getIndexCondition(Map<String, String> log) {
		AbstractCondition condition = new OpenPeopleSearchCondition();
		condition.queryCondition.putQueryMap("query", log.get("query"));
		condition.addOpUser(Integer.parseInt(log.get("hostId")));
		return condition.buildIndexCondition();
	}
}
