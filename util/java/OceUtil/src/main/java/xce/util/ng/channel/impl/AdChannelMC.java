package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class AdChannelMC extends AbstractMultiCommunicatorChannel {

	public AdChannelMC() {
		super(30);
	}

	@Override
	protected void initLocator() {
		configure("IceGrid.InstanceName", "Ad");
		configure(
				"Ice.Default.Locator",
				"Ad/Locator:default -h AdRegistry -p 14800:default -h AdRegistryReplica1 -p 14801:default -h AdRegistryReplica2 -p 14802");
	}

}
