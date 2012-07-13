package xce.socialgraph.recommend.friendcluster;

import java.util.ArrayList;
import java.util.List;

/**
 * 提供给业务者使用的好友簇数据结构
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class Recommend {

    /** ugly!!! shit! */
    public static final String bizName = "FFUCR";

    private int hostId;
    private int rcdId;
    private int commonFriendCount;
    private List<Integer> commonFriendList;

    public Recommend(int hostId, int rcdId, int commonFriendCount,
	    List<Integer> commonFriendList) {
	super();
	this.hostId = hostId;
	this.rcdId = rcdId;
	this.commonFriendCount = commonFriendCount;
	if (commonFriendList != null) {
	    this.commonFriendList = commonFriendList;
	} else {
	    this.commonFriendList = new ArrayList<Integer>();
	}
    }

    public int getHostId() {
	return hostId;
    }

    public void setHostId(int hostId) {
	this.hostId = hostId;
    }

    public int getRcdId() {
	return rcdId;
    }

    public void setRcdId(int rcdId) {
	this.rcdId = rcdId;
    }

    public int getCommonFriendCount() {
	return commonFriendCount;
    }

    public void setCommonFriendCount(int commonFriendCount) {
	this.commonFriendCount = commonFriendCount;
    }

    public List<Integer> getCommonFriendList() {
	return commonFriendList;
    }

    public void setCommonFriendList(List<Integer> commonFriendList) {
	this.commonFriendList = commonFriendList;
    }

    public String toString() {
	StringBuffer sb = new StringBuffer();
	sb.append("host:").append(hostId).append("\t");
	sb.append("rcdId:").append(rcdId).append("\t");
	sb.append("commonFriendCount:").append(commonFriendCount).append("\t");
	sb.append("commonFriendList").append(commonFriendList);
	return sb.toString();
    }

}
