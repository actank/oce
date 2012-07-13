package com.xiaonei.xce.p;

import java.util.List;

public class PAdapter {
	private static PAdapter _instance = new PAdapter();

	public static PAdapter getInstance() {
		return _instance;
	}

	public int doPerformanceTest(String sid) {
/*		if (sid == null)
			return -1;
		try {
			Integer userId = Integer.parseInt(sid);
			if (userId == null)
				return -1;
			List<DefaultOffer> result = DefaultOfferFriendsAdapter
					.getInstance().get(userId);
			return result.size();
		} catch (NumberFormatException e) {
			Oce.getLogger().error(this.getClass().getName() + e);
		}*/
		return -1;
	}
}

