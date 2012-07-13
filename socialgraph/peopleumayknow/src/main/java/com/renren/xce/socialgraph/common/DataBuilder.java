package com.renren.xce.socialgraph.common;

import java.util.List;

/**
 * interface of creating specific data
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public interface DataBuilder {
	public List<DataItem> getData(int hostId);
	public String getBuilderType();
}
