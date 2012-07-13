/*
 * Function: Blog recommendation data structure
 * Author:   XiongJunWu
 * Email:    junwu.xiong@renren-inc.com
 * Date:     2011-11-01
 * */

package xce.socialgraph.recommend.blog;

public class BlogRecommendData {
	//blog item information initialization
	private int user_id = 0;
	private long blog_id = 0;
    private long share_id = 0;
	private int share_user_id = 0;
//	private int stage = 0;
//	private int type = 0;
	private String title = "";
	private String url = "";
	private String summary = "";
	private String url_md5 = "";
	private String share_user_name = "";
	private int share_count = 0;
  
  //set and get blog item information
  public void setUserId(int user_id) {
	  this.user_id = user_id;
  }
  
  public int getUserId() {
	  return this.user_id;
  }
  
  public void setBlogId(long blog_id) {
	  this.blog_id = blog_id;
  }
  
  public long getBlogId() {
	  return this.blog_id;
  }

  public void setShareId(long share_id) {
          this.share_id = share_id;
  } 
 
  public long getShareId() {
 	  return this.share_id;
  }  
  
  public void setShareUserId(int share_user_id) {
	  this.share_user_id = share_user_id;	  
  }
  
  public int getShareUserId() {
	  return this.share_user_id;  
  }
  
//  public void setStage(int stage) {
//	  this.stage = stage;  
//  }
//  
//  public int getStage() {
//	  return this.stage;
//  }
//  
//  public void setType(int type) {
//	  this.type = type;
//  }
//  
//  public int getType() {
//	  return this.type;
//  }
  
  public void setTitle(String title) {
	  this.title = title;
  }
  
  public String getTitle() {
	  return this.title;
  }
  
  public void setUrl(String url) {
	  this.url = url;
  }
  
  public String getUrl() {
	  return this.url;
  }
  
  public void setSummary(String summary) {
	  this.summary = summary;
  }
  
  public String getSummary() {
	  return this.summary;
  }
  
  public void setUrlMd5(String url_md5) {
	  this.url_md5 = url_md5;
  }
  
  public String getUrlMd5() {
	  return this.url_md5;
  }
  public void setShareUserName(String share_user_name) {
	  this.share_user_name = share_user_name;
  }
  
  public String getShareUserName() {
	  return this.share_user_name;
  }
  
  public void setShareCount(int share_count) {
	  this.share_count = share_count;
  }
  
  public int getShareCount() {
	  return this.share_count;
  }  
  
}
