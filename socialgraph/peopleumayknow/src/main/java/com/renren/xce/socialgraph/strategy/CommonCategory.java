package com.renren.xce.socialgraph.strategy;

import com.renren.xce.socialgraph.common.Category;

/**
 * default category class
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class CommonCategory implements Category {
	private String type = "";
	private int num = 0;
	private float weight = 0;
	
	public CommonCategory(String type, int num, float weight) {
		this.type = type;
		this.num = num;
		this.weight = weight;
	}

	/**
	 * get type of category
	 */
	public String getType() {
		return this.type;
	}

	/**
	 * get number of this category in recommendation
	 */
	public int getNum() {
		return this.num;
	}

	/**
	 * get weight of this category 
	 */
	public float getWeight() {
		return this.weight;
	}
}
