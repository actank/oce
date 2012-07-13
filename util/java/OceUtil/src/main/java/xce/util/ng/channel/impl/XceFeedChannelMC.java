package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class XceFeedChannelMC extends AbstractMultiCommunicatorChannel {

  public XceFeedChannelMC() {
    super(30);
  }

  @Override
  protected void initLocator() {
    configure("IceGrid.InstanceName", "XceFeed");
    configure("Ice.Default.Locator",
        "XceFeed/Locator:default -h XceFeedRegistry -p 14300"
            + ":default -h XceFeedRegistryReplica1 -p 14301"
            + ":default -h XceFeedRegistryReplica2 -p 14302");
  }

}
