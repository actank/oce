package xce.util.ng.channel;

public abstract class AbstractChannel implements ChannelIF {
  protected AbstractChannel() {
    initDefault();
    initLocator();
    initOverride();
  }

  protected Ice.Properties _properties = Ice.Util.createProperties();

  protected void configure(String key, String value) {
    _properties.setProperty(key, value);
  }

  public Ice.Properties getProperties() {
    return _properties;
  }

  protected void initDefault() {
    configure("Ice.Override.ConnectTimeout", "70");
    configure("Ice.ThreadPool.Client.Size", "1");
    configure("Ice.ThreadPool.Client.SizeMax", "1000");
    configure("Ice.ThreadPool.Client.StackSize", "131072");
    configure("Ice.ThreadPool.Server.Size", "1");
    configure("Ice.ThreadPool.Server.SizeMax", "1000");
    configure("Ice.ThreadPool.Server.StackSize", "131072");
    configure("Ice.MessageSizeMax", "102400");
    // configure("Ice.Trace.Protocol", "3");
    // configure("Ice.Trace.Network", "3");
  }

  protected abstract void initLocator();

  protected void initOverride() {
    if (System.getProperty("__NO_TIMEOUT__") != null) {
      configure("Ice.Override.Timeout", "-1");
      configure("Ice.Override.ConnectTimeout", "-1");
    } else if (System.getProperty("__LONG_TIMEOUT__") != null) {
      configure("Ice.Override.Timeout", "30000");
      configure("Ice.Override.ConnectTimeout", "7000");
    }
  }

  @Override
  public Ice.ObjectPrx stringToProxy(String str) {
    return getCommunicator().stringToProxy(str);
  }
}
