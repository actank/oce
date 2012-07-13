package xce.util.ng.proxyholder;

import xce.util.ng.channel.ChannelIF;

abstract class AbstractProxyHolder<T extends Ice.ObjectPrx> implements
    ProxyHolder<T> {
  protected final ChannelIF _channel;

  public AbstractProxyHolder(ChannelIF channel) {
    _channel = channel;
  }

  protected abstract Ice.ObjectPrx initProxy(Ice.ObjectPrx baseProxy);
}
