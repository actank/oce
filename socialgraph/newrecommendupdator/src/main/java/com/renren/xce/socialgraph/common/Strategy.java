package com.renren.xce.socialgraph.common;

import java.util.List;

/**
 * basic interface of strategy that point out feature of each person
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public interface Strategy {
	public List<Category> getStrategy(int userId);
}
