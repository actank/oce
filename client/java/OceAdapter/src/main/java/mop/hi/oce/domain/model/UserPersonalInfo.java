package mop.hi.oce.domain.model;

import java.io.Serializable;
import java.util.Map;

/**
 * 
 * UserPersonalInfo.java
 * 
 * @author drizzt drizzt.wang@gmail.com 2006-10-5 æ¶å¬ªå´04:00:56
 */
public class UserPersonalInfo implements Serializable {
	private static final long serialVersionUID = -4022715871191565448L;

	public static final String ID = "ID";

	public static final String INVITECODE = "INVITECODE";

	public static final String LINK = "LINK";

	public static final String LINKSTATUS = "LINKSTATUS";

	public static final String SPACEALL = "SPACE_ALL";

	public static final String LASTIPLONG = "LASTIP_LONG";

	private int id;

	private String inviteCode;

	private String link;

	private int linkStatus;

	private int spaceAll;

	private long lastipLong;

	private Map<String, String> properties;

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("UserPersonalInfo:\n");
		str.append("ID = ").append(this.getId()).append("\n");
		str.append("INVITECODE = ").append(getInviteCode()).append("\n");
		str.append("LINK = ").append(getLink()).append("\n");
		str.append("LINKSTATUS = ").append(getLinkStatus()).append("\n");
		str.append("SPACE_ALL = ").append(getSpaceAll()).append("\n");
		str.append("LASTIP_LONG = ").append(getLastipLong()).append("\n");

		return (str.toString());
	}

	public Map<String, String> getProperties() {
		return this.properties;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		this.properties.put(this.ID, String.valueOf(this.id));
	}

	public String getInviteCode() {
		return inviteCode;
	}

	public void setInviteCode(String inviteCode) {
		this.inviteCode = inviteCode;
		this.properties.put(this.INVITECODE, this.inviteCode);
	}

	public long getLastipLong() {
		return lastipLong;
	}

	public void setLastipLong(long lastipLong) {
		this.lastipLong = lastipLong;
		this.properties.put(this.LASTIPLONG, String.valueOf(this.lastipLong));
	}

	public String getLink() {
		return link;
	}

	public void setLink(String link) {
		this.link = link;
		this.properties.put(this.LINK, this.link);
	}

	public int getLinkStatus() {
		return linkStatus;
	}

	public void setLinkStatus(int linkStatus) {
		this.linkStatus = linkStatus;
		this.properties.put(this.LINKSTATUS, String.valueOf(this.linkStatus));
	}

	public int getSpaceAll() {
		return spaceAll;
	}

	public void setSpaceAll(int spaceAll) {
		this.spaceAll = spaceAll;
		this.properties.put(this.SPACEALL, String.valueOf(this.spaceAll));
	}

	public static UserPersonalInfo parser(Map<String, String> props) {
		UserPersonalInfo result = new UserPersonalInfo();

		result.setId(Integer.parseInt(props.get(UserPersonalInfo.ID)));
		result.setInviteCode(props.get(UserPersonalInfo.INVITECODE));
		result.setLastipLong(Long.parseLong(props
				.get(UserPersonalInfo.LASTIPLONG)));
		result.setLink(props.get(UserPersonalInfo.LINK));
		result.setLinkStatus(Integer.parseInt(props
				.get(UserPersonalInfo.LINKSTATUS)));
		result.setSpaceAll(Integer.parseInt(props
				.get(UserPersonalInfo.SPACEALL)));

		result.properties.clear();
		return result;
	}

}