package com.xiaonei.xce.test.buddysixdegrees;

import xce.buddy.BuddySixDegreesManagerPrx;
import xce.buddy.BuddySixDegreesManagerPrxHelper;
import xce.buddy.RouteList;
import xce.util.channel.Channel;

import Ice.ObjectPrx;

import com.xiaonei.xce.buddysixdegrees.BuddySixDegreesAdapter;

public class BuddySixDegreesMiniTest {
	public static void main(String[] args) {
		try {
			int host = 30010;
			int guest = 30030;
			
			int begin = 0;
			int limit = 100000;
			//test1
//			ObjectPrx op=Channel.newOceChannel("").getCommunicator().stringToProxy("M@BuddySixDegrees0");
//			BuddySixDegreesManagerPrx bsda = BuddySixDegreesManagerPrxHelper.uncheckedCast(op);
//			bsda.computeTaskStart(host, guest);
			
//			RouteList testResult = bsda.getRouteList(host, guest, begin, limit);
//			System.out.println("size:" + testResult.friendsRoutList.length);
			
			//online
			BuddySixDegreesAdapter _adapter = new BuddySixDegreesAdapter();
//			_adapter.computeTaskStar(host, guest);
		
			RouteList result = _adapter.getRouteList(host, guest, begin, limit);
			
			System.out.println("size:" + result.friendsRoutList.length + " isFinish: " + result.isEnd);
			
//			for (int[] i : result.friendsRoutList) {
//				for (int f : i) {
//					System.out.print(f + ":");
//				}
//				System.out.println();
//			}
			System.exit(0);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}