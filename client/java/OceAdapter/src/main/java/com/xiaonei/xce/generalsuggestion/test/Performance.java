package com.xiaonei.xce.generalsuggestion.test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;
import com.xiaonei.xce.generalsuggestion.GeneralSuggestionAdapter;
import com.xiaonei.xce.generalsuggestion.view.SuggestionDataView;
import com.xiaonei.xce.usercache.DefaultUserCache;
import com.xiaonei.xce.usercache.DefaultUserCacheFactory;
import com.xiaonei.xce.usercache.UserCacheAdapter;

public class Performance {

	public static Map<Integer, String> getFriendOfFriendName(int id) {
		List<Integer> friends = BuddyByIdCacheAdapter.getInstance()
				.getFriendListAsc(id, -1);
		// List<Integer> friends = new ArrayList<Integer>();
		// friends.add(id);
		Map<Integer, String> friendOfFriend = new HashMap<Integer, String>();
		UserCacheAdapter<DefaultUserCache> adapter = new UserCacheAdapter<DefaultUserCache>(
				new DefaultUserCacheFactory());
		for (int userid : friends) {
			List<Integer> friendOfFriendIds = BuddyByIdCacheAdapter.getInstance()
					.getFriendListAsc(userid, -1);
			Map<Integer, DefaultUserCache> id2Name = adapter
					.getUserCacheMap(friendOfFriendIds);
			for (Entry<Integer, DefaultUserCache> entry : id2Name.entrySet()) {
				friendOfFriend.put(entry.getKey(), entry.getValue().getName());
				// System.out.println(entry.getKey() + " " +
				// entry.getValue().getName());
				// if (friendOfFriend.size() > 1) {
				// System.out.println("一度好友个数: " + friendOfFriend.size());
				// return friendOfFriend;
				// }
			}
		}
		friendOfFriend.remove(id);
		System.out.println("一度好友个数: " + friendOfFriend.size());
		return friendOfFriend;
	}

	public int search(int id, String query, int prefixLength) {
		GeneralSuggestionAdapter.getInstance().load(id);
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		SuggestionDataView res = null;
		try {
			int len = Math.min(prefixLength, query.length());
			res = GeneralSuggestionAdapter.getInstance().search(id,
					query.substring(0, len), 0, 10,
					new String[] { "friend_name", "page_name", "app_name" });
		} catch (Exception e) {
			System.out.println("异常: " + e.getMessage());
			return -1;
		}
		return res.getAppData().size() + res.getPageData().size()
				+ res.getFriendData().size();
	}

	public static List<String> getAllPinyin(String name) {
		List<String> result = new ArrayList<String>();
		List<Set<String>> pinyins = new ArrayList<Set<String>>();
		for (int i = 0; i < name.length(); ++i) {
			Set<String> pinyin = PinYinDict.getInstance().getPinyin(
					name.substring(i, i + 1));
			if (pinyin != null) {
				pinyins.add(pinyin);
			}
		}
		try {
			getAllPinyin(pinyins, 0, "", result);
		} catch (Exception e) {
			System.out.println("Name: " + name + " Pinyin.size: " + pinyins.size());
			e.printStackTrace();
		}
		return result;
	}

	private static void getAllPinyin(List<Set<String>> words, int start,
			String buffer, List<String> result) {
		if (start >= words.size()) {
			result.add(buffer);
			return;
		}
		Set<String> temp = words.get(start);
		if (temp == null) {
			System.out.println("Words.size: " + words.size() + " Start: " + start);
		}
		for (String word : temp) {
			getAllPinyin(words, start + 1, buffer + word, result);
		}
	}

	public static void main(String[] args) {
		int id = 62522936;
		PinYinDict.getInstance().init();
		String name = "王文博";
		List<String> pinyins = getAllPinyin(name);
		for (String pinyin : pinyins) {
			System.out.println(pinyin);
		}
	}
}
