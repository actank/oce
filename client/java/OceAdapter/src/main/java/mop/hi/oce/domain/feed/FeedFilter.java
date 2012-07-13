package mop.hi.oce.domain.feed;

public class FeedFilter {
	// maps
	public final static int FEED_X_COUNT = 16;	
	
	public final static int FEED_X_REPLY = 0;

	public final static int FEED_X_SHARE = 1;

	public final static int FEED_X_TRIBE = 2;

	public final static int FEED_X_CAMPAIGN = 3;

	public final static int FEED_X_NETWORK = 4;

	public final static int FEED_X_PROFILE = 5;

	public final static int FEED_X_BLOG = 6;

	public final static int FEED_X_PHOTO = 7;

	public final static int FEED_X_GIFT = 8;
	
	public final static int FEED_X_CAT = 9;
	
	public final static int FEED_X_IM = 10;
	
	public final static int FEED_X_INVITATION = 11;
	
	public final static int FEED_X_ADIDAS = 12;
	
	public final static int FEED_X_FRIENDSHIP = 13;
	
	public final static int FEED_X_MARKET = 14;
	
	public final static int FEED_X_MOVIE_COMMENT = 15;
	
	public final static int FEED_X_CLASS = 16;
	
	public final static int FEED_X_AD_SUPPORT = 50;
	
	
	public final static int FEED_Y_COUNT = 9;
	
	public final static int FEED_Y_NONE = 1;

	public final static int FEED_Y_LESS = 3;

	public final static int FEED_Y_DEFAULT = 5;

	public final static int FEED_Y_MORE = 7;

	public final static int FEED_Y_ALL = 9;

	public final static String FEED_FILTER_DEFAULT = "555555555555515";
	
	public final static String FEED_FILTER_WORK_DEFAULT = "555555555555555";
	
	public final static String FEED_FILTER_ALL = "11111111111111";

	private String value;

	public FeedFilter(String value) {
		if (value.length() > FEED_X_COUNT) {
			this.value = value.substring(0, FEED_X_COUNT);
		} else {
			this.value = value;
		}
	}

	public FeedFilter(int x, int y) {
		if (x > FEED_X_COUNT - 1 || x < 0 || y > FEED_Y_ALL || y < FEED_Y_NONE) {
			value = FEED_FILTER_DEFAULT;
			return;
		}
		StringBuffer strb = new StringBuffer();
		for (int i = 0; i < FEED_X_COUNT; ++i) {
			strb.append('5');
		}
		strb.setCharAt(x, (char)(y+48));
		value = strb.toString();
	}

	public FeedFilter mark(int x, int y) {
		//System.out.println("### FeedFilter.mark x=" + x + ", y=" + y);
		if (x > FEED_X_COUNT - 1 || x < 0) {			
			return this;
		}
		if (y > FEED_Y_ALL || y < FEED_Y_NONE) {
			y = FEED_Y_DEFAULT;
		}
		StringBuffer strb = new StringBuffer(value);
		while (strb.length() < FEED_X_COUNT) {
			strb.append('5');
		}
		strb.setCharAt(x, (char)(y+48));
		value = strb.toString();
		//System.out.println("### FeedFilter.mark x=" + x + ", y=" + y + ", result=" + value);
		return this;
	}

	public void reset() {
		this.value = FEED_FILTER_DEFAULT;
	}
	
	public int yAxisOf(int x) {
		if (value == null || x >= value.length() || x < 0) {
			return FEED_Y_NONE;
		}
		return value.charAt(x)-48;
	}
	
	public int length() {
		return value.length();
	}
	
	public String getValue() {
		return value;
	}
	
	public String toString() {
		return getValue();
	}
}
