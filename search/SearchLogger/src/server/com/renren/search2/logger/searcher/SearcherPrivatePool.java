package com.renren.search2.logger.searcher;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import xce.util.tools.IPAddress;

import com.xiaonei.search2.Config;
import com.xiaonei.search2.model.GroupType;
import com.xiaonei.search2.search.RenrenParallelMultiSearcher;

public class SearcherPrivatePool {
	private static Log _logger = LogFactory.getLog(SearcherPrivatePool.class);

	private static SearcherPrivatePool pool;

	public static GroupType type;

	public static Map<String, List<RenrenParallelMultiSearcher>> searchermap;

	public static List<String> groupnames;

	private SearcherUpdater thread;

	private SearcherPrivatePool(GroupType t) {
		type = t;
		groupnames = new ArrayList<String>();
		// 类型－>Searcher的映射;类型:User
		searchermap = new ConcurrentHashMap<String, List<RenrenParallelMultiSearcher>>();
		List<RenrenParallelMultiSearcher> usersearcherlist = new ArrayList<RenrenParallelMultiSearcher>();
		searchermap.put("User", usersearcherlist);
		thread = new SearcherUpdater();
		thread.start();
	}

	/** 取得搜索池 **/
	public static SearcherPrivatePool getPool(GroupType t) {
		String address = null;
		try {
			address = IPAddress.getLocalAddress().split("\\.")[1];
		} catch (ArrayIndexOutOfBoundsException e) {
			e.printStackTrace();
			System.out.println(address);
		}

		if (Config.getInstance().isTestManager()) {

			if (t == GroupType.GroupBrowse) {
				t = GroupType.TestGroupBrowse;
			}
			if (t == GroupType.GroupGuide) {
				t = GroupType.TestGroupGuide;
			}
			if (t == GroupType.GroupSearch) {
				t = GroupType.TestGroupSearch;
			}
		}
		if (pool == null) {
			pool = new SearcherPrivatePool(t);
		}
		pool.setType(t);
		return pool;
	}

	public GroupType getType() {
		return type;
	}

	public void setType(GroupType t) {
		type = t;
	}

	public RenrenParallelMultiSearcher getUsersearcher() {
		try {
			int index = (int) (searchermap.get("User").size() * Math.random());
			_logger.debug("use container id: " + index);
			// System.out.println("searcherMap.getUser.size = "
			// + searchermap.get("User").size());
			return searchermap.get("User").get(index);
		} catch (IndexOutOfBoundsException e) {
			_logger.warn(e);
		}
		return null;
	}

	public static void main(String[] args) {
		System.out.println(SearcherPrivatePool.getPool(GroupType.GroupSearch)
				.getUsersearcher());
	}
}