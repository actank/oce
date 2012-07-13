package com.xiaonei.xce.hotword;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import xce.hotwords.DoingItem;
import xce.hotwords.HotWord;
import xce.hotwords.HotWordManagerPrx;
import xce.hotwords.HotWordManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class HotWordNewAdapter extends ServiceAdapter {
	private static HotWordNewAdapter _instance = new HotWordNewAdapter();
	private static final String endpoints = "@HotWord";

	private Vector<HotWordManagerPrx> _managers = new Vector<HotWordManagerPrx>();

	protected HotWordNewAdapter() {
		super(endpoints, 10, Channel.newBlogSearchChannel(endpoints));

	}

	public static HotWordNewAdapter getInstance() {
		return _instance;
	}

	protected HotWordManagerPrx getHotWordManager(int userId) {
		HotWordManagerPrx prx = HotWordManagerPrxHelper
				.uncheckedCast(locateProxy(_managers, "M", userId,
						Channel.Invoke.Twoway, HotWordManagerPrxHelper.class,
						300));
		return prx;
	}

	public void addDoings(List<DoingItem> items) {
		getHotWordManager(0).addDoings(items);
	}

	public List<HotWord> lastHotWords(String unit) {
		return getHotWordManager(0).lastHotWords(unit);
	}

	public List<DoingItem> searchHotWords(String unit, String key) {
		return getHotWordManager(0).searchDoings(unit, key);
	}

	public static void main(String[] args) {
		try {
			List<DoingItem> items = new ArrayList<DoingItem>();
			DoingItem item = new DoingItem();
			item.content = "Lady Gaga、韩庚、张根锡。。。连我最爱的幂幂都来成都了，这让一个不能到现场观摩的人情何以堪啊！！！！(哭)(哭)(哭))";
			item.id = 1897952963;
			item.uid = 338408199;
			item.time = (int) (System.currentTimeMillis() / 1000);
			item.meta = "";
			items.add(item);
			// HotWordNewAdapter.getInstance().addDoings(items);
			List<HotWord> words = HotWordNewAdapter.getInstance().lastHotWords(
					"min");
			int cnt =0;
			for (HotWord word : words) {
				StringBuilder builder = new StringBuilder();
				for (String like : word.likes) {
					builder.append(like + ",");
				}
				System.out.println(word.key + " " + word.alias + " "
						+ word.score + " " + word.uidFreq + " " + word.history
						+ " [" + builder.toString() + "]");
				List<DoingItem> doings = HotWordNewAdapter.getInstance()
						.searchHotWords("min", word.key);
				System.out.println(doings.get(0).content);
				if(cnt++ >=10)
					break;
			}
		} catch (Exception e) {
			System.out.println(e);
		}
		System.exit(0);
	}
}
