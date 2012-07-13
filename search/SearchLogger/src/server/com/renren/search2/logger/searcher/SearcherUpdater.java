package com.renren.search2.logger.searcher;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.xiaonei.search2.BasicSearchChannel;
import com.xiaonei.search2.search.RenrenParallelMultiSearcher;
import com.xiaonei.search2.server.IndexContainerPrx;
import com.xiaonei.search2.server.IndexRegistryPrx;
import com.xiaonei.search2.server.IndexRegistryPrxHelper;
import com.xiaonei.search2.service.IndexManagerService;

public class SearcherUpdater extends Thread {

	private BasicSearchChannel channel;

	public static RenrenParallelMultiSearcher myUserSearcher = null;

	public SearcherUpdater() {
		this.channel = new BasicSearchChannel();
	}

	public void run() {
		while (true) {
			try {
				updateSearcherList();
				Thread.sleep(100000);
			} catch (Exception e) {
				e.printStackTrace();
				try {
					Thread.sleep(5000);
				} catch (InterruptedException e1) {
					e1.printStackTrace();
				}
			}
		}
	}

	public void updateSearcherList() {
		IndexRegistryPrx registry = IndexRegistryPrxHelper
				.uncheckedCast(channel
						.getCommunicator()
						.stringToProxy(
								"IndexRegistry@IndexRegistry.IndexRegistry.IndexRegistry"));
		Map<String, IndexContainerPrx[]> g = registry
				.getSearcherGroup(SearcherPrivatePool.type);
		System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>"
				+ SearcherPrivatePool.type);
		System.out.println("From registry: " + SearcherPrivatePool.type + " "
				+ g.toString());
		List<RenrenParallelMultiSearcher> usersearcherlist = new ArrayList<RenrenParallelMultiSearcher>();
		System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>"
				+ IndexManagerService.getSearcherName());

		if (g.containsKey(IndexManagerService.getSearcherName())) {
			myUserSearcher = IndexManagerService.initUserSearcher(g
					.get(IndexManagerService.getSearcherName()));
		} else {
			myUserSearcher = null;
		}

		if (myUserSearcher == null) {
			System.out.println("myUserSearcher == NULL");
			for (Entry<String, IndexContainerPrx[]> entry : g.entrySet()) {
				// _logger.debug("Add user search group: " + entry.getKey());
				usersearcherlist.add(IndexManagerService.initUserSearcher(entry
						.getValue()));
			}
			System.out.println("放入searchermap " + usersearcherlist.size()
					+ " 个");
			SearcherPrivatePool.searchermap.put("User", usersearcherlist);
		}

		System.out.println("Number of user searchers: " + g.size());
	}
}
