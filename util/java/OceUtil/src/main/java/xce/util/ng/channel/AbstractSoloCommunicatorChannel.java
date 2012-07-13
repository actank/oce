package xce.util.ng.channel;

public abstract class AbstractSoloCommunicatorChannel extends AbstractChannel {

  protected Ice.Communicator _communictor;

  @Override
  public synchronized Ice.Communicator getCommunicator() {
    if (_communictor == null) {
      _communictor = new CommunictorFactory(this).create();
    }
    return _communictor;
  }

}
