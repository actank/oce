/*
 * 作者：熊君武
 * 邮件：junwu.xiong@renren-inc.com
 * 说明：本文件用于首页推荐之小站推荐
 * 时间：2011-12-21
 */

package xce.socialgraph.recommend.minisite;

/*
 * 小站数据，包括小站名，小站id, 小站头像，小站url,小站关注人数
 */
public class RcdSiteData {
	private int userId = 0;      //user id
	private int siteId;          //site id
	private String url;          //site url   
    private String name;         //site name
    private String head;         //site head
    private int followerCount;   //site follower count
    
    public int getUserId() {
    	return userId;
    }
    public void setUserId(int userId) {
    	this.userId = userId;
    }
    
	public int getSiteId() {
		return siteId;
	}
	public void setSiteId(int siteId) {
		this.siteId = siteId;
	}
	public String getUrl() {
		return url;
	}
	public void setUrl(String url) {
		this.url = url;
	}    

	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getHead() {
		return head;
	}
	public void setHead(String head) {
		this.head = head;
	}	
	
	public int getFollowerCount() {
		return followerCount;
	}
    public void setFollowerCount(int followerCount) {
    	this.followerCount = followerCount;
    }
}