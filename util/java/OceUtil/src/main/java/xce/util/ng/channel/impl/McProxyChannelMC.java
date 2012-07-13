package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class McProxyChannelMC extends AbstractMultiCommunicatorChannel {

  public McProxyChannelMC() {
    super(30);
  }

  @Override
  protected void initLocator() {
    configure("IceGrid.InstanceName", "McProxy");
    configure("Ice.Default.Locator",
        "McProxy/Locator:default -h McProxyRegistry -p 14700");
  }

}
