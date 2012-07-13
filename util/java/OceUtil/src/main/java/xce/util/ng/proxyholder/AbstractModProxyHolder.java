package xce.util.ng.proxyholder;

import xce.util.ng.channel.ChannelIF;

public abstract class AbstractModProxyHolder<KEY_T, PRX_T extends Ice.ObjectPrx>
    extends AbstractProxyHolder<PRX_T> {

  private final Ice.ObjectPrx[] _proxies;
  private final int _size;

  public AbstractModProxyHolder(ChannelIF channel, String namePrefix, int size) {
    super(channel);
    _size = size;
    _proxies = new Ice.ObjectPrx[size];
    for (int i = 0; i < size; ++i) {
      _proxies[i] = initProxy(channel.stringToProxy(namePrefix + i));
    }
  }

  public PRX_T getProxy(KEY_T key) {
    return uncheckedCast(_proxies[Math.abs(mod(key, _size))]);
  }

  protected abstract int mod(KEY_T key, int size);
}
