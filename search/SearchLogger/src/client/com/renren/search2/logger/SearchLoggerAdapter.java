package com.renren.search2.logger;

import com.renren.search2.logger.SearchLoggerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import java.util.Vector;

public class SearchLoggerAdapter extends ServiceAdapter {

	private static SearchLoggerAdapter _instance = new SearchLoggerAdapter();

	public static SearchLoggerAdapter getInstance() {
		return _instance;
	}

	private SearchLoggerAdapter() {
		super("@SearchLogger", 10, Channel
				.newAddressbookChannel("SearchLogger"));
	}

	/*-- 以下同时列出了各同的代理调用方式, 可根据实际需要保留和删除 --*/
	private Vector<SearchLoggerPrx> managers = new Vector<SearchLoggerPrx>();
	private Vector<SearchLoggerPrx> managersOneway = new Vector<SearchLoggerPrx>();

	private SearchLoggerPrx getSearchLogger(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,
				SearchLoggerPrxHelper.class, 300);
	}

	private SearchLoggerPrx getSearchLoggerOneway(int userId) {
		return locateProxy(managersOneway, "M", userId, Channel.Invoke.Oneway,
				SearchLoggerPrxHelper.class, 300);
	}

	private SearchLoggerPrx getSearchLoggerUDP() {
		return locateProxy("M", Channel.Invoke.Udp,
				SearchLoggerPrxHelper.class, 300);
	}

	public void access(Map<String, String> log) {
		getSearchLoggerUDP(log);
	}

}
