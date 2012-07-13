package com.xiaonei.xce.hotstate;

import java.util.List;
import java.util.Map;

import xce.clusterstate.ReplicatedClusterAdapter;

import com.xiaonei.search.state.HotStateSubscribePrx;
import com.xiaonei.search.state.HotStateSubscribePrxHelper;

public class HotStateSubscribeAdapter extends ReplicatedClusterAdapter {
	private HotStateSubscribeAdapter() {
		super("ControllerStateKeyWords", 120);
	}

	/*
	 * State（doing)表中具体字段在HashMap中key名约定如下: id; userid; content; dtime;
	 * commentCount; //对应State表中comment_count repeatCount;
	 * //对应State表中repeat_count type; link; source;
	 * repeatCountTotal;//对应State表中repeat_count_total markbit;
	 */
	public class STATE_FIELD {
		static final String ID = "id";
		static final String USERID = "userid";
		static final String CONTENT = "content";
		static final String ROOTCONTENT = "rootContent";
		static final String DTIME = "dtime";
	}

	public class INFO_FIELD {
		static final String TYPE = "type";
		static final String PATTERN = "pattern";
		static final String TOTAL = "total";
		static final String TIME = "time";
	}

	private static HotStateSubscribeAdapter _instance = new HotStateSubscribeAdapter();

	public static HotStateSubscribeAdapter getInstance() {
		return _instance;
	}

	public HotStateSubscribePrx getProxy(int userid) {
		Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
		if (prx0 != null) {
			HotStateSubscribePrx prx = HotStateSubscribePrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private boolean subscribe(String type, String pattern, int maxSize,
			String passwd) {
		return getProxy(0).subscribe(type, pattern, maxSize, passwd);
	}

	private boolean unSubscribe(String type) {
		return getProxy(0).unSubscribe(type);
	}

	private boolean patternTest(String pattern, String state) {
		return getProxy(0).test(pattern, state);
	}

	public List<Map<String, String>> getLastState(String type, int limit) {
		return getProxy(0).getNewState(type, limit);
	}

	public Map<String, String> showTypeInfo(String type) {
		return getProxy(0).showTypeInfo(type);
	}
}
