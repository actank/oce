package xce.util.ng.channel;

public interface ChannelIF {
  public enum Invoke {
    ONEWAY, TWOWAY
  }

  public Ice.Communicator getCommunicator();

  public Ice.ObjectPrx stringToProxy(String str);
}
