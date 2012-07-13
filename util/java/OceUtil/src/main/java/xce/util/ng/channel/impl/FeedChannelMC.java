package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class FeedChannelMC extends AbstractMultiCommunicatorChannel {

  public FeedChannelMC() {
    super(30);
  }

  @Override
  protected void initLocator() {
    configure("IceGrid.InstanceName", "XiaoNei");
    configure("Ice.Default.Locator",
        "XiaoNei/Locator:default -h XceFeedMaster -p 12000"
            + ":default -h XceFeedReplica1 -p 12001"
            + ":default -h XceFeedReplica2 -p 12002");
  }

}
