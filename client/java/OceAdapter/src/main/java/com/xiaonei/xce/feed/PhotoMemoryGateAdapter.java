package com.xiaonei.xce.feed;
import xce.feed.PhotoMemoryGatePrx;
import xce.feed.PhotoMemoryGatePrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
public class PhotoMemoryGateAdapter extends 
  AbstractModProxyHolder<Integer, PhotoMemoryGatePrx> {
  private static PhotoMemoryGateAdapter instance;
  private static String endpoints = "@PhotoMemoryGate";

  public PhotoMemoryGateAdapter() {
    super(ChannelFactory.getChannel(ChannelType.XCEFEED),
        "M"+endpoints, 5);
  }
    public static PhotoMemoryGateAdapter getInstance() {
    if (instance == null) {
      instance = new PhotoMemoryGateAdapter();
    }
    return instance;
  }

  @Override
  protected int mod(Integer key, int size) {
    return key % size;
  }

  @Override
  public PhotoMemoryGatePrx uncheckedCast(ObjectPrx baseProxy) {
    return PhotoMemoryGatePrxHelper.uncheckedCast(baseProxy);
  }

  @Override
  protected ObjectPrx initProxy(ObjectPrx baseProxy) {
    return baseProxy;
  }

  private PhotoMemoryGatePrx getManagerOneway(int uid) {
    return uncheckedCast(getProxy(uid).ice_oneway());
  }

  private PhotoMemoryGatePrx getManager(int uid) {
    return getProxy(uid);
  }

  public String GetPhotos(int userid, int begin, int limit) {
    return getManager(userid).GetPhotos(userid,begin, limit);
  }

  public String GeFriendPhotos(int userid, int friendid, int begin, int limit) {
    return getManager(userid).GetFriendPhotos(userid,friendid, begin, limit);
  }
}
