package xce.util.ng.channel;

import java.util.concurrent.ConcurrentHashMap;

import xce.util.ng.channel.impl.AdChannelMC;
import xce.util.ng.channel.impl.FeedChannelMC;
import xce.util.ng.channel.impl.NotifyChannelMC;
import xce.util.ng.channel.impl.OceChannelMC;
import xce.util.ng.channel.impl.PassportChannelMC;
import xce.util.ng.channel.impl.XceFeedChannelMC;
import xce.util.ng.channel.impl.McProxyChannelMC;

public class ChannelFactory {
  public enum ChannelType {
    OCE, FEED, PASSPORT, CORECACHE, XCEFEED,NOTIFY,AD,MCPROXY
  }

  private static ConcurrentHashMap<ChannelType, ChannelIF> _cachedChannels = new ConcurrentHashMap<ChannelType, ChannelIF>();

  public static ChannelIF getChannel(ChannelType type) {
    ChannelIF channel = _cachedChannels.get(type);
    if (channel == null) {
      channel = createChannel(type);
      _cachedChannels.put(type, channel);
    }
    return channel;
  }

  private static ChannelIF createChannel(ChannelType type) {
    switch (type) {
    case OCE:
      return new OceChannelMC();
    case FEED:
      return new FeedChannelMC();
    case XCEFEED:
      return new XceFeedChannelMC();
    case NOTIFY:
        return new NotifyChannelMC();
    case AD:
        return new AdChannelMC();
    case PASSPORT:
      return new PassportChannelMC();
    case MCPROXY:
        return new McProxyChannelMC();
    case CORECACHE:
    default:
      assert (false);
    }
    assert (false);
    return null;
  }
}
