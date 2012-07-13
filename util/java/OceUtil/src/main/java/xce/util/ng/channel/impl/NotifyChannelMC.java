package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class NotifyChannelMC extends AbstractMultiCommunicatorChannel {

	  public NotifyChannelMC() {
	    super(30);
	  }

	  @Override
	  protected void initLocator() {
	    configure("IceGrid.InstanceName", "Notify");
	    configure("Ice.Default.Locator",
	        "Notify/Locator:default -h NotifyRegistry -p 14400"
	            + ":default -h NotifyRegistryReplica1 -p 14401"
	            + ":default -h NotifyRegistryReplica2 -p 14402");
	  }
}
