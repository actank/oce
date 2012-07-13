package xce.tools.objectcache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import mop.hi.svc.model.FriendDistManagerPrx;
import mop.hi.svc.model.FriendDistManagerPrxHelper;

import xce.util.channel.Channel;
import MyUtil.ObjectCachePrx;
import MyUtil.ObjectCachePrxHelper;

public class InvokeFriendDistAction extends Consumer {
	private final String endpoints;
	private final int cluster;

	private final Ice.Communicator ic;
	private final FriendDistManagerPrx prx[];
	private static final Map<String, String> ctx = new HashMap<String, String>();
	static {
		ctx.put("LEVEL", "DIFF");
	}

	public InvokeFriendDistAction(Producer producer) {
		super(producer);
		this.endpoints = "FriendDist";
		this.cluster = 10;
		ic = Channel.newOceChannel(endpoints).getCommunicator();
		prx = new FriendDistManagerPrx[cluster];
		for (int i = 0; i < cluster; ++i) {
			prx[i] = FriendDistManagerPrxHelper.uncheckedCast(ic
					.stringToProxy("FDM@" + this.endpoints + i));
		}
	}

	@Override
	protected void consume(Long[] batch) {
		System.out.println("FriendDist reloading " + batch.length + " objects");
		Vector<ArrayList<Long>> seperate = new Vector<ArrayList<Long>>();
		for (int i = 0; i < cluster; ++i) {
			seperate.add(new ArrayList<Long>());
		}
		for (Long one : batch) {
			seperate.get((int) (one % cluster)).add(one);
		}

		for (int i = 0; i < cluster; ++i) {
			ArrayList<Long> sepBatch = seperate.get(i);
			long[] sepLongBatch = new long[sepBatch.size()];
			for (int pos = 0; pos < sepBatch.size(); ++pos) {
				sepLongBatch[pos] = sepBatch.get(pos);
			}
			for (Long userId : sepBatch) {
				prx[i].rebuildFriendDist((int)(userId/1), ctx);
			}
		}
	}

}
