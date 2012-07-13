package xce.util.ng.proxyholder;

import java.util.Iterator;

import xce.util.ng.channel.AbstractMultiCommunicatorChannel;
import xce.util.ng.channel.ChannelIF;
import xce.util.ng.tools.Replicator;
import xce.util.ng.tools.ReplicatorFactory;

public abstract class AbstractRandomProxyHolder<T extends Ice.ObjectPrx>
    extends AbstractProxyHolder<T> {

  private String _name;
  protected final Replicator<Ice.ObjectPrx> _proxies;
  protected final Iterator<Ice.ObjectPrx> _it;

  public AbstractRandomProxyHolder(ChannelIF channel, String name) {
    super(channel);
    _name = name;
    Ice.ObjectPrx[] cachedProxies = new Ice.ObjectPrx[_channel instanceof AbstractMultiCommunicatorChannel ? ((AbstractMultiCommunicatorChannel) channel)
        .getLimits()
        : 30];
    _proxies = new Replicator<Ice.ObjectPrx>(
        new ReplicatorFactory<Ice.ObjectPrx>() {
          @Override
          public Ice.ObjectPrx create() {
            return initProxy(_channel.getCommunicator().stringToProxy(_name));
          }
        }, cachedProxies);
    _it = _proxies.iterator();
  }

  public T getProxy() {
    return uncheckedCast(_it.next());
  }
}
