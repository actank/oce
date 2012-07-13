package xce.util.ng.channel;

import java.util.Iterator;

import xce.util.ng.tools.Replicator;

public abstract class AbstractMultiCommunicatorChannel extends AbstractChannel {
  protected AbstractMultiCommunicatorChannel(int limits) {
    assert (limits > 0);
    _limits = limits;
    _communicators = new Replicator<Ice.Communicator>(new CommunictorFactory(
        this), new Ice.Communicator[limits]);
    _it = _communicators.iterator();
  }

  protected final int _limits;

  public int getLimits() {
    return _limits;
  }

  protected final Replicator<Ice.Communicator> _communicators;
  protected final Iterator<Ice.Communicator> _it;

  /**
   * @return null if no communicator found.
   */
  @Override
  public synchronized Ice.Communicator getCommunicator() {
    return _it.next();
  }
}
