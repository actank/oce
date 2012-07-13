package xce.socialgraph.recommend.recommendbyinfonew;

/**
 * Structure of InfoItem used for RecommendByInfoNewUtil 
 * @author Administrator
 *
 */
public class InfoRecommendItem {
	private int userId;
	private String schoolName;
	
	public InfoRecommendItem(int userId, String schoolName) {
		this.userId = userId;
		this.schoolName = schoolName;
	}
	
	/**
	 * Get UserId
	 * @return
	 */
	public int getUserId() {
		return userId;
	}
	
	/**
	 * Get School name
	 * @return
	 */
	public String getSchoolName() {
		return schoolName;
	}
}
