package xce.util.ng.channel.impl;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;

public class PassportChannelMC extends AbstractMultiCommunicatorChannel {

  public PassportChannelMC() {
    super(30);
  }

  @Override
  protected void initLocator() {
    configure("IceGrid.InstanceName", "Passport");
    configure("Ice.Default.Locator",
        "Passport/Locator:default -h PassportMaster -p 4076:"
            + "default -h PassportReplica1 -p 4076:"
            + "default -h PassportReplica2 -p 4076");
  }

  @Override
  protected void initOverride() {
    configure("Ice.ThreadPool.Client.Size", "100");
    super.initOverride();
  }
}
