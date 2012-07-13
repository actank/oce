package com.xiaonei.xce.socialgraph.page;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * Page推荐数据结构
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 * 修改：添加带参数的构造函数RcdPageData(Item item)，从protobuf取数据
 */
public class RcdPageData {
	private int userId = 0;
	private int pageId = 0;
	private double pageValue = 0;
	private String pageName = "";
	private String description = "";
	private int fansNum = 0;
	private int commonSize = 0;
	private List<Integer> commonFriends = null;
	
	public RcdPageData() {
		commonFriends = new ArrayList<Integer>();
	}
	
	public RcdPageData(Item item) {
		commonFriends = new ArrayList<Integer>();
		this.pageId = item.getId();
		this.fansNum = item.getField();
		if (item.getFieldsCount() > 0) {
			//System.out.println("id= " + item.getFields(0));			
			this.commonFriends.add( item.getFields(0) );
		} else {
			this.commonFriends.add(0);
		}
		//this.commonFriends = item.getFieldsList();
	}
	public void setUserId(int userId) {
		this.userId = userId;
	}
	
	public int getUserId() {
		return this.userId;
	}
	
	public void setPageId(int pageId) {
		this.pageId = pageId;
	}
	
	public int getPageId() {
		return this.pageId;
	}
	
	public void setPageValue(double pageValue) {
		this.pageValue = pageValue;
	}
	
	public double getPageValue() {
		return this.pageValue;
	}
	
	public void setPageName(String pageName) {
		this.pageName = pageName;
	}
	
	public String getPageName() {
		return this.pageName;
	}
	
	public void setDescription(String description) {
		this.description = description;
	}
	
	public String getDescription() {
		return this.description;
	}
	
	public void setFansNum(int fansNum) {
		this.fansNum = fansNum;
	}
	
	public int getFansNum() {
		return this.fansNum;
	}
	
	public void setCommonSize(int commonSize) {
		this.commonSize = commonSize;
	}
	
	public int getCommonSize() {
		return this.commonSize;
	}
	
	public void setCommonFriends(List<Integer> commonFriends) {
		for (Integer id : commonFriends) {
			this.commonFriends.add(id);
		}
	}
	
	public List<Integer> getCommonFriends() {
		return this.commonFriends;
	}
}
