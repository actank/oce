package com.xiaonei.xce.search;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import xce.search.SearchId2NameCachePrx;
import xce.search.SearchId2NameCachePrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.service.ServiceAdapter;
import xce.util.tools.MathUtil;

public class SearchId2NameAdapter extends ServiceAdapter {
	private static final String endpoints = "@SearchId2NameCache";

	private Vector<SearchId2NameCachePrx> managers = new Vector<SearchId2NameCachePrx>();
	private Vector<SearchId2NameCachePrx> managersOneway = new Vector<SearchId2NameCachePrx>();

	public SearchId2NameAdapter() {
		super(endpoints, 1, Channel.newSearchChannel(endpoints));
	}

	protected SearchId2NameCachePrx getSearchId2NameCacheManager() {
		return locateProxy(managers, "ID2NAME", 0, Channel.Invoke.Twoway,
				SearchId2NameCachePrxHelper.class, 300);
	}

	public String getName(int userId) {
		return getSearchId2NameCacheManager().GetName(userId);
	}

	public Map<Integer, String> getNames(int[] userIds) {
		return getSearchId2NameCacheManager().GetNames(userIds);
	}

	public static void main(String[] args) {
		try {
      if (0 == args.length) {
        System.out.println("Usage: <user_id_list>");
        System.exit(-1);
      }

			SearchId2NameAdapter adapter = new SearchId2NameAdapter();
			List<Integer> tempUserIds = new ArrayList<Integer>();
			for (int i = 0; i < args.length; ++i) {
				tempUserIds.add(Integer.parseInt(args[i]));
			}
			int[] userIds = new int[tempUserIds.size()];
			for (int i = 0; i < tempUserIds.size(); ++i) {
				userIds[i] = tempUserIds.get(i);
			}
			Map<Integer, String> cacheResults = adapter.getNames(userIds);
			for (Map.Entry<Integer, String> entry : cacheResults.entrySet()) {
				System.out.println("id=" + entry.getKey() + ", name="
						+ entry.getValue());
			}
			System.out.println("");
		} catch (Exception e) {
		}

		System.exit(0);
	}

}
