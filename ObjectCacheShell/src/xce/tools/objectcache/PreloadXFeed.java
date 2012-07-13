package xce.tools.objectcache;


import xce.util.channel.Channel;
import xce.xfeed.XFeedDispatcherPrx;
import xce.xfeed.XFeedDispatcherPrxHelper;

public class PreloadXFeed extends Consumer {
	private final int cluster;
	
	private final Ice.Communicator ic;
	private final XFeedDispatcherPrx prx[];
	
	public PreloadXFeed(Producer producer, int cluster){
		super(producer);
		ic = Channel.newFeedChannel("XFeedDispatcher").getCommunicator();
		if (cluster == 0) {
			this.cluster = 1;
			prx=new XFeedDispatcherPrxHelper[1];
			prx[0]=XFeedDispatcherPrxHelper.uncheckedCast(ic.stringToProxy("XFD@XFeedDispatcher"));
		}else {
			this.cluster = cluster;
			prx=new XFeedDispatcherPrxHelper[cluster];
			for (int i = 0; i < cluster; ++i) {
				prx[i]=XFeedDispatcherPrxHelper.uncheckedCast(ic.stringToProxy("XFD@XFeedDispatcher"+i));
			}
		}
	}
	
	@Override
	protected void consume(Long[] batch) {
		System.out.println("FeedChannel Preload " + batch.length + " objects");
		for (int i = 0; i < cluster; ++i) {
			prx[i].dispatchAgain(batch[0], batch[1]);
		}
	}
}
