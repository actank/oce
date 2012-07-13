package mop.hi.oce.domain.user;

import java.io.Serializable;
import java.util.Date;
import java.util.Map;
import com.xiaonei.xce.log.Oce;

import mop.hi.oce.domain.Markable;
import xce.util.tools.DateFormatter;


/**
 * 
 * UserDesc.java
 * 
 * @author drizzt drizzt.wang@gmail.com 2006-5-7 4:00:47
 */
public class UserDesc extends Markable implements Serializable {
	private static final long serialVersionUID = -2573032935427223078L;

	public static final int flagNotUpdate = 3;

	public static final int flagUpdated = 7;

	public static final int userbanned = 1;

	public static final int deuserban = 0;

	public static final String ID = "ID";

	public static final String REGISTERTIME = "REGISTERTIME";

	public static final String LASTLOGINTIME = "LASTLOGINTIME";

	public static final String LASTLOGOUTTIME = "LASTLOGOUTTIME";

	public static final String MSN = "MSN";

	public static final String HOMEPAGE = "HOMEPAGE";

	public static final String MOBILEPHONE = "MOBILEPHONE";

	public static final String FIXPHONE = "FIXPHONE";

	public static final String CONTACTCONTROL = "CONTACTCONTROL";

	public static final String QQ = "QQ";

	public static final String MAINURL = "MAINURL";

	public static final String DESC = "DESCR";

	public static final String DESCBAN = "DESCBAN";

	public static final String ACTIVATETIME = "ACTIVATETIME";

	public static final String FAV = "FAV";

	public static final String FLAGUPDATE = "FAV_FLAG";

	public static final String TEMPLATE = "TEMPLATE";

	public static final String LAST_MODIFY_TIME = "LAST_MODIFY_TIME";

	// public static final String ID ="lastemailnotify";

	protected int id;

	protected String desc;

	protected String mainUrl;// = Globals.userMainUrl;

	protected String qq;

	protected String msn;

	protected String homepage;

	protected int contactControl;// = Relation1.uuGuestRequest;

	protected String mobilePhone;

	protected String fixPhone;

	protected Date registerTime;// = new Date();

	protected Date lastLoginTime;// = new Date();

	protected Date lastLogoutTime;// = new Date();

	protected Date activateTime;// = new Date();

	protected int flagUpdate = flagNotUpdate;

	protected String fav;

	protected int descBan;

	protected int template;

	protected Date lastModifyTime;

	// private Date lastEmailNotify = new Date();

	public String toString() {
		StringBuffer str = new StringBuffer();
		try {
			str.append("UserDesc:\n");
			str.append("ID = ").append(this.getId()).append("\n");
			str.append("REGISTERTIME = ").append(getRegisterTime())
					.append("\n");
			str.append("LASTLOGINTIME = ").append(getLastLoginTime()).append(
					"\n");
			str.append("LASTLOGOUTTIME = ").append(getLastLogoutTime()).append(
					"\n");
			str.append("MSN = ").append(getMsn()).append("\n");
			str.append("HOMEPAGE = ").append(getHomepage()).append("\n");
			str.append("FIXPHONE = ").append(getFixPhone()).append("\n");
			str.append("CONTACTCONTROL = ").append(getContactControl()).append(
					"\n");
			str.append("QQ = ").append(getQq()).append("\n");
			str.append("MAINURL = ").append(getMainUrl()).append("\n");
			str.append("DESC = ").append(getDesc()).append("\n");
			str.append("DESCBAN = ").append(getDescBan()).append("\n");
			str.append("ACTIVATETIME = ").append(getActivateTime())
					.append("\n");
			str.append("FAV = ").append(getFav()).append("\n");
			str.append("FLAGUPDATE = ").append(getFlagUpdate()).append("\n");
			str.append("TEMPLATE = ").append(getTemplate()).append("\n");
			str.append("LASTMODIFYTIME = ").append(
					getLastModifyTime().toString()).append("\n");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " " + e);
		}
		return (str.toString());
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		// id will not update
		// this.properties.put(this.ID,String.valueOf(this.id));
	}

	public String getDesc() {
		return desc;
	}

	public void setDesc(String desc) {
		this.desc = desc;
		mark(this.DESC, this.desc);
	}

	public int getContactControl() {
		return contactControl;
	}

	public void setContactControl(int contactControl) {
		this.contactControl = contactControl;
		mark(this.CONTACTCONTROL, String.valueOf(this.contactControl));
	}

	public String getHomepage() {
		return homepage;
	}

	public void setHomepage(String homepage) {
		this.homepage = homepage;
		mark(this.HOMEPAGE, this.homepage);
	}

	public Date getLastLoginTime() {
		return lastLoginTime;
	}

	public void setLastLoginTime(Date lastLoginTime) {
		this.lastLoginTime = lastLoginTime;
		mark(this.LASTLOGINTIME, DateFormatter.format(this.lastLoginTime));
	}

	public Date getLastLogoutTime() {
		return lastLogoutTime;
	}

	public void setLastLogoutTime(Date lastLogoutTime) {
		this.lastLogoutTime = lastLogoutTime;
		mark(this.LASTLOGOUTTIME, DateFormatter.format(this.lastLogoutTime));
	}

	public String getMainUrl() {
		return mainUrl;
	}

	public void setMainUrl(String mainUrl) {
		this.mainUrl = mainUrl;
		mark(this.MAINURL, this.mainUrl);
	}

	public String getMobilePhone() {
		return mobilePhone;
	}

	public void setMobilePhone(String mobilePhone) {
		this.mobilePhone = mobilePhone;
		mark(this.MOBILEPHONE, this.mobilePhone);
	}

	public String getMsn() {
		return msn;
	}

	public void setMsn(String msn) {
		this.msn = msn;
		mark(this.MSN, this.msn);
	}

	public String getQq() {
		return qq;
	}

	public void setQq(String qq) {
		this.qq = qq;
		mark(this.QQ, this.qq);
	}

	public Date getRegisterTime() {
		return registerTime;
	}

	public void setRegisterTime(Date registerTime) {
		this.registerTime = registerTime;
		if (registerTime == null) {
			return;
		}
		mark(this.REGISTERTIME, DateFormatter.format(this.registerTime));
	}

	public String getFixPhone() {
		return fixPhone;
	}

	public void setFixPhone(String fixPhone) {
		this.fixPhone = fixPhone;
		mark(this.FIXPHONE, this.fixPhone);
	}

	public String getFav() {
		return fav;
	}

	public void setFav(String fav) {
		this.fav = fav;
		mark(this.FAV, this.fav);
	}

	public int getFlagUpdate() {
		return flagUpdate;
	}

	public void setFlagUpdate(int flagUpdate) {
		this.flagUpdate = flagUpdate;
		mark(this.FLAGUPDATE, String.valueOf(this.flagUpdate));
	}

	public Date getActivateTime() {
		return activateTime;
	}

	public void setActivateTime(Date activateTime) {
		this.activateTime = activateTime;
		if (activateTime == null) {
			return;
		}
		mark(this.ACTIVATETIME, DateFormatter.format(this.activateTime));
	}

	public int getDescBan() {
		return descBan;
	}

	public void setDescBan(int descBan) {
		this.descBan = descBan;
		mark(this.DESCBAN, String.valueOf(this.descBan));
	}

	public int getTemplate() {
		return this.template;
	}

	public void setTemplate(int template) {
		this.template = template;
		mark(this.TEMPLATE, String.valueOf(this.template));
	}

	public Date getLastModifyTime() {
		return this.lastModifyTime;
	}

	public void setLastModifyTime(Date lastModifyTime) {
		this.lastModifyTime = lastModifyTime;
		if (lastModifyTime == null) {
			return;
		}
		mark(this.LAST_MODIFY_TIME, DateFormatter.format(this.lastModifyTime));
	}

	//
	// public Date getLastEmailNotify() {
	// return lastEmailNotify;
	// }
	//
	// public void setLastEmailNotify(Date lastEmailNotify) {
	// this.lastEmailNotify = lastEmailNotify;
	// }

	public UserDesc parse(Map<String, String> props) {
		// do not catch parsing id
		this.id = Integer.parseInt(props.get(UserDesc.ID));

		this.activateTime = DateFormatter.parse(props
				.get(UserDesc.ACTIVATETIME));
		try {
			this.contactControl = Integer.parseInt(props
					.get(UserDesc.CONTACTCONTROL));
		} catch (NumberFormatException e) {
			this.contactControl = 1;
		}
		this.desc = props.get(UserDesc.DESC);
		try {
			this.descBan = Integer.parseInt(props.get(UserDesc.DESCBAN));
		} catch (NumberFormatException e) {
			this.descBan = 0;
		}
		this.fav = props.get(UserDesc.FAV);
		this.fixPhone = props.get(UserDesc.FIXPHONE);
		try {
			this.flagUpdate = Integer.parseInt(props.get(UserDesc.FLAGUPDATE));
		} catch (NumberFormatException e) {
			this.flagUpdate = 3;
		}
		this.homepage = props.get(UserDesc.HOMEPAGE);

		this.lastLoginTime = DateFormatter.parse(props
				.get(UserDesc.LASTLOGINTIME));
		this.lastLogoutTime = DateFormatter.parse(props
				.get(UserDesc.LASTLOGOUTTIME));
		this.mainUrl = props.get(UserDesc.MAINURL);
		this.mobilePhone = props.get(UserDesc.MOBILEPHONE);
		this.msn = props.get(UserDesc.MSN);
		String qq = props.get(UserDesc.QQ);
		if (qq == null || qq.equals("")) {
			this.qq = "0";
		} else {
			this.qq = qq;
		}
		this.registerTime = DateFormatter.parse(props
				.get(UserDesc.REGISTERTIME));
		this.template = Integer.parseInt(props.get(UserDesc.TEMPLATE));
		this.lastModifyTime = DateFormatter.parse(props
				.get(UserDesc.LAST_MODIFY_TIME));

		return this;
	}
}
