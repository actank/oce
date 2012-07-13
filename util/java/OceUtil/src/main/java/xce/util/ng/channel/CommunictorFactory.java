package xce.util.ng.channel;

import Ice.Communicator;
import xce.util.ng.tools.ReplicatorFactory;

public class CommunictorFactory implements ReplicatorFactory<Ice.Communicator> {

  private final AbstractChannel _channel;

  public CommunictorFactory(AbstractChannel channel) {
    _channel = channel;
  }

  @Override
  public Communicator create() {
    Ice.InitializationData initData = new Ice.InitializationData();
    initData.properties = _channel.getProperties();
    return Ice.Util.initialize(initData);
  }
}
