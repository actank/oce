package com.renren.xce.socialgraph.common;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * basic interface of item of data
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public interface DataItem {
	public long getId();
	public Item.Builder getItem();
}
