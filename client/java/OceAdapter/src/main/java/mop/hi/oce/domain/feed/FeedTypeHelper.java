package mop.hi.oce.domain.feed;

import java.util.StringTokenizer;

public class FeedTypeHelper {

	public final static String FEED_TYPE_REPLY_LEAVEWORD = "0.3.1";

	public final static String FEED_TYPE_REPLY_BLOG = "0.3.2";

	public final static String FEED_TYPE_REPLY_PHOTO = "0.3.3";

	public final static String FEED_TYPE_REPLY_TRIBE = "0.3.4";

	public final static String FEED_TYPE_REPLY_CLUB = "0.3.5";

	public final static String FEED_TYPE_SHARE_PAGE = "1.3.1";
	public final static String FEED_TYPE_SHARE_BLOG = "1.3.2";
	public final static String FEED_TYPE_SHARE_PHOTO = "1.3.3";
	public final static String FEED_TYPE_SHARE_ALBUM = "1.3.4";
	public final static String FEED_TYPE_SHARE_TRIBE_THREAD = "1.5.5";
	public final static String FEED_TYPE_SHARE_CLUB_THREAD = "1.5.6";
	public final static String FEED_TYPE_SHARE_RSS = "1.5.7";
	public final static String FEED_TYPE_SHARE_MARKETPLACE = "1.5.8";
	public final static String FEED_TYPE_SHARE_MOVIE_COMMENT = "1.5.9";
	public final static String FEED_TYPE_SHARE_VIDEO = "1.5.10";
	public final static String FEED_TYPE_SHARE_MUSIC = "1.5.11";
	public final static String FEED_TYPE_SHARE_MOVIE = "1.5.12";
	public final static String FEED_TYPE_SHARE_CAMPAIGN = "1.5.13";
	
	public final static String FEED_TYPE_TRIBE_CREATE = "2.5.1";

	public final static String FEED_TYPE_TRIBE_JOIN = "2.5.2";

	public final static String FEED_TYPE_TRIBE_QUIT = "2.7.3";

	public final static String FEED_TYPE_TRIBE_THREAD = "2.7.4";

	public final static String FEED_TYPE_TRIBE_CLUB = "2.5.5";

	public final static String FEED_TYPE_CAMPAIGN_CREATE = "3.3.1";

	public final static String FEED_TYPE_CAMPAIGN_JOIN = "3.5.2";

	public final static String FEED_TYPE_CAMPAIGN_QUIT = "3.7.3";

	public final static String FEED_TYPE_CAMPAIGN_UPDATE = "3.5.4";

	public final static String FEED_TYPE_NETWORK_JOIN = "4.3.1";

	public final static String FEED_TYPE_NETWORK_QUIT = "4.3.2";

	public final static String FEED_TYPE_NETWORK_SWITCH = "4.3.3";

	public final static String FEED_TYPE_PROFILE_HEADURL = "5.3.1";

	public final static String FEED_TYPE_PROFILE_STATUS = "5.3.2";

	public final static String FEED_TYPE_PROFILE_BASIC = "5.5.3";

	public final static String FEED_TYPE_PROFILE_INTRO = "5.5.4";

	public final static String FEED_TYPE_PROFILE_CONTACT = "5.5.5";

	public final static String FEED_TYPE_PROFILE_SCHOOL = "5.5.6";

	public final static String FEED_TYPE_PROFILE_WORK = "5.5.7";

	public final static String FEED_TYPE_BLOG_PUBLISH = "6.3.1";

	public final static String FEED_TYPE_PHOTO_PUBLISH = "7.3.1";

	public final static String FEED_TYPE_PHOTO_TAG = "7.3.2";

	public final static String FEED_TYPE_GIFT = "8.5.1";
	public final static String FEED_TYPE_GIFT_FREE = "8.5.2";
	public final static String FEED_TYPE_GIFT_DONATE = "8.5.3";
	
	public final static String FEED_TYPE_CAT = "9.5.1";
	public final static String FEED_TYPE_CAT_LEVELUP = "9.5.2";

	public final static String FEED_TYPE_IM = "10.5.1";

	public final static String FEED_TYPE_INVITATION = "11.5.1";
	
	public final static String FEED_TYPE_INVITATION_REG = "11.5.2";
	
	public final static String FEED_TYPE_INVITATION_FIND = "11.5.3";

	public final static String FEED_TYPE_ADIDAS_CREATE = "12.5.1";

	public final static String FEED_TYPE_ADIDAS_JOIN = "12.5.2";

	public final static String FEED_TYPE_ADIDAS_BLOG = "12.5.3";

	public final static String FEED_TYPE_FRIENDSHIP = "13.5.1";
	
	public final static String FEED_TYPE_FRIENDSHIP_2DIMENSION = "13.5.2";

	public final static String FEED_TYPE_MARKET_MSG = "14.5.1";

	public final static String FEED_TYPE_MOVIE_COMMENT = "15.5.1";

	
	public final static String FEED_TYPE_CLASS_JOIN = "16.5.1";
	
	public final static String FEED_TYPE_CLASS_LEAVEWORD = "16.5.2";
	
	public final static String FEED_TYPE_CLASS_PHOTO = "16.5.3";
	
	public final static String FEED_TYPE_CLASS_FILE = "16.5.4";
	
	public final static String FEED_TYPE_CLASS_NOTICE = "16.5.5";
	
	
	public final static String FEED_TYPE_SPONSORED = "17.5.1";
	
	public final static String FEED_TYPE_SPONSORED_2 = "17.5.2";
	
	public final static String FEED_TYPE_SPONSORED_AD = "17.5.3";
	
	public final static String FEED_TYPE_VIP_ON = "18.5.1";
	public final static String FEED_TYPE_VIP_OFF = "18.5.2";
	
	public final static String FEED_TYPE_GAME_SANGUO = "19.5.1";
	
	public final static String FEED_TYPE_MMS = "20.5.1";
	
	public final static String FEED_TYPE_HEADURL = "21.5.1";
	
	public final static String FEED_TYPE_AD_SUPPORT_1 = "50.5.1";
	public final static String FEED_TYPE_AD_SUPPORT_2 = "50.5.2";
	public final static String FEED_TYPE_AD_SUPPORT_3 = "50.5.3";
	public final static String FEED_TYPE_AD_SUPPORT_4 = "50.5.4";
	public final static String FEED_TYPE_AD_SUPPORT_5 = "50.5.5";
	public final static String FEED_TYPE_AD_SUPPORT_6 = "50.5.6";
	public final static String FEED_TYPE_AD_SUPPORT_7 = "50.5.7";
	public final static String FEED_TYPE_AD_SUPPORT_8 = "50.5.8";
	public final static String FEED_TYPE_AD_SUPPORT_9 = "50.5.9";
	public final static String FEED_TYPE_AD_SUPPORT_0 = "50.5.0";
	//	
	// private static Map<String, FeedFilter> filters = new HashMap<String,
	// FeedFilter>();
	//	
	// static {
	// filters.put(FEED_TYPE_REPLY_LEAVEWORD, new FeedFilter(0, 1));
	// filters.put(FEED_TYPE_REPLY_BLOG, new FeedFilter(0, 1));
	// filters.put(FEED_TYPE_REPLY_PHOTO, new FeedFilter(0, 1));
	// filters.put(FEED_TYPE_REPLY_TRIBE, new FeedFilter(0, 1));
	// filters.put(FEED_TYPE_REPLY_CLUB, new FeedFilter(0, 1));
	//		
	// filters.put(FEED_TYPE_SHARE_PAGE, new FeedFilter(1, 1));
	// filters.put(FEED_TYPE_SHARE_BLOG, new FeedFilter(1, 1));
	// filters.put(FEED_TYPE_SHARE_PHOTO, new FeedFilter(1, 1));
	// filters.put(FEED_TYPE_SHARE_ALBUM, new FeedFilter(1, 1));
	// filters.put(FEED_TYPE_SHARE_TRIBE_THREAD, new FeedFilter(1, 2));
	// filters.put(FEED_TYPE_SHARE_CLUB_THREAD, new FeedFilter(1, 2));
	//		
	// filters.put(FEED_TYPE_TRIBE_THREAD, new FeedFilter(2, 1));
	// filters.put(FEED_TYPE_TRIBE_JOIN, new FeedFilter(2, 2));
	// filters.put(FEED_TYPE_TRIBE_QUIT, new FeedFilter(2, 3));
	//		
	// filters.put(FEED_TYPE_CAMPAIGN_CREATE, new FeedFilter(3, 1));
	// filters.put(FEED_TYPE_CAMPAIGN_UPDATE, new FeedFilter(3, 2));
	// filters.put(FEED_TYPE_CAMPAIGN_JOIN, new FeedFilter(3, 2));
	// filters.put(FEED_TYPE_CAMPAIGN_QUIT, new FeedFilter(3, 3));
	//		
	// filters.put(FEED_TYPE_NETWORK_JOIN, new FeedFilter(4, 1));
	// filters.put(FEED_TYPE_NETWORK_QUIT, new FeedFilter(4, 1));
	// filters.put(FEED_TYPE_NETWORK_SWITCH, new FeedFilter(4, 1));
	//		
	// filters.put(FEED_TYPE_PROFILE_HEADURL, new FeedFilter(5, 1));
	// filters.put(FEED_TYPE_PROFILE_STATUS, new FeedFilter(5, 1));
	// filters.put(FEED_TYPE_PROFILE_BASIC, new FeedFilter(5, 2));
	// filters.put(FEED_TYPE_PROFILE_INTRO, new FeedFilter(5, 2));
	// filters.put(FEED_TYPE_PROFILE_CONTACT, new FeedFilter(5, 2));
	// filters.put(FEED_TYPE_PROFILE_SCHOOL, new FeedFilter(5, 2));
	// filters.put(FEED_TYPE_PROFILE_WORK, new FeedFilter(5, 2));
	//		
	// filters.put(FEED_TYPE_BLOG_PUBLISH, new FeedFilter(6, 1));
	//		
	// filters.put(FEED_TYPE_PHOTO_PUBLISH, new FeedFilter(7, 1));
	// };
	//	
	// public static FeedFilter makeFeedFilter(String value) {
	// return new FeedFilter(value);
	// }
	//	
	// public static FeedFilter makeFeedFilter(int x, int y) {
	// return new FeedFilter(x, y);
	// }
	//
	// public static FeedFilter appendFeedFilter(FeedFilter filter, int x, int
	// y) {
	// filter.mark(x, y);
	// return filter;
	// }
	//	
	// public static int yAxisOfFeedFilter(FeedFilter filter, int x) {
	// return filter.yAxisOf(x);
	// }
	//	
	// public static FeedFilter getFeedFilter(String type) {
	// return filters.get(type);
	// }

	public static String getX(String type) {
		StringTokenizer st = new StringTokenizer(type, ".");
		if (st.hasMoreTokens()) {
			return st.nextToken();
		}
		return null;
	}

	public static String getTopCategory(String type) {
		return getX(type);
	}

	public static String getY(String type) {
		StringTokenizer st = new StringTokenizer(type, ".");
		int count = 0;
		while (st.hasMoreTokens()) {
			String s = st.nextToken();
			if (count++ == 1) {
				return s;
			}
		}
		return "5";
	}

	public static String getLevel(String type) {
		return getY(type);
	}

	public static String getZ(String type) {
		StringTokenizer st = new StringTokenizer(type, ".");
		int count = 0;
		while (st.hasMoreTokens()) {
			String s = st.nextToken();
			if (count++ == 2) {
				return s;
			}
		}
		return "1";
	}

	public static String getSubCategory(String type) {
		return getZ(type);
	}
}
