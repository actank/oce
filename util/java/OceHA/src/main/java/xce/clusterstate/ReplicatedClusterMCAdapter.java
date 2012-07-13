package xce.clusterstate;

import xce.util.channel.Channel;
import xce.util.ng.channel.ChannelIF;

public class ReplicatedClusterMCAdapter extends ReplicatedClusterAdapter{

	public ReplicatedClusterMCAdapter(ChannelIF channel, String endpoint,
			int interval) {
		super();
		_csClient = new ClientInterface(channel, endpoint, interval,Channel.newOceChannel(endpoint));
	}
	public ReplicatedClusterMCAdapter(ChannelIF channel, String endpoint,
			int interval,Channel c) {
		super();
		_csClient = new ClientInterface(channel, endpoint, interval,c);
	}

}
