package xce.tools.objectcache;

import java.util.ArrayList;
import java.util.Vector;

import xce.util.channel.Channel;
import MyUtil.ObjectCachePrx;
import MyUtil.ObjectCachePrxHelper;

public class RemoveFeedAction extends Consumer {
	private final String endpoints;
	private final int[] categories;
	private final int cluster;

	private final Ice.Communicator ic;
	private final ObjectCachePrx prx[];

	public RemoveFeedAction(Producer producer, String endpoints, int[] categories,
			int cluster) {
		super(producer);
		this.endpoints = endpoints;
		this.categories = categories;
		ic = Channel.newFeedChannel(endpoints).getCommunicator();
		if( cluster == 0 ){
			this.cluster = 1;
			prx = new ObjectCachePrxHelper[1];
			prx[0] = ObjectCachePrxHelper.uncheckedCast(ic.stringToProxy("SC@"
						+ this.endpoints ));
		}
		else {
			this.cluster = cluster;			
			prx = new ObjectCachePrxHelper[cluster];
			for (int i = 0; i < cluster; ++i) {
				prx[i] = ObjectCachePrxHelper.uncheckedCast(ic.stringToProxy("SC@"
						+ this.endpoints + i));
			}
		}
	}

	@Override
	protected void consume(Long[] batch) {
		System.out.println("FeedChannel Remove " + batch.length + " objects");
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
			for (int category : categories) {
				prx[i].removeObjects(category, sepLongBatch);
			}
		}
	}

}
