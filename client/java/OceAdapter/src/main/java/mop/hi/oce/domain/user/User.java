package mop.hi.oce.domain.user;

import java.io.Serializable;
import java.util.Map;

import mop.hi.oce.domain.Markable;

/**
 * 
 * User.java
 * 
 * @author drizzt drizzt.wang@gmail.com 2006-6-24 5:04:47
 */
public class User extends Markable implements Serializable {

	public static final String ID = "ID";

	public static final String ORIGPASSWORD = "PASSWORD";

	public static final String EMAIL = "EMAIL";

	public static final String NAME = "NAME";

	public static final String FRIENDCOUNT = "FRIENDCOUNT";

	public static final String NEWMESSAGECOUNT = "NEWMESSAGECOUNT";

	public static final String POSTCOUNT = "POSTCOUNT";

	public static final String STATUS = "STATUS";

	public static final String ONLINESTATUS = "ONLINESTATUS";

	public static final String LOGINCOUNT = "LOGINCOUNT";

	public static final String HEADURL = "HEADURL";

	public static final String MESSAGEINCOUNT = "MESSAGE_IN_COUNT";

	public static final String GOSSIPCOUNT = "LEAVEWORD_COUNT";

	public static final String MESSAGEOUTCOUNT = "MESSAGE_OUT_COUNT";

	public static final String SELECTED = "SELECTED";

	public static final String VERIFYCODE = "VERIFY_CODE";

	public static final String AUTH = "AUTH";

	public static final String NEWEMAIL = "NEW_EMAIL";

	public static final String EMAILVERIFY = "EMAIL_VERIFY";

	public static final String TRIBECOUNT = "TRIBE_COUNT";

	public static final String GENDER = "GENDER";

	public static final String UNIV = "UNIVERSITY";

	public static final String EXP = "EXP";

	public static final String LASTIP = "LAST_IP";

	public static final String LARGEURL = "LARGEURL";

	// public static final String = "BLOG_CONTROL";
	public static final String NOTIFY = "NOTIFY";

	// public static final String = "LEAVE_WORD_FLAG";
	public static final String ENTRYCOMMENTFLAG = "ENTRY_COMMENT_FLAG";

	public static final String STAR = "STAR";

	public static final String PHOTOCONTROL = "PHOTOCONTROL";

	public static final String GUESTREQUESTCOUNT = "GUESTREQUESTCOUNT";

	public static final String TRIBEREQUESTCOUNT = "TRIBE_REQUESTCOUNT";

	public static final String COMMONRIGHTS = "COMMON_RIGHTS";

	public static final String PASSWORD_MD5 = "PASSWORD_MD5";

	public static final String ENTRY = "ENTRY";

	public static final String VERIFYREQUESTCOUNT = "VERIFYREQUESTCOUNT";

	public static final String SPACESIZE = "SPACESIZE";

	public static final String COOKIEID = "COOKIE_ID";

	public static final String OPEN_CONFIG = "OPEN_CONFIG";

	public static final String CHAT_CONFIG = "CHAT_CONFIG";

	public static final String SIGNATURE = "SIGNATURE";

	public static final String QMAIL = "QMAIL";

	public static final String SOURCE = "SOURCE";

	public static final String CLASSCREATECOUNT = "CLASS_CREATE_COUNT";

	public static final String CLASSINVITEDCOUNT = "CLASS_INVITED_COUNT";

	public static final String CLASSJOINCOUNT = "CLASS_JOIN_COUNT";

	public static final String TEMPLATE = "TEMPLATE";

	public static final String STAGE = "STAGE";

	// public static final String CUR_NETWORK_ID = "CUR_NETWORK_ID";
	//	
	// public static final String CUR_NETWORK_NAME = "CUR_NETWORK_NAME";

	public static final String IS_OLD_USER = "IS_OLD_USER";

	protected int id;

	protected String origPassword;

	protected String email;

	protected String name;

	protected int friendCount;

	protected int newMessageCount;

	protected int postCount;

	protected int status;// = User.statusOffline;

	// protected int onlineStatus;

	protected int loginCount;

	protected String headUrl;// = Globals.userHeadUrl;

	protected int messageInCount;

	protected int gossipCount;

	protected int messageOutCount;

	protected int selected;// = User.usrNotSelected;

	protected String verifyCode;

	protected int auth;

	protected String newEmail;

	protected String emailVerify;

	protected int tribeCount;

	protected String gender;// = User.genderUnknown;

	protected int univ;

	protected int exp;

	protected String lastIp;

	protected String largeUrl;// = Globals.userLargeUrl;

	protected int notify;// = DailyEmail.notifyYes;

	protected int gossipFlag;

	protected int entryCommentFlag;

	protected int star;

	protected int photoControl;// = Relation1.uuGuestRequest;

	protected int guestRequestCount;

	protected int tribeRequestCount;

	protected String password;

	protected int commonRights;// = User.defaultUserRights;

	protected int entry;

	protected String cookieId;

	protected int verifyRequestCount;

	protected int spaceSize;

	protected int open_config;

	protected int chat_config;

	protected String signature;

	protected String qmail;

	protected int source;

	protected int classCreateCount;

	protected int classInvitedCount;

	protected int classJoinCount;

	protected int template;

	protected int stage;

	protected int isOldUser;

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("User:\n");
		str.append("ID = ").append(this.getId()).append("\n");
		str.append("ORIGPASSWORD = ").append(getOrigPassword()).append("\n");
		str.append("EMAIL = ").append(getEmail()).append("\n");
		str.append("NAME = ").append(getName()).append("\n");
		str.append("FRIENDCOUNT = ").append(getFriendCount()).append("\n");
		str.append("NEWMESSAGECOUNT = ").append(getNewMessageCount()).append(
				"\n");
		str.append("POSTCOUNT = ").append(getPostCount()).append("\n");
		str.append("STATUS = ").append(getStatus()).append("\n");
		// str.append("ONLINE_STATUS =
		// ").append(getOnlineStatus()).append("\n");
		str.append("LOGINCOUNT = ").append(getLoginCount()).append("\n");
		str.append("HEADURL = ").append(getHeadUrl()).append("\n");
		str.append("MESSAGEINCOUNT = ").append(getMessageInCount())
				.append("\n");
		str.append("GOSSIPCOUNT = ").append(getGossipCount()).append("\n");
		str.append("MESSAGEOUTCOUNT = ").append(getMessageOutCount()).append(
				"\n");
		str.append("SELECTED = ").append(getSelected()).append("\n");
		str.append("VERIFYCODE = ").append(getVerifyCode()).append("\n");
		str.append("AUTH = ").append(getAuth()).append("\n");
		str.append("NEWEMAIL = ").append(getNewEmail()).append("\n");
		str.append("EMAILVERIFY = ").append(getEmailVerify()).append("\n");
		str.append("TRIBECOUNT = ").append(getTribeCount()).append("\n");
		str.append("GENDER = ").append(getGender()).append("\n");
		str.append("UNIV = ").append(getUniv()).append("\n");
		str.append("EXP = ").append(getExp()).append("\n");
		str.append("LASTIP = ").append(getLastIp()).append("\n");
		str.append("LARGEURL = ").append(getLargeUrl()).append("\n");
		str.append("NOTIFY = ").append(getNotify()).append("\n");
		str.append("ENTRYCOMMENTFLAG = ").append(getEntryCommentFlag()).append(
				"\n");
		str.append("STAR = ").append(getStar()).append("\n");
		str.append("PHOTOCONTROL = ").append(getPhotoControl()).append("\n");
		str.append("GUESTREQUESTCOUNT = ").append(getGuestRequestCount())
				.append("\n");
		str.append("TRIBEREQUESTCOUNT = ").append(getTribeRequestCount())
				.append("\n");
		str.append("COMMONRIGHTS = ").append(getCommonRights()).append("\n");
		str.append("PASSWORD = ").append(getPassword()).append("\n");
		str.append("ENTRY = ").append(getEntry()).append("\n");
		str.append("VERIFYREQUESTCOUNT = ").append(getVerifyRequestCount())
				.append("\n");
		str.append("SPACESIZE = ").append(getSpaceSize()).append("\n");
		str.append("OPEN_CONFIG = ").append(getOpenConfig()).append("\n");
		str.append("CHAT_CONFIG = ").append(getChatConfig()).append("\n");
		str.append("SIGNATURE = ").append(getSignature()).append("\n");
		str.append("QMAIL = ").append(getQmail()).append("\n");
		str.append("SOURCE = ").append(String.valueOf(getSource()))
				.append("\n");
		str.append("COOKIEID = ").append(getCookieId()).append("\n");
		str.append("CLASSCREATECOUNT = ").append(getClassCreateCount()).append(
				"\n");
		str.append("CLASSINVITEDCOUNT = ").append(getClassInvitedCount())
				.append("\n");
		str.append("CLASSJOINCOUNT = ").append(getClassJoinCount())
				.append("\n");
		str.append("TEMPLATE = ").append(getTemplate()).append("\n");
		str.append("STAGE = ").append(getStage()).append("\n");
		str.append("IS_OLD_USER = ").append(getIsOldUser()).append("\n");
		return (str.toString());
	}

	public int getGossipCount() {
		return gossipCount;
	}

	public void setGossipCount(int leaveWordCount) {
		this.gossipCount = leaveWordCount;
		mark(this.GOSSIPCOUNT, String.valueOf(this.gossipCount));
	}

	public int getSelected() {
		return selected;
	}

	public void setSelected(int selected) {
		this.selected = selected;
		mark(this.SELECTED, String.valueOf(this.selected));
	}

	public int getSpaceSize() {
		return spaceSize;
	}

	public void setSpaceSize(int spaceSize) {
		this.spaceSize = spaceSize;
		mark(this.SPACESIZE, String.valueOf(this.spaceSize));
	}

	public int getUniv() {
		return univ;
	}

	public void setUniv(int univ) {
		this.univ = univ;
		mark(this.UNIV, String.valueOf(this.univ));
	}

	public int getAuth() {
		return auth;
	}

	public void setAuth(int auth) {
		this.auth = auth;
		mark(User.AUTH, String.valueOf(this.auth));
	}

	public int getCommonRights() {
		return commonRights;
	}

	public void setCommonRights(int commonRights) {
		this.commonRights = commonRights;
		mark(this.COMMONRIGHTS, String.valueOf(this.commonRights));

	}

	public String getCookieId() {
		return cookieId;
	}

	public void setCookieId(String cookieId) {
		this.cookieId = cookieId;
		mark(this.COOKIEID, this.cookieId);

	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
		mark(this.EMAIL, this.email);
	}

	public String getEmailVerify() {
		return emailVerify;
	}

	public void setEmailVerify(String emailVerify) {
		this.emailVerify = emailVerify;
		mark(this.EMAILVERIFY, this.emailVerify);
	}

	public int getEntry() {
		return entry;
	}

	public void setEntry(int entry) {
		this.entry = entry;
		mark(this.ENTRY, String.valueOf(this.entry));
	}

	public int getEntryCommentFlag() {
		return entryCommentFlag;
	}

	public void setEntryCommentFlag(int entryCommentFlag) {
		this.entryCommentFlag = entryCommentFlag;
		mark(this.ENTRYCOMMENTFLAG,
				String.valueOf(this.entryCommentFlag));
	}

	public int getExp() {
		return exp;
	}

	public void setExp(int exp) {
		this.exp = exp;
		mark(this.EXP, String.valueOf(this.exp));
	}

	public String getGender() {
		return gender;
	}

	public void setGender(String gender) {
		this.gender = gender;
		mark(this.GENDER, this.gender);
	}

	public int getGuestRequestCount() {
		return guestRequestCount;
	}

	public void setGuestRequestCount(int guestRequestCount) {
		this.guestRequestCount = guestRequestCount;
		mark(this.GUESTREQUESTCOUNT,
				String.valueOf(this.guestRequestCount));
	}

	public String getLargeUrl() {
		return largeUrl;
	}

	public void setLargeUrl(String largeUrl) {
		this.largeUrl = largeUrl;
		mark(this.LARGEURL, this.largeUrl);
	}

	public String getLastIp() {
		return lastIp;
	}

	public void setLastIp(String lastIp) {
		this.lastIp = lastIp;
		mark(this.LASTIP, this.lastIp);
	}

	@Deprecated
	public int getGossipFlag() {
		return gossipFlag;
	}

	@Deprecated
	public void setGossipFlag(int leaveWordFlag) {
		this.gossipFlag = leaveWordFlag;

	}

	public int getMessageInCount() {
		return messageInCount;
	}

	public void setMessageInCount(int messageInCount) {
		this.messageInCount = messageInCount;
		mark(this.MESSAGEINCOUNT,
				String.valueOf(this.messageInCount));
	}

	public int getMessageOutCount() {
		return messageOutCount;
	}

	public void setMessageOutCount(int messageOutCount) {
		this.messageOutCount = messageOutCount;
		mark(this.MESSAGEOUTCOUNT,
				String.valueOf(this.messageOutCount));
	}

	public String getNewEmail() {
		return newEmail;
	}

	public void setNewEmail(String newEmail) {
		this.newEmail = newEmail;
		mark(User.NEWEMAIL, this.newEmail);
	}

	public int getNotify() {
		return notify;
	}

	public void setNotify(int notify) {
		this.notify = notify;
		mark(User.NOTIFY, String.valueOf(this.notify));
	}

	public String getOrigPassword() {
		return origPassword;
	}

	public void setOrigPassword(String origPassword) {
		this.origPassword = origPassword;
		mark(User.ORIGPASSWORD, this.origPassword);
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
		mark(User.PASSWORD_MD5, this.password);
	}

	public int getPhotoControl() {
		return photoControl;
	}

	public void setPhotoControl(int photoControl) {
		this.photoControl = photoControl;
		mark(User.PHOTOCONTROL, String.valueOf(this.photoControl));
	}

	public int getStar() {
		return star;
	}

	public void setStar(int star) {
		this.star = star;
		mark(User.STAR, String.valueOf(this.star));
	}

	public int getTribeCount() {
		return tribeCount;
	}

	public void setTribeCount(int tribeCount) {
		this.tribeCount = tribeCount;
		mark(User.TRIBECOUNT, String.valueOf(this.tribeCount));
	}

	public int getTribeRequestCount() {
		return tribeRequestCount;
	}

	public void setTribeRequestCount(int tribeRequestCount) {
		this.tribeRequestCount = tribeRequestCount;
		mark(User.TRIBEREQUESTCOUNT,
				String.valueOf(this.tribeRequestCount));
	}

	public String getVerifyCode() {
		return verifyCode;
	}

	public void setVerifyCode(String verifyCode) {
		this.verifyCode = verifyCode;
		mark(User.VERIFYCODE, this.verifyCode);
	}

	public int getVerifyRequestCount() {
		return verifyRequestCount;
	}

	public void setVerifyRequestCount(int verifyRequestCount) {
		this.verifyRequestCount = verifyRequestCount;
		mark(VERIFYREQUESTCOUNT,
				String.valueOf(this.verifyRequestCount));
	}

	public int getFriendCount() {
		return friendCount;
	}

	public void setFriendCount(int friendCount) {
		this.friendCount = friendCount;
		mark(FRIENDCOUNT, String.valueOf(this.friendCount));
	}

	public String getHeadUrl() {
		return headUrl;
	}

	public void setHeadUrl(String headUrl) {
		this.headUrl = headUrl;
		mark(HEADURL, this.headUrl);
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getLoginCount() {
		return loginCount;
	}

	public void setLoginCount(int loginCount) {
		this.loginCount = loginCount;
		mark(LOGINCOUNT, String.valueOf(this.loginCount));
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
		mark(NAME, this.name);
	}

	public int getNewMessageCount() {
		return newMessageCount;
	}

	public void setNewMessageCount(int newMessageCount) {
		this.newMessageCount = newMessageCount;
		mark(NEWMESSAGECOUNT, String.valueOf(this.newMessageCount));
	}

	public int getPostCount() {
		return postCount;
	}

	public void setPostCount(int postCount) {
		this.postCount = postCount;
		mark(POSTCOUNT, String.valueOf(this.postCount));
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		this.status = status;
		mark(STATUS, String.valueOf(this.status));
	}

	public int getOpenConfig() {
		return this.open_config;
	}

	public void setOpenConfig(int openConfig) {
		this.open_config = openConfig;
		mark(OPEN_CONFIG, String.valueOf(this.open_config));
	}

	public int getChatConfig() {
		return this.chat_config;
	}

	public void setChatConfig(int chatConfig) {
		this.chat_config = chatConfig;
		mark(CHAT_CONFIG, String.valueOf(this.chat_config));
	}

	public String getSignature() {
		return this.signature;
	}

	public void setSignature(String signature) {
		this.signature = signature;
		mark(SIGNATURE, this.signature);
	}

	public String getQmail() {
		return this.qmail;
	}

	public void setQmail(String qmail) {
		this.qmail = qmail;
		mark(QMAIL, this.qmail);
	}

	public int getSource() {
		return this.source;
	}

	public void setSource(int source) {
		this.source = source;
		mark(SOURCE, String.valueOf(this.source));
	}

	public User parse(Map<String, String> props) {
		
		// do not catch exception when parse id
		this.id = Integer.parseInt(props.get(User.ID));
		
		try {
			this.auth = Integer.parseInt(props.get(User.AUTH));
		} catch (NumberFormatException e) {

		}
		try {
			this.commonRights = Integer.parseInt(props.get(User.COMMONRIGHTS));
		} catch (NumberFormatException e) {
			this.commonRights = 63;
		}
		this.cookieId = props.get(User.COOKIEID);
		this.email = props.get(User.EMAIL);
		this.emailVerify = props.get(User.EMAILVERIFY);
		try {
			this.entry = Integer.parseInt(props.get(User.ENTRY));
		} catch (NumberFormatException e) {

		}
		try {
			this.entryCommentFlag = Integer.parseInt(props
					.get(User.ENTRYCOMMENTFLAG));
		} catch (NumberFormatException e) {

		}
		try {
			this.exp = Integer.parseInt(props.get(User.EXP));
		} catch (NumberFormatException e) {

		}
		this.friendCount = Integer.parseInt(props.get(User.FRIENDCOUNT));
		this.gender = props.get(User.GENDER);
		try {
			this.gossipCount = Integer.parseInt(props.get(User.GOSSIPCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.guestRequestCount = Integer.parseInt(props
					.get(User.GUESTREQUESTCOUNT));
		} catch (NumberFormatException e) {

		}
		this.headUrl = props.get(User.HEADURL);
		this.largeUrl = props.get(User.LARGEURL);
		this.lastIp = props.get(User.LASTIP);
		try {
			this.loginCount = Integer.parseInt(props.get(User.LOGINCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.messageInCount = Integer.parseInt(props
					.get(User.MESSAGEINCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.messageOutCount = Integer.parseInt(props
					.get(User.MESSAGEOUTCOUNT));
		} catch (NumberFormatException e) {

		}
		this.name = props.get(User.NAME);
		this.newEmail = props.get(User.NEWEMAIL);
		try {
			this.newMessageCount = Integer.parseInt(props
					.get(User.NEWMESSAGECOUNT));
		} catch (NumberFormatException e) {

		}
		this.notify = Integer.parseInt(props.get(User.NOTIFY));
		this.origPassword = props.get(User.ORIGPASSWORD);
		this.password = props.get(User.PASSWORD_MD5);
		try {
			this.photoControl = Integer.parseInt(props.get(User.PHOTOCONTROL));
		} catch (NumberFormatException e) {

		}
		try {
			this.postCount = Integer.parseInt(props.get(User.POSTCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.selected = Integer.parseInt(props.get(User.SELECTED));
		} catch (NumberFormatException e) {

		}
		try {
			this.spaceSize = Integer.parseInt(props.get(User.SPACESIZE));
		} catch (NumberFormatException e) {

		}
		try {
			this.star = Integer.parseInt(props.get(User.STAR));
		} catch (NumberFormatException e) {
			this.star = 1;
		}
		try {
			this.status = Integer.parseInt(props.get(User.STATUS));
		} catch (NumberFormatException e) {
			this.status = 2;
		}
		try {
			this.tribeCount = Integer.parseInt(props.get(User.TRIBECOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.tribeRequestCount = Integer.parseInt(props
					.get(User.TRIBEREQUESTCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.univ = Integer.parseInt(props.get(User.UNIV));
		} catch (NumberFormatException e) {

		}
		this.verifyCode = props.get(User.VERIFYCODE);
		try {
			this.verifyRequestCount = Integer.parseInt(props
					.get(User.VERIFYREQUESTCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.stage = Integer.parseInt(props.get(User.STAGE));
		} catch (NumberFormatException e) {

		}

		try {
			this.isOldUser = Integer.parseInt(props.get(User.IS_OLD_USER));
		} catch (NumberFormatException e) {

		}
		this.signature = props.get(User.SIGNATURE);
		this.qmail = props.get(User.QMAIL);
		try {
			this.source = Integer.parseInt(props.get(User.SOURCE));
		} catch (NumberFormatException e) {

		}
		try {
			this.classCreateCount = Integer.parseInt(props
					.get(User.CLASSCREATECOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.classInvitedCount = Integer.parseInt(props
					.get(User.CLASSINVITEDCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.classJoinCount = Integer.parseInt(props
					.get(User.CLASSJOINCOUNT));
		} catch (NumberFormatException e) {

		}
		try {
			this.template = Integer.parseInt(props.get(User.TEMPLATE));
		} catch (NumberFormatException e) {

		}
		
		return this;
	}
	
	public int getClassCreateCount() {
		return classCreateCount;
	}

	public void setClassCreateCount(int classCreateCount) {
		this.mark(CLASSCREATECOUNT,
				String.valueOf(classCreateCount));
		this.classCreateCount = classCreateCount;
	}

	public int getClassInvitedCount() {
		return classInvitedCount;
	}

	public void setClassInvitedCount(int classInvitedCount) {
		this.mark(CLASSINVITEDCOUNT,
				String.valueOf(classInvitedCount));
		this.classInvitedCount = classInvitedCount;
	}

	public int getClassJoinCount() {
		return classJoinCount;
	}

	public void setClassJoinCount(int classJoinCount) {
		this.mark(CLASSJOINCOUNT, String.valueOf(classJoinCount));
		this.classJoinCount = classJoinCount;
	}

	public int getTemplate() {
		return template;
	}

	public void setTemplate(int template) {
		this.mark(TEMPLATE, String.valueOf(template));
		this.template = template;
	}

	public int getStage() {
		return stage;
	}

	public void setStage(int stage) {
		this.mark(STAGE, String.valueOf(stage));
		this.stage = stage;
	}

	public int getIsOldUser() {
		return isOldUser;
	}

	public void setIsOldUser(int isOldUser) {
		this.isOldUser = isOldUser;
		this.mark(IS_OLD_USER, String.valueOf(isOldUser));
	}

}
