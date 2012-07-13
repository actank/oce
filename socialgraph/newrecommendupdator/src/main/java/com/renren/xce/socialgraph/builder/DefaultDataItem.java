package com.renren.xce.socialgraph.builder;

import com.renren.xce.socialgraph.common.DataItem;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * 默认数据类型
 * @author zhangnan
 */
public class DefaultDataItem implements DataItem {
	private Item.Builder item = null;
	private long id = 0;
	
	public DefaultDataItem(long id, Item.Builder item) {
		this.id = id;
		this.item = item;
	}
	
	public long getId() {
		return this.id;
	}
	
	public Item.Builder getItem() {
		return this.item;
	}
	
}
