package com.xiaonei.xcetest.invoke;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import xce.util.channel.CoreChannel;

import com.xiaonei.service.buddy.BuddyCoreLogicPrx;
import com.xiaonei.service.buddy.BuddyCoreLogicPrxHelper;
import com.xiaonei.service.buddy.BuddyCoreSnapshotPrx;
import com.xiaonei.service.buddy.BuddyCoreSnapshotPrxHelper;
import com.xiaonei.service.buddy.SnapshotEntry;

import junit.framework.TestCase;

public class BuddyCoreSnapshotTester extends TestCase {

	BuddyCoreSnapshotPrx _prx = BuddyCoreSnapshotPrxHelper
			.uncheckedCast(CoreChannel.newOceChannel("BuddyCoreSnapshotTester")
					.getCommunicator().stringToProxy("M@BuddyCoreSnapshotA"));

	BuddyCoreLogicPrx _logic = BuddyCoreLogicPrxHelper.uncheckedCast(CoreChannel
			.newOceChannel("BuddyCoreLogicTester").getCommunicator()
			.stringToProxy("BCL@BuddyCoreLogic").ice_timeout(2000));

	private BuddyCoreSnapshotPrx getProxy(boolean useLogic) {
		if (useLogic) {
			BuddyCoreSnapshotPrx[] proxies = _logic.getSnapshots();
			System.out.println("Found " + proxies.length + " proxies");
			BuddyCoreSnapshotPrx result = BuddyCoreSnapshotPrxHelper
					.uncheckedCast(proxies[(int) (Math.random() * proxies.length)]
							.ice_timeout(-1));
			System.out.println("Using \"" + result + "\"");
			return result;
		} else {
			return _prx;
		}
	}

	public void getShared() {
		int id = 228994679;
		Map<Integer, SnapshotEntry[]> result = getProxy(false)
				.getEntry4ShareFriend(new int[] { id, id });
		for (Entry<Integer, SnapshotEntry[]> entry : result.entrySet()) {
			System.out.println("UserID=" + entry.getKey());
			for (SnapshotEntry sentry : entry.getValue()) {
				if(sentry.toId==222479639){
					System.err.println("hhhhhhhhh");
				}
				System.out.println("\tToId=" + sentry.toId + "\tDesc="
						+ sentry.desc);
			}
		}
	}

	public void getCommon() {
		int userId = 700000015;
		Map<String, String> ctx = new HashMap<String, String>();
		// ctx.put("OVERRIDE_GETALL", "1");
		Map<Integer, SnapshotEntry[]> result = getProxy(true)
				.getEntry4CommonFriend(userId);
		for (Entry<Integer, SnapshotEntry[]> entry : result.entrySet()) {
			if (userId == entry.getKey()) {
				System.out.println("UserID=" + entry.getKey());
				for (SnapshotEntry sentry : entry.getValue()) {
					System.out.println("\tToId=" + sentry.toId + "\tDesc="
							+ sentry.desc);
				}
			} else {
				System.out.println("UserID=" + entry.getKey() + " Size="
						+ entry.getValue().length);
			}
		}
	}
}
