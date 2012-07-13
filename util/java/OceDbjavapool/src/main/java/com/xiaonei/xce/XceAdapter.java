package com.xiaonei.xce;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.UUID;
import com.xiaonei.xce.log.Oce;
import javax.sql.DataSource;

import Ice.InitializationData;

import com.xiaonei.xce.pool.ConnectionPoolManagerIF;
import com.xiaonei.xce.pool.old.ConnectionPoolManager;

public final class XceAdapter extends Thread {
    org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(XceAdapter.class);
    private static XceAdapter _instance = new XceAdapter();

    public static XceAdapter getInstance() {
	return _instance;
    }

    private final ConnectionPoolManagerIF _manager;
    // private final Ice.Communicator _ic;
    private final UUID myUUID = UUID.randomUUID();
    private static final String OBJECT_ADAPTER_PREFIX = "XceDbObserver_";
    private final String myName = OBJECT_ADAPTER_PREFIX + myUUID.toString();

    public XceAdapter() {
	String which = System.getProperty("WHICH_DB_JAVA_POOL", "old");
	if (which.equals("old")) {
	    Ice.Communicator _ic = createIceCommunicator();
	    Ice.ObjectAdapter objectAdapter = _ic.createObjectAdapter(myName);
	    objectAdapter.activate();
	    _manager = new ConnectionPoolManager(objectAdapter);
	} else {
	    Oce
		    .getLogger()
		    .error(
			    "Wrong WHICH_DB_JAVA_POOL property, Only <old|new|dist> supported.");
	    _manager = null;
	}
	start();
    }

    public static final String DB_COMMON = "common";
    public static final String DB_USER_COMMON = "user_common";
    public static final String DB_USER_CONFIG = "user_config";
    public static final String DB_USER_VIEW_COUNT = "user_view_count";
    public static final String DB_USER_DESC = "user_desc";
    public static final String DB_IDSEQUENCE = "idseq";
    public static final String DB_APP = "app";
    public static final String DB_COUNT = "count";
    public static final String DB_RELATION = "relation";
    public static final String DB_RELATION_INFO = "relation_info";
    public static final String DB_RELATION_BLOCK = "relation_block";
    public static final String DB_FRIEND_DIST = "friend_dist";
    public static final String DB_ACCOUNT = "account";
    public static final String DB_CLASS = "class";
    public static final String DB_CLASS_INFO = "class_info";
    public static final String DB_GROUP = "group";
    public static final String DB_GROUP_INFO = "group_info";
    public static final String DB_MEMBER_GROUP = "member_group";
    public static final String DB_GOSSIP = "gossip";
    public static final String DB_GOSSIP_INFO = "gossip_info";
    public static final String DB_BLOG = "blog";
    public static final String DB_BLOG_INFO = "blog_info";
    public static final String DB_BLOG_STAT = "blog_stat";
    public static final String DB_MESSAGE = "message";
    public static final String DB_MESSAGE_INFO = "message_info";
    public static final String DB_SELF_MESSAGE = "self_message";
    public static final String DB_STAT = "stat";
    public static final String DB_ALBUM = "album";
    public static final String DB_ALBUM_INFO = "album_info";
    public static final String DB_MARKET = "market";
    public static final String DB_GIFT = "gift";
    public static final String DB_LOGGING = "logging";
    public static final String DB_SEARCH = "search";
    public static final String DB_BROWSE = "browse";
    public static final String DB_GUIDE = "guide";
    public static final String DB_IMMSG = "immsg";
    public static final String DB_ASK = "ask";
    public static final String DB_SEARCH2_USERCOMMON = "search2_userbasic";
    public static final String DB_SEARCH2_USERCONFIG = "search2_userconfig";
    public static final String DB_SEARCH2_USER = "search2_user";
    public static final String DB_SEARCH2_USERBASIC = "search2_userbasic";
    public static final String DB_SEARCH2_TRIBE = "search2_tribe";
    public static final String DB_SEARCH2_MODIFY = "search2_modify";
    public static final String DB_SEARCH2_USERDESC = "search2_userdesc";
    public static final String DB_SEARCH2_USERTIME = "search2_usertime";
    public static final String DB_SEARCH2_RELATION = "search2_relation";
    public static final String DB_AUDIT_USER = "audit_user";
    public static final String DB_AUDIT_CONTENT = "audit_content";
    public static final String DB_REQUEST_FRIEND_MEMO = "request_friend_memo";
    public static final String DB_USER_MSN = "user_msn";
    public static final String DB_SHARE = "share";
    public static final String DB_GOING_INFO = "going_info";
    public static final String DB_MOVIE = "movie";
    public static final String DB_VIPMEMBER = "vipmember";
    public static final String DB_MAIL_MMS = "mail_mms";
    public static final String DB_POLICE = "police";
    public static final String DB_EMAIL = "email";
    public static final String DB_MESSAGE_SESSION = "message_session";
    public static final String DB_ACTIVITY = "activity";
    public static final String DB_EVENT = "event";
    public static final String DB_GAOKAO = "gaokao";
    public static final String DB_DOING = "doing";
    public static final String DB_STORAGE_NODE_LANGFANG = "storage_node_langfang";
    public static final String DB_POP_USER = "pop_user";
    public static final String DB_NATIVE_APP = "native_app";
    public static final String DB_CARPARK = "carpark";
    public static final String DB_CARPARK_KX = "carpark_kx";
    public static final String DB_VAMPIRE = "vampire";
    public static final String DB_APP_USER = "app_user";
    public static final String DB_USER_APP = "user_app";
    public static final String DB_GROUP_MEMBER = "group_member";
    public static final String DB_MESSAGE_NOTIFY = "message_notify";
    public static final String DB_APP_LOG = "app_log";
    public static final String DB_KNOWABC_INFO = "knowabc_info";
    public static final String DB_KNOWABC_SUB = "knowabc_sub";
    public static final String DB_KNOWABC_NEWS = "knowabc_news";
    public static final String DB_TYCOON = "tycoon";
    public static final String DB_TYCOON_KX = "tycoon_kx";
    public static final String DB_CARPARKGAME = "carparkgame";
    public static final String DB_NOTIFY_CONFIG = "notify_config";
    public static final String DB_REQUEST_CONFIG = "request_config";
    public static final String DB_MOUSE = "mouse";
    public static final String DB_USER_SCORE = "user_score";
    public static final String DB_USER_SCORE_INFO = "user_score_info";
    public static final String DB_DOG = "dog";
    public static final String DB_DOG_KX = "dog_kx";
    public static final String DB_DOG_INFO_KX = "dog_info_kx";
    public static final String DB_MINIGAME = "minigame";
    public static final String DB_MINIGAME_KX = "minigame_kx";
    public static final String DB_MINIGAME_INFO = "minigame_info";
    public static final String DB_MINIGAME_INFO_KX = "minigame_info_kx";
    public static final String DB_CARPARK_LOG = "carpark_log";
    public static final String DB_CARPARK_LOG_KX = "carpark_log_kx";
    public static final String DB_BITE = "bite";
    public static final String DB_MOBWAR = "mobwar";
    public static final String DB_GODFATHER = "godfather";
    public static final String DB_MOBWAR_NEWS = "mobwar_news";
    public static final String DB_MOBWAR_FIGHT = "mobwar_fight";
    public static final String DB_MOBWAR_REL = "mobwar_rel";
    public static final String DB_MOBWAR_LIST = "mobwar_list";
    public static final String DB_FRIENDINPUT = "friendinput_relation";
    public static final String DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";
    public static final String DB_ADDRESS_BOOK = "address_book";
    public static final String DB_REQUEST_CENTER = "request_center";
    public static final String DB_MINICITY = "minicity";
    public static final String DB_GREENPATCH = "greenpatch";
    public static final String DB_BIRTHDAILY = "birthdaily";
    public static final String DB_FLUFF = "fluff";
    public static final String DB_ACTIVATION_LETTER = "activation_letter";
    public static final String DB_OWNED = "owned";
    public static final String DB_STORAGE_NODE_TIANJIN = "storage_node_tianjin";
    public static final String DB_USER_DESC_2008 = "user_desc_2008";
    public static final String DB_MOBWAR_STAT = "mobwar_stat";
    public static final String DB_REQUEST_CENTER_APP = "request_center_app";
    public static final String DB_REQUEST_CENTER_SYSTEM = "request_center_system";
    public static final String DB_FASHION_WAR = "fashion_war";
    public static final String DB_GAME = "game";
    public static final String DB_KICKASS = "kickass";
    public static final String DB_KICKASS_KX = "kickass_kx";
    // public static final String DB_LATEST_LOGIN_FRIENDS =
    // "latest_login_friends";
    public static final String DB_GUEST_BOOK = "guest_book";
    public static final String DB_GODFATHER_MAIN = "godfather_main";
    public static final String DB_GODFATHER_LIST = "godfather_list";
    public static final String DB_GODFATHER_INFO = "godfather_info";
    public static final String DB_MOBILE_STAT = "mobile_stat";
    public static final String DB_KNOWABC_VOTING_LOG = "knowabc_voting_log";
    public static final String DB_BIZ_DOING_INFO = "biz_doing_info";
    public static final String DB_BIZ_FRIEND_DOING = "biz_friend_doing";
    public static final String DB_EMAIL_STAT = "email_stat";
    public static final String DB_INVITE_LETTER = "invite_letter";
    public static final String DB_OP_LETTER = "op_letter";
    public static final String DB_IMPRESSION = "impression";
    public static final String DB_APP_CONNECT = "app_connect";
    public static final String DB_APP_CONNECT_EMAIL = "app_connect_email";
    public static final String DB_UPLOAD = "upload";
    public static final String DB_REG_INVITE = "reg_invite";
    public static final String DB_WAP_PAY = "wap_pay";
    public static final String DB_USER_TIME = "user_time";
    public static final String DB_USER_LOGINCOUNT = "user_logincount";
    public static final String DB_FLYER = "flyer";
    public static final String DB_USER_NETWORK = "user_network";
    public static final String DB_WEBGAME_INVATE = "webgame_invite";
    public static final String DB_HONESTY = "honesty";
    public static final String DB_PAGE = "page";
    public static final String DB_FOLLOW = "follow";
    public static final String DB_MOBILE_GAME = "mobile_game";
    public static final String DB_COOK = "cook";
    public static final String DB_COOK_USER = "cook_user";
    public static final String DB_COOK_MAIL = "cook_mail";
    public static final String DB_WEBGAME_INVITE = "webgame_invite";
    public static final String DB_VOYAGE = "voyage";
    public static final String DB_KNOWABC_INVESTIGATION = "knowabc_investigation";
    public static final String DB_BULLETIN_USER_APP_LOGIN = "bulletin_user_app_login";
    public static final String DB_NEWBIE_PROFILE = "newbie_profile";
    public static final String DB_ALBUM_FRIEND = "album_friend";
    public static final String DB_SEARCH2_PAGE = "search2_page";
    public static final String DB_SEARCH2_APP = "search2_app";
    public static final String DB_PAGE_FANS = "page_fans";
    public static final String DB_FANS_PAGES = "fans_pages";
    public static final String DB_GIFT_RECORD_RECEIVER = "gift_record_receiver";
    public static final String DB_GIFT_RECORD_SENDER = "gift_record_sender";
    public static final String DB_API_PAY = "api_pay";
    public static final String DB_SOCIAL_GAME = "social_game";
    public static final String DB_DOING_HASH_TEMP = "doing_hash_temp";
    public static final String DB_MIKE_WEBSITE = "mike_website";
    public static final String DB_GIFT_NEW = "gift_new";
    public static final String DB_WAP_REGISTER = "wap_register";
    public static final String DB_USER_PRELOAD = "user_preload";
    public static final String DB_USER_BASIC_BIRTHDAY = "user_basic_birthday";
    public static final String DB_APP_USER_MENU = "app_user_menu";
    public static final String DB_ILIKEIT = "ilikeit";
    public static final String DB_KNOWABC_SEND_INV = "knowabc_send_inv";
    public static final String DB_WAP_APP = "wap_app";
    public static final String DB_GAME_HALL = "game_hall";
    public static final String DB_HOT_FRIEND = "hot_friend";
    public static final String DB_QA_COMMON = "qa_common";
    public static final String DB_ADMIN_USER_LOGIN = "admin_user_login";
    public static final String DB_MOBILE_CLIENT = "mobile_client";
    public static final String DB_SAFE_CENTER = "safe_center";
    public static final String DB_MOBILE_GAME_PLATFORM = "mobile_game_platform";
    public static final String DB_SHARE_INFO = "share_info";
    public static final String DB_GUIDE_BUDDYAPPLY = "guide_buddyapply";
    public static final String DB_APP_COMMENTS = "app_comments";
    public static final String DB_APP_FANS = "app_fans";
    public static final String DB_USER_PASSPORT_EXT = "user_passport_ext";
    public static final String DB_USER_MAIL = "user_mail";
    public static final String DB_SPAM_DELETE_NETBAR = "spam_delete_netbar";
    public static final String DB_KX_STAT = "kx_stat";
    public static final String DB_KX_REG_INVITE = "kx_reg_invite";
    public static final String DB_USER_REGISTER_IP = "user_register_ip";
    public static final String DB_FLUFF_KX = "fluff_kx";
    public static final String DB_FASHION_WAR_KX = "fashion_war_kx";
    public static final String DB_MINICITY_KX = "minicity_kx";
    public static final String DB_BIZ_CG_KX = "biz_cg_kx";
    public static final String DB_IMPRESSION_KX = "impression_kx";
    public static final String DB_WEBGAME_INVITE_KX = "webgame_invite_kx";
    public static final String DB_UGC_DELETE_RECORD = "ugc_delete_record";
    public static final String DB_PAGE_STAT = "page_stat";
    public static final String DB_USER_FOND = "user_fond";
    public static final String DB_REG_MLINK_VISIT_LOG = "reg_mlink_visit_log";
    public static final String DB_REG_INVITE_ACT = "reg_invite_act";
    public static final String DB_FRIEND_REQUEST = "friend_request";
    public static final String DB_FRIEND_RANK = "friend_rank";
    public static final String DB_SEARCH_USER_PASSPORT = "search_user_passport";
    public static final String DB_FEED_EDM = "feed_edm";
    public static final String DB_ICODE = "icode";
    public static final String DB_GOSSIP_NEW = "gossip_new";
    public static final String DB_GOSSIP_INFO_NEW = "gossip_info_new";
    public static final String DB_PAGE_VIDEO_COMMENT = "page_video_comment";
    public static final String DB_DIPROSOPUS = "diprosopus";
    public static final String DB_RROP_LETTER = "rrop_letter";
    public static final String DB_USER_TIME_EMAIL = "user_time_email";
    public static final String DB_APP_BBS = "app_bbs";
    public static final String DB_APP_WORKFLOW = "app_workflow";
    public static final String DB_SEARCH2_USERFOND = "search2_userfond";
    public static final String DB_USER_STAGE_EMAIL = "user_stage_email";
    public static final String DB_USER_PAGE_STAT = "page_stat";
    public static final String DB_PAGE_SCORE = "page_score";
    public static final String DB_USER_QUEST = "user_quest";
    public static final String DB_LOGGING_TJ = "logging_tj";
    public static final String DB_SHARE_CONTENT = "share_content";
    public static final String DB_IDSEQ_UGC = "idseq_ugc";
    public static final String DB_USER_COMMON_TJ = "user_common_tj";
    public static final String DB_INVITE_URL_PARAS = "invite_url_paras";
    public static final String DB_KX_INVITE_URL_PARAS = "kx_invite_url_paras";
    public static final String DB_GAME_STAT = "game_stat";
    public static final String DB_SHARE_INFO_NEW = "share_info_new";
    public static final String DB_SHARE_NEW = "share_new";
    public static final String DB_HIGH_SCHOOL_PAGE = "high_school_page";
    public static final String DB_MOBILE_FARM = "mobile_farm";
    public static final String DB_USER_SCORE_LOG = "user_score_log";
    public static final String DB_USER_SCORE_TOTAL = "user_score_total";
    public static final String DB_USER_SCORE_TOTAL_INFO = "user_score_total_info";
    public static final String DB_PAGE_BIZ = "page_biz";
    public static final String DB_YUREN_ACTIVE = "yuren_active";
    public static final String DB_YUREN_ATTACK = "yuren_attack";
    public static final String DB_YUREN_ATTACK_INFO = "yuren_attack_info";
    public static final String DB_YUREN_USER_RELATION_INFO = "yuren_user_relation_info";
    public static final String DB_FRIENDFINDER_ACTIVEUSER = "friendfinder_activeuser";
    public static final String DB_USER_SCORE_AWARD = "user_score_award";
    public static final String DB_SHARE_DEL_NEW = "share_del_new";
    public static final String DB_MENUCACHE_FAVORITE_MENU = "menucache_favorite_menu";
    public static final String DB_MENUCACHE_RECENT_MENU = "menucache_recent_menu";
    public static final String DB_SEO_COMPANY = "seo_company";
    public static final String DB_YUREN_WEAPON_RELATION = "yuren_weapon_relation";
    public static final String DB_YUREN_USER = "yuren_user";
    public static final String DB_PASSPORT_WAP = "passport_wap";
    public static final String DB_CHARITY = "charity";
    public static final String DB_PAGE_RING = "page_ring";
    public static final String DB_GAME_CENTER = "game_center";
    public static final String DB_UGC_EVENT = "ugc_event";
    public static final String DB_UGC_EVENT_INFO = "ugc_event_info";
    public static final String DB_FINANCE_STATS = "finance_stats";
    public static final String DB_SAFECENTER_CHUANG = "safecenter_chuang";
    public static final String DB_USER_SCORE_AWARD_NEW = "user_score_award_new";
    public static final String DB_FRIENDFINDER_USERLOGIN = "friendfinder_userlogin";
    public static final String DB_BUDDYAPPLY_APPLICANT = "buddyapply_friendfinder_applicant";
    public static final String DB_PAGE_LOVER = "page_lover";
    public static final String DB_WORLD_CUP_USER = "world_cup_user";
    public static final String DB_WORLD_CUP_COUPLE = "world_cup_couple";
    public static final String DB_KFC_SHARE = "kfc_share";
    public static final String DB_USER_PASSPORT_INVITE = "user_passport_invite";
    public static final String DB_SEARCH_USER_FRIENDCOUNT = "search_user_friendcount";
    public static final String DB_DB_HA = "db_ha";
    public static final String DB_WAP_PAY_STAT = "wap_pay_stat";
    public static final String DB_VIP_SCORE_RECORDS = "vip_score_records";
    public static final String DB_ADDRESSBOOK = "addressbook";
    public static final String DB_FRIENDFINDER_COMMON_FRIENDS_NEW = "friendfinder_common_friends_new";
    public static final String DB_XUEBI = "xuebi";
    public static final String DB_HOTORNOT = "hotornot";
    public static final String DB_FORUM = "forum";
    public static final String DB_FORUM_INFO = "forum_info";
    public static final String DB_HIGH_SCHOOL_INVITE_AWARD = "high_school_invite_award";
    public static final String DB_ASP_ORG = "asp_org";
    public static final String DB_PAGE_THREAD = "page_thread";
    public static final String DB_MUSIC = "music";
    public static final String DB_MUSIC_FAV_PLAYLIST = "music_fav_playlist";
    public static final String DB_MUSIC_PLAYLIST = "music_playlist";
    public static final String DB_MUSIC_COMMENT_USER = "music_comment_user";
    public static final String DB_MUSIC_COMMENT_SONG = "music_comment_song";
    public static final String DB_MUSIC_LIKE_USER = "music_like_user";
    public static final String DB_MUSIC_LIKE_SONG = "music_like_song";
    public static final String DB_MUSIC_FAVED_PLAYLIST = "music_faved_playlist";
    public static final String DB_USER_GAME_ACCOUNT = "user_game_account";
    public static final String DB_CLUB_ORG = "club_org";
    public static final String DB_CLUB_ORG_FANS = "club_org_fans";
    public static final String DB_CLUB_ORG_SCORE = "club_org_score";
    public static final String DB_CLUB_ORG_BIZ = "club_org_biz";
    public static final String DB_FANS_CLUB_ORG = "fans_club_org";
    public static final String DB_CLUB_ORG_VIDEO_COMMENT = "club_org_video_comment";
    public static final String DB_CLUB_ORG_RING = "club_org_ring";
    public static final String DB_CLUB_ORG_STAT = "club_org_stat";
    public static final String DB_MESSAGE_ATTACHMENT = "message_attachment";
    public static final String DB_RELATION_CENTER = "relation_center";

    public static final String PATTERN_USER_SCORE(final int id) {
	return "";
    }

    public static final String PATTERN_RELATION(final int id) {
	return _PATTERN_HELPER("relation_", id, 100);
    }

    public static final String PATTERN_GOSSIP(final int id) {
	return _PATTERN_HELPER("gossip_", id, 100);
    }

    public static final String PATTERN_BLOG(final int id) {
	return _PATTERN_HELPER("blog_", id, 100);
    }

    public static final String PATTERN_MESSAGE(final int id) {
	return _PATTERN_HELPER("message_", id, 100);
    }

    public static final String PATTERN_ALBUM(final int id) {
	return _PATTERN_HELPER("album_", id, 100);
    }

    public static final String PATTERN_GROUP(final int id) {
	return _PATTERN_HELPER("group_", id, 100);
    }

    public static final String PATTERN_CLASS(final int id) {
	return _PATTERN_HELPER("class_", id, 100);
    }

    public static final String PATTERN_BUDDYAPPLY_APPLICANT(final int id) {
	return _PATTERN_HELPER("buddyapply_applicant_", id, 100);
    }

    public static final String PATTERN_MEMBER(final int id) {
	return _PATTERN_HELPER("member_", id, 100);
    }

    public static final String PATTERN_MEMBER(final long id) {
	return _PATTERN_HELPER("member_", id, 100);
    }

    private static final String _PATTERN_HELPER(final String prefix,
	    final int id, final int cluster) {
	StringBuffer buff = new StringBuffer();
	buff.append(prefix);
	buff.append(Math.abs(id) % cluster);
	return buff.toString();
    }

    private static final String _PATTERN_HELPER(final String prefix,
	    final long id, final int cluster) {
	StringBuffer buff = new StringBuffer();
	buff.append(prefix);
	buff.append(Math.abs(id) % cluster);
	return buff.toString();
    }

    private Ice.Communicator createIceCommunicator() {
	Ice.Properties properties = Ice.Util.createProperties();
	properties.setProperty("Ice.Override.Timeout", "300");
	properties.setProperty("Ice.Override.ConnectTimeout", "300");
	properties.setProperty("IceGrid.InstanceName", "XiaoNei");

	properties.setProperty("Ice.ThreadPool.Client.Size", "1");
	properties.setProperty("Ice.ThreadPool.Client.SizeMax", "1000");
	properties.setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties.setProperty("Ice.ThreadPool.Server.Size", "1");
	properties.setProperty("Ice.ThreadPool.Server.SizeMax", "1000");
	properties.setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	// properties.setProperty(myName + ".AdapterId", myName);
	properties.setProperty(myName + ".Endpoints", "tcp -h "
		+ xce.util.tools.IPAddress.getLocalAddress());
	// properties.setProperty(myName + ".PublishedEndpoints", "tcp -h
	// 10.2.6.17 -p 50001");

	// properties.setProperty("Ice.Trace.Protocol", "3");
	// properties.setProperty("Ice.Trace.Network", "3");

	// properties.setProperty("Ice.ProgramName", "ice");
	// properties.setProperty("Ice.UseSyslog", "1");

	String hostname = xce.util.tools.IPAddress.getLocalHostName();
	_logger.info("OceChannel got hostname: " + hostname);
	if (hostname.equals("hedgehog.xiaonei.com")
		|| hostname.equals("hedgehog")) {
	    _logger.warn("oce.jar using TestRegistry as registry");
	    properties
		    .setProperty(
			    "Ice.Default.Locator",
			    "XiaoNei/Locator:default -h TestRegistry -p 12000:default -h TestRegistryReplica1 -p 12001:default -h TestRegistryReplica2 -p 12002");
	} else {
	    _logger.warn("oce.jar using XiaoNeiRegistry as registry");
	    properties
		    .setProperty(
			    "Ice.Default.Locator",
			    "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002");
	}

	Ice.InitializationData initData = new InitializationData();
	initData.properties = properties;
	Ice.Communicator ic = Ice.Util.initialize(initData);
	return ic;
    }

    public Connection getReadConnection(String name, String pattern)
	    throws SQLException {
	return _manager.getReader(name, pattern);
    }

    public Connection getWriteConnection(String name, String pattern)
	    throws SQLException {
	return _manager.getWriter(name, pattern);
    }

    public Connection getReadConnection(String name) throws SQLException {
	return _manager.getReader(name);
    }

    public Connection getWriteConnection(String name) throws SQLException {
	return _manager.getWriter(name);
    }

    public DataSource getReadDataSource(String name, String pattern)
	    throws SQLException {
	return _manager.getReaderDataSource(name, pattern);
    }

    public DataSource getWriteDataSource(String name, String pattern)
	    throws SQLException {
	return _manager.getWriterDataSource(name, pattern);
    }

    public DataSource getReadDataSource(String name) throws SQLException {
	return _manager.getReaderDataSource(name);
    }

    public DataSource getWriteDataSource(String name) throws SQLException {
	return _manager.getWriterDataSource(name);
    }
    
    public void alert(String name,String info){
    try {
	_manager.alert(name, info);
    } catch (Exception e) {
      _logger.warn("XceAdapter.alert ", e);
    }
    }

    public void run() {
	while (true) {
	    try {
		_manager.detect();
		Thread.sleep(60000);
	    } catch (Exception e) {
		_logger.warn("XceAdapter.run ", e);
	    }
	}
    }

    public static void main(String[] args) throws SQLException,
	    InterruptedException {
	while (true) {
	    Connection conn = null;
	    try {
		conn = XceAdapter.getInstance().getWriteConnection("zhaoning");
		PreparedStatement stmt = null;
		try {
		    String sql = "SELECT now()";
		    stmt = conn.prepareStatement(sql);
		    ResultSet res = null;
		    try {
			res = stmt.executeQuery();
			while (res.next()) {
			    System.out.println(res.getString(1));
			}
		    } catch (Exception e) {
			e.printStackTrace();
		    } finally {
			if (res != null)
			    res.close();
		    }
		} catch (Exception e) {
		    e.printStackTrace();

		} finally {
		    if (stmt != null)
			stmt.close();
		}
	    } catch (Exception e) {
		e.printStackTrace();
	    } finally {
		if (conn != null)
		    conn.close();
	    }

	    Thread.sleep(1000);
	}
    }
}
