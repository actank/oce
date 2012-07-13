package xce.util.monitor;

import xce.util.channel.Channel;
import Ice.ObjectPrx;

public class Ping {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 2) {
			System.out.println("Usage: Ping <UM@UserN> <100>");
			System.exit(1);
		}
		String namePrefix = args[0];
		Integer idMax = Integer.valueOf(args[1]);
		for (int x = 0; x < idMax; ++x) {
			try {
				System.out.println(namePrefix + x);
				ObjectPrx prx = Channel.newOceChannel("Ping").getCommunicator()
						.stringToProxy(namePrefix + x);
				prx.ice_ping();
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
		System.exit(0);
	}

}
