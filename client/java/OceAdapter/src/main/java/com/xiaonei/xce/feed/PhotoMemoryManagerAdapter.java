package com.xiaonei.xce.feed;
import xce.feed.PhotoContent;
import java.util.ArrayList;
import java.util.List;
import xce.feed.PhotoMemoryManagerPrx;
import xce.feed.PhotoMemoryManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
public class PhotoMemoryManagerAdapter extends
  AbstractModProxyHolder<Integer, PhotoMemoryManagerPrx> {
  private static PhotoMemoryManagerAdapter instance;
  private static String endpoints = "@PhotoMemory";

  public PhotoMemoryManagerAdapter() {
    super(ChannelFactory.getChannel(ChannelType.XCEFEED),
        "M"+endpoints, 5);
  }
    public static PhotoMemoryManagerAdapter getInstance() {
    if (instance == null) {
      instance = new PhotoMemoryManagerAdapter();
    }
    return instance;
  }

  @Override
  protected int mod(Integer key, int size) {
    return key % size;
  }

  @Override
  public PhotoMemoryManagerPrx uncheckedCast(ObjectPrx baseProxy) {
    return PhotoMemoryManagerPrxHelper.uncheckedCast(baseProxy);
  }

  @Override
  protected ObjectPrx initProxy(ObjectPrx baseProxy) {
    return baseProxy;
  }

  private PhotoMemoryManagerPrx getManagerOneway(int uid) {
    return uncheckedCast(getProxy(uid).ice_oneway());
  }
  private PhotoMemoryManagerPrx getManager(int uid) {
    return getProxy(uid);
  }

  public void  SetRead(int userid, long photoid) {
    getManagerOneway(userid).SetRead(userid,photoid);
  }

  public List<PhotoContent> GetPhotoContentList(int userid,int begin, int end)
  {
    try{
      List<PhotoContent> r;
      ArrayList<PhotoContent> res = new ArrayList<PhotoContent>();

      PhotoContent[] seq =  getManager(userid).GetPhotoContentSeq(userid, begin,end); 
      for (int i = 0; i < seq.length; ++i) {
        res.add(seq[i]);
      }    
      r = res; 
      return r;
    }catch (Ice.TimeoutException e) { 
      throw e;
    }

  }  

  public void RemoveFriendPhotos(int userid, int friendid) {
    getManagerOneway(userid).RemoveFriendPhotos(userid,friendid);
  }
}

