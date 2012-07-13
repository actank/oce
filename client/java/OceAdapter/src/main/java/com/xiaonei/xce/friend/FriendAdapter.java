package com.xiaonei.xce.friend;

import java.io.IOException;
import java.util.List;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.buddybyidcache.*;;


public class FriendAdapter {

	public static boolean hasShareFriends(int useridA, int useridB) {
		List<Integer> friendsOfA;
		try {
			friendsOfA = BuddyByIdCacheAdapter.getInstance().getFriendListAsc(useridA, -1);
		} catch (Throwable e) {
			Oce.getLogger().error(" FriendAdapter " + e);
			return false;
		}
		if (friendsOfA == null || friendsOfA.size() == 0) {
			return false;
		}
		for (int friendOfA : friendsOfA) {
			if (friendOfA == useridB) {
				return true;
			}
		}
		List<Integer> friendsOfB;
		try {
			friendsOfB = BuddyByIdCacheAdapter.getInstance().getFriendListAsc(useridB, -1);
		} catch (Throwable e) {
			Oce.getLogger().error(" FriendAdapter " + e);
			return false;
		}
		if (friendsOfB == null || friendsOfB.size() == 0) {
			return false;
		}
		int indexInFriendsOfB = 0;
		for (int friendOfA : friendsOfA) {
			while (friendOfA >= friendsOfB.get(indexInFriendsOfB)) {
				if (friendOfA == friendsOfB.get(indexInFriendsOfB)) {
					return true;
				}
				if (++indexInFriendsOfB == friendsOfB.size()) {
					return false;
				}
			}
		}
		return false;
	}

	public static void main(String[] args) throws IOException {
		/*
		 * BufferedReader reader = new BufferedReader(new FileReader(args[0]));
		 * BufferedWriter writer = new BufferedWriter(new FileWriter(args[1]));
		 * String line = reader.readLine(); while (line != null) { int useridA =
		 * Integer.valueOf(line.substring(0, line.indexOf(' '))); int useridB =
		 * Integer .valueOf(line.substring(line.indexOf(' ') + 1)); if
		 * (hasShareFriends(useridA, useridB)) { writer.write((useridA + " " +
		 * useridB + " " + hasShareFriends( useridA, useridB)).toString());
		 * writer.newLine(); } line = reader.readLine(); } reader.close();
		 * writer.flush(); writer.close();
		 */
		int userA = 220840375;
		int userB = 222677625;
		boolean flag = hasShareFriends(userA, userB);
			if (flag) {
				System.out.println(userA + " " + userB + " " + flag);
			}
		}

	
}
