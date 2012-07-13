package mop.hi.oce.domain.user;

import java.util.Map;

import mop.hi.oce.domain.Markable;

public class UserConfigInfo extends Markable {

	public static final String ID = "ID";

	public static final String BASIC_CONFIG = "BASIC_CONFIG";

	public static final String POKE_CONFIG = "POKE_CONFIG";

	public static final String REQUEST_FRIEND_CONFIG = "REQUEST_FRIEND_CONFIG";

	public static final String PHOTO_CONFIG = "PHOTO_CONFIG";

	public static final String MESSAGE_CONFIG = "MESSAGE_CONFIG";

	public static final String EMAIL_CONFIG = "EMAIL_CONFIG";

	public static final String BROWSE_CONFIG = "BROWSE_CONFIG";

	public static final String RECV_FEED_CONFIG = "RECV_FEED_CONFIG";

	public static final String SEND_FEED_CONFIG = "SEND_FEED_CONFIG";

	public static final String WANT_SEE_CSS = "WANT_SEE_CSS";

	public static final String DISPLAY_MENU_LIST = "DISPLAY_MENU_LIST";
	public static final String MORE_MENU_LIST = "MORE_MENU_LIST";
	public static final String PROFILE_PRIVACY = "PROFILE_PRIVACY";
	public static final String STATUS_CONFIG = "STATUS_CONFIG";

	protected int userId;
	protected int basicConfig;
	protected int pokeConfig;
	protected int requestFriendConfig;
	protected int photoConfig;
	protected int messageConfig;
	protected long emailConfig;
	protected int browseConfig;
	protected String displayMenuList;
	protected String moreMenuList;
	protected String sendFeedConfig;
	protected String recvFeedConfig;
	protected int wantSeeCss;
	protected String profilePrivacy;
	protected int statusConfig;

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("userId:").append(getId()).append("\n");
		buf.append("basicConfig:").append(getBasicConfig()).append("\n");
		buf.append("pokeConfig:").append(getPokeConfig()).append("\n");
		buf.append("requestFriendConfig:").append(getRequestFriendConfig()).append("\n");
		buf.append("photoConfig:").append(getPhotoConfig()).append("\n");
		buf.append("messageConfig:").append(getMessageConfig()).append("\n");
		buf.append("emailConfig:").append(getEmailConfig()).append("\n");
		buf.append("browseConfig:").append(getBrowseConfig()).append("\n");
		buf.append("displayMenuList:").append(getDisplayMenuList()).append("\n");
		buf.append("moreMenuList:").append(getMoreMenuList()).append("\n");
		buf.append("sendFeedConfig:").append(getSendFeedConfig()).append("\n");
		buf.append("recvFeedConfig:").append(getRecvFeedConfig()).append("\n");
		buf.append("wantSeeCss:").append(getWantSeeCss()).append("\n");
		buf.append("profilePrivacy:").append(getProfilePrivacy()).append("\n");
		buf.append("statusConfig:").append(getStatusConfig()).append("\n");
		
		return buf.toString();
	}
	
	public String getProfilePrivacy() {
		return this.profilePrivacy;
	}

	public void setProfilePrivacy(String profilePrivacy) {
		this.profilePrivacy = profilePrivacy;
		mark(UserConfigInfo.PROFILE_PRIVACY, this.profilePrivacy);
	}

	public int getBasicConfig() {
		return this.basicConfig;
	}

	public void setBasicConfig(int basicConfig) {
		this.basicConfig = basicConfig;
		mark(UserConfigInfo.BASIC_CONFIG, String.valueOf(this.basicConfig));
	}

	public long getEmailConfig() {
		return this.emailConfig;
	}

	public void setEmailConfig(long emailConfig) {
		this.emailConfig = emailConfig;
		mark(UserConfigInfo.EMAIL_CONFIG, String.valueOf(this.emailConfig));
	}

	public int getId() {
		return this.userId;
	}

	public void setId(int id) {
		this.userId = id;
		mark(UserConfigInfo.ID, String.valueOf(this.userId));
	}

	public int getMessageConfig() {
		return this.messageConfig;
	}

	public void setMessageConfig(int messageConfig) {
		this.messageConfig = messageConfig;
		mark(UserConfigInfo.MESSAGE_CONFIG, String.valueOf(this.messageConfig));
	}

	public String getDisplayMenuList() {
		return this.displayMenuList;
	}

	public void setDisplayMenuList(String displayMenuList) {
		this.displayMenuList = displayMenuList;
		mark(UserConfigInfo.DISPLAY_MENU_LIST, this.displayMenuList);
	}

	public String getMoreMenuList() {
		return this.moreMenuList;
	}

	public void setMoreMenuList(String moreMenuList) {
		this.moreMenuList = moreMenuList;
		mark(UserConfigInfo.MORE_MENU_LIST, this.moreMenuList);
	}

	public int getPhotoConfig() {
		return this.photoConfig;
	}

	public void setPhotoConfig(int photoConfig) {
		this.photoConfig = photoConfig;
		mark(UserConfigInfo.PHOTO_CONFIG, String.valueOf(this.photoConfig));
	}

	public int getPokeConfig() {
		return this.pokeConfig;
	}

	public void setPokeConfig(int pokeConfig) {
		this.pokeConfig = pokeConfig;
		mark(UserConfigInfo.POKE_CONFIG, String.valueOf(this.pokeConfig));
	}

	public int getRequestFriendConfig() {
		return this.requestFriendConfig;
	}

	public void setRequestFriendConfig(int requestFriendConfig) {
		this.requestFriendConfig = requestFriendConfig;
		mark(UserConfigInfo.REQUEST_FRIEND_CONFIG, String
				.valueOf(this.requestFriendConfig));
	}

	public int getBrowseConfig() {
		return this.browseConfig;
	}

	public void setBrowseConfig(int browseConfig) {
		this.browseConfig = browseConfig;
		mark(UserConfigInfo.BROWSE_CONFIG, String.valueOf(this.browseConfig));
	}

	public String getRecvFeedConfig() {
		return this.recvFeedConfig;
	}

	public void setRecvFeedConfig(String config) {
		this.recvFeedConfig = config;
		mark(UserConfigInfo.RECV_FEED_CONFIG, this.recvFeedConfig);
	}

	public String getSendFeedConfig() {
		return this.sendFeedConfig;
	}

	public void setSendFeedConfig(String config) {
		this.sendFeedConfig = config;
		mark(UserConfigInfo.SEND_FEED_CONFIG, this.sendFeedConfig);
	}

	public int getWantSeeCss() {
		return wantSeeCss;
	}

	public void setWantSeeCss(int wantSeeCss) {
		this.wantSeeCss = wantSeeCss;
		mark(UserConfigInfo.WANT_SEE_CSS, String.valueOf(this.wantSeeCss));
	}

	public int getStatusConfig(){
		return statusConfig;
	}
	
	public void setStatusConfig(int statusconfig){
		this.statusConfig = statusconfig;
		mark(UserConfigInfo.STATUS_CONFIG,String.valueOf(this.statusConfig));
	}
	
	public UserConfigInfo parse(Map<String, String> config) {
		// this.config = config;

		try {
			userId = Integer.valueOf(config.get(UserConfigInfo.ID));
		} catch (NumberFormatException e) {
			userId = 0;
		}
		try {
			basicConfig = Integer.valueOf(config
					.get(UserConfigInfo.BASIC_CONFIG));
		} catch (NumberFormatException e) {
			basicConfig = 0;
		}
		try {
			pokeConfig = Integer
					.valueOf(config.get(UserConfigInfo.POKE_CONFIG));
		} catch (NumberFormatException e) {
			pokeConfig = 0;
		}

		try {
			requestFriendConfig = Integer.valueOf(config
					.get(UserConfigInfo.REQUEST_FRIEND_CONFIG));
		} catch (NumberFormatException e) {
			requestFriendConfig = 0;
		}
		try {
			photoConfig = Integer.valueOf(config
					.get(UserConfigInfo.PHOTO_CONFIG));
		} catch (NumberFormatException e) {
			photoConfig = 0;
		}
		try {
			messageConfig = Integer.valueOf(config
					.get(UserConfigInfo.MESSAGE_CONFIG));
		} catch (NumberFormatException e) {
			messageConfig = 0;
		}
		try {
			emailConfig = Long.valueOf(config.get(UserConfigInfo.EMAIL_CONFIG));
		} catch (NumberFormatException e) {
			emailConfig = 0;
		}
		try {
			browseConfig = Integer.valueOf(config
					.get(UserConfigInfo.BROWSE_CONFIG));
		} catch (NumberFormatException e) {
			browseConfig = 0;
		}
		displayMenuList = config.get(UserConfigInfo.DISPLAY_MENU_LIST);
		moreMenuList = config.get(UserConfigInfo.MORE_MENU_LIST);

		this.sendFeedConfig = config.get(UserConfigInfo.SEND_FEED_CONFIG);
		this.recvFeedConfig = config.get(UserConfigInfo.RECV_FEED_CONFIG);
		try {
			this.wantSeeCss = Integer.valueOf(config
					.get(UserConfigInfo.WANT_SEE_CSS));
		} catch (NumberFormatException e) {
			this.wantSeeCss = 0;
		}
		
		
		try {
			this.statusConfig = Integer.valueOf(config
					.get(UserConfigInfo.STATUS_CONFIG));
		} catch (NumberFormatException e) {
			this.statusConfig = 0;
		}
		
		this.profilePrivacy = config.get(UserConfigInfo.PROFILE_PRIVACY);

		return this;
	}
}
