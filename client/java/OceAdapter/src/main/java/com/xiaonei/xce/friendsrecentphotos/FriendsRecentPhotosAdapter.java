package com.xiaonei.xce.friendsrecentphotos;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ClientInterface;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.friendsphotos.FriendsRecentPhotosManagerPrx;
import xce.friendsphotos.FriendsRecentPhotosManagerPrxHelper;
import xce.friendsphotos.FriendsRecentPhotosWriterPrx;
import xce.friendsphotos.FriendsRecentPhotosWriterPrxHelper;
import xce.friendsphotos.Photo;
import xce.friendsphotos.PhotoResult;
import xce.friendsphotos.UserPhotos;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;

public class FriendsRecentPhotosAdapter extends ReplicatedClusterAdapter {
  private FriendsRecentPhotosAdapter() {
    super(friendsRecentPhotosManagerEndpoints, interval);
  }

  private static final String friendsRecentPhotosManagerEndpoints = "ControllerFriendsRecentPhotosManager";
  private static final String friendsRecentPhotosWriterEndpoints = "ControllerFriendsRecentPhotosWriter";
  private static int interval = 120;

  private static FriendsRecentPhotosAdapter _instance = new FriendsRecentPhotosAdapter();
  
  private static ClientInterface _friendsRecentPhotosWriterClient;
  
  static {
    try{
      _friendsRecentPhotosWriterClient = new ClientInterface(friendsRecentPhotosWriterEndpoints,
          interval);
    }catch(Throwable e){
      e.printStackTrace();
    }
  }

  public static FriendsRecentPhotosAdapter getInstance() {
    return _instance;
  }
  
  private UserPhotos[] toUserPhotosArray(final List<UserPhotosInfo> userPhotosInfo) {
    UserPhotos[] result = new UserPhotos[userPhotosInfo.size()];
    for (int i = 0; i < userPhotosInfo.size(); ++i) {
      UserPhotos userPhotos = new UserPhotos();
      userPhotos.userId = userPhotosInfo.get(i).getUserId();
      List<PhotoInfo> photoInfos = userPhotosInfo.get(i).getPhotoInfos();
      userPhotos.photos = new Photo[photoInfos.size()];
      for (int j = 0; j < userPhotos.photos.length; ++j) {
        userPhotos.photos[j] = new Photo(photoInfos.get(j).getUploadTime(), photoInfos.get(j).getPhotoId());
      }
      result[i] = userPhotos;
    }
    return result;
  }
  
  private int[] intListToArray(final List<Integer> nums) {
    int[] result = new int[nums.size()];
    for (int i = 0; i < nums.size(); ++i) {
      result[i] = nums.get(i).intValue();
    }
    return result;
  }
  
  public PhotosResultInfo getFriendsRecentPhotos(int userId, int begin, int limit) {
    FriendsRecentPhotosManagerPrx prx = null;
    PhotoResult result = null;
    try {
      prx = getFriendsRecentPhotosManagerPrx();
      result = prx.getFriendsRecentPhotos(userId, begin, limit);
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getFriendsRecentPhotos [" + prx
              + "] userId = " + userId + " begin = " + begin + " limit = " + limit + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getFriendsRecentPhotos [" + prx
              + "] userId = " + userId + " begin = " + begin + " limit = " + limit + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getFriendsRecentPhotos [" + prx
              + "] userId = " + userId + " begin = " + begin + " limit = " + limit + " " + e);
    }
    return new PhotosResultInfo(result);
  }
  
  public UserPhotosInfo getUserRecentPhotos(int userId) {
    FriendsRecentPhotosManagerPrx prx = null;
    UserPhotosInfo result = null;
    try {
      prx = getFriendsRecentPhotosManagerPrx();
      UserPhotos rawData = prx.getUserRecentPhotos(userId);
      if (rawData.userId == -1) {
        rawData.userId = userId;
      }
      result = new UserPhotosInfo(rawData);
//      System.out.println("The result of userId = " + userId + " is " + result.toString()
//          + " in getUserRecentPhotos");
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserRecentPhotos [" + prx
              + "] userId = " + userId + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserRecentPhotos [" + prx
              + "] userId = " + userId + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserRecentPhotos [" + prx
              + "] userId = " + userId + " " + e);
    }
    return result;
  }
  
  public void addUserPhotos(final List<UserPhotosInfo> userPhotosInfo) {
    FriendsRecentPhotosWriterPrx prx = null;
    try {
      prx = getFriendsRecentPhotosWriterPrx();
      prx.addRecentPhotos(toUserPhotosArray(userPhotosInfo));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addUserPhotos [" + prx
              + "] size  = " + userPhotosInfo.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addUserPhotos [" + prx
              + "] size = " + userPhotosInfo.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addUserPhotos [" + prx
              + "] size= " + userPhotosInfo.size() + " " + e);
    }
  }
  
  public void deleteUserPhotos(final List<UserPhotosInfo> userPhotosInfo) {
    FriendsRecentPhotosWriterPrx prx = null;
    try {
      prx = getFriendsRecentPhotosWriterPrx();
      prx.deleteUserPhotos(toUserPhotosArray(userPhotosInfo));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".deleteUserPhotos [" + prx
              + "] size  = " + userPhotosInfo.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".deleteUserPhotos [" + prx
              + "] size = " + userPhotosInfo.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".deleteUserPhotos [" + prx
              + "] size= " + userPhotosInfo.size() + " " + e);
    }
  }
  
  public void reloadUserPhotos(final List<Integer> userIds) {
    FriendsRecentPhotosWriterPrx prx = null;
    try {
      prx = getFriendsRecentPhotosWriterPrx();
      prx.reloadUserPhotos(intListToArray(userIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUserPhotos [" + prx
              + "] size  = " + userIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUserPhotos [" + prx
              + "] size = " + userIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUserPhotos [" + prx
              + "] size= " + userIds.size() + " " + e);
    }
  }
  
  protected FriendsRecentPhotosManagerPrx getFriendsRecentPhotosManagerPrx() {
    Ice.ObjectPrx prx0 = getValidProxy(0, "FRPM");
    if (prx0 != null) {
      FriendsRecentPhotosManagerPrx prx = FriendsRecentPhotosManagerPrxHelper
          .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }
  
  protected FriendsRecentPhotosWriterPrx getFriendsRecentPhotosWriterPrx() {
    Ice.ObjectPrx prx0 = _friendsRecentPhotosWriterClient.getValidProxy(0);
    if (prx0 != null) {
      FriendsRecentPhotosWriterPrx prx = FriendsRecentPhotosWriterPrxHelper
          .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }
  
  public static void main(String args[]) {
    List<Integer> userIds = new ArrayList<Integer>();
    userIds.add(new Integer(67913));
    try {
      Thread.sleep(4000);
    } catch (InterruptedException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
    FriendsRecentPhotosAdapter.getInstance().reloadUserPhotos(userIds);
    List<UserPhotosInfo> userPhotosInfo = new ArrayList<UserPhotosInfo>();
    List<Integer> uploadTimes = new ArrayList<Integer>();
    System.out.println(Integer.MAX_VALUE);
    uploadTimes.add(new Integer(1320921989));
    uploadTimes.add(new Integer(123414));
    List<Long> photoIds = new ArrayList<Long>();
    photoIds.add(new Long(232342342342l));
    photoIds.add(new Long(23423424322l));
    UserPhotosInfo userPhoto = new UserPhotosInfo(227198911, uploadTimes, photoIds);
    userPhotosInfo.add(userPhoto);
    FriendsRecentPhotosAdapter.getInstance().addUserPhotos(userPhotosInfo);
    try {
      Thread.sleep(4000);
    } catch (InterruptedException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
    System.out.println(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(227198911));
    System.exit(0);
  }
}
