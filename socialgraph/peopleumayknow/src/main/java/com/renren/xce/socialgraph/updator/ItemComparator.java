package com.renren.xce.socialgraph.updator;

import java.util.Comparator;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item.Builder;

/**
 * Comparator between Item.Builder
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class ItemComparator implements Comparator<Item.Builder>{

	public int compare(Builder o1, Builder o2) {
		return o2.getField() - o1.getField();
	}
}
