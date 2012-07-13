package com.xiaonei.search2.server;

import com.xiaonei.search2.server.PersonalSearcherPrxHelper;
import com.xiaonei.search3.server.PersonalSearcherPrx;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import java.util.Vector;

public class PersonalSearcherAdapter extends ServiceAdapter {

	private static PersonalSearcherAdapter _instance = new PersonalSearcherAdapter();

	public static PersonalSearcherAdapter getInstance() {
		return _instance;
	}

	private PersonalSearcherAdapter() {
		super("@PersonalSearcher", 0, Channel
				.newAddressBookChannel("PersonalSearcher"));
	}

	/*-- 以下同时列出了各同的代理调用方式, 可根据实际需要保留和删除 --*/
	private Vector<PersonalSearcherPrx> managers = new Vector<PersonalSearcherPrx>();
	private Vector<PersonalSearcherPrx> managersOneway = new Vector<PersonalSearcherPrx>();

	private PersonalSearcherPrx getPersonalSearcher(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,
				PersonalSearcherPrxHelper.class, 300);
	}

	private PersonalSearcherPrx getPersonalSearcherOneway(int userId) {
		return locateProxy(managersOneway, "M", userId, Channel.Invoke.Oneway,
				PersonalSearcherPrxHelper.class, 300);
	}

	private PersonalSearcherPrx getPersonalSearcherUDP() {
		return locateProxy("M", Channel.Invoke.Udp,
				PersonalSearcherPrxHelper.class, 300);
	}

	public List<Integer> search(int userId, Map<String,Map<String,String> > query, int begin, int limit) {
		return getPersonalSearcher(userId).search(userid, query, begin, limit)
	}

	public static void main(String[] args) {
		int userId = 220840375;

		Map<String, Map<String, String>> query = new HashMap<String, Map<String, String>>();
		Map<String, String> m = new HashMap<String, String>();
		m.put("name", "赵宁");
		query.put("name", m);
		int begin = 0;
		int limit = -1;
		List<Integer> res = PersonalSearcherAdapter.getInstance().search(
				userId, query, begin, limit);
		System.out.println(res.size());
	}
}
