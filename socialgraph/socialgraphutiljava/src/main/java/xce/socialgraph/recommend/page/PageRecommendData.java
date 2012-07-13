package xce.socialgraph.recommend.page;

import java.util.ArrayList;
import java.util.List;

/**
 * Page推荐数据结构
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class PageRecommendData {
	private int userId = 0;
	private int pageId = 0;
	private double pageValue = 0;
	private String pageName = "";
	private String description = "";
	private int fansNum = 0;
	private int commonSize = 0;
	private List<Integer> commonFriends = null;
	
	public PageRecommendData() {
		commonFriends = new ArrayList<Integer>();
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
