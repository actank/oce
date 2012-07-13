package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class OceChannelMC extends AbstractMultiCommunicatorChannel {

  public OceChannelMC() {
    super(30);
  }

  @Override
  protected void initLocator() {
    configure("IceGrid.InstanceName", "XiaoNei");
    configure("Ice.Default.Locator",
        "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"
            + ":default -h XiaoNeiRegistryReplica1 -p 12001"
            + ":default -h XiaoNeiRegistryReplica2 -p 12002");
  }

}
