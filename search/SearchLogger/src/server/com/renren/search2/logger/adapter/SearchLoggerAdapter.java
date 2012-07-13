package com.renren.search2.logger.adapter;

import com.renren.search2.logger.SearchLoggerPrx;
import com.renren.search2.logger.SearchLoggerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

public class SearchLoggerAdapter extends ServiceAdapter {

	private static SearchLoggerAdapter _instance = new SearchLoggerAdapter();

	public static SearchLoggerAdapter getInstance() {
		return _instance;
	}

	private SearchLoggerAdapter() {
		super("@StatisticsLogger", 1, Channel
				.newAddressBookChannel("StatisticsLogger"));
	}

	/*-- 以下同时列出了各同的代理调用方式, 可根据实际需要保留和删除 --*/
	private Vector<SearchLoggerPrx> managers = new Vector<SearchLoggerPrx>();
	private Vector<SearchLoggerPrx> managersOneway = new Vector<SearchLoggerPrx>();

	private SearchLoggerPrx getSearchLogger(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,
				SearchLoggerPrxHelper.class, 10000);
	}

	private SearchLoggerPrx getSearchLoggerOneway(int userId) {
		return locateProxy(managersOneway, "M", userId, Channel.Invoke.Oneway,
				SearchLoggerPrxHelper.class, 3000);
	}

	private SearchLoggerPrx getSearchLoggerUDP() {
		return locateProxy("M", Channel.Invoke.Udp,
				SearchLoggerPrxHelper.class, 3000);
	}

	public void access(Map<String, String> log) {
		getSearchLogger(0).access(log);
	}

	public String getExplain(Map<String, String> log) {
		return getSearchLogger(0).getExplain(log);
	}

	public static void main(String[] args) {
		Map<String, String> map = new HashMap<String, String>();
		map.put("time",
				String.valueOf(Calendar.getInstance().getTimeInMillis()));
		map.put("hostId", String.valueOf(201783098));
		map.put("userId", String.valueOf(193577421));
		map.put("query", "史玉龙");
		map.put("relation", String.valueOf(7));
		map.put("commonFriends", String.valueOf(235));
		map.put("totalCount", String.valueOf(453));
		map.put("position", String.valueOf(4));
		System.out.println(SearchLoggerAdapter.getInstance().getExplain(map));
	}
}
