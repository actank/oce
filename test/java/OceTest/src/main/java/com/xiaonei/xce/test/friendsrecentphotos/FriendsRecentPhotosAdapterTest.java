package com.xiaonei.xce.test.friendsrecentphotos;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

import com.xiaonei.xce.friendsrecentphotos.FriendsRecentPhotosAdapter;
import com.xiaonei.xce.friendsrecentphotos.PhotosResultInfo;
import com.xiaonei.xce.friendsrecentphotos.UserPhotosInfo;


public class FriendsRecentPhotosAdapterTest {
  
  private List<UserPhotosInfo> formUserPhotosInfoList(final Integer[] userIds,
                                                      final Integer[][] uploadTimes,
                                                      final Long[][] photoIds) {
    List<UserPhotosInfo> result = new ArrayList<UserPhotosInfo>();
    for (int i = 0; i < userIds.length; ++i) {
      result.add(formUserPhotosInfo(userIds[i], uploadTimes[i], photoIds[i]));
    }
    return result;
  }
  
  private UserPhotosInfo formUserPhotosInfo(Integer userId, Integer[] uploadTimes, Long[] photoIds) {
    UserPhotosInfo result = new UserPhotosInfo(userId.intValue(), fromArrayToList(uploadTimes), fromArrayToList(photoIds));
    return result;
  }
  
  private <T> List<T> fromArrayToList(final T[] nums) {
    List<T> result = new ArrayList<T>();
    for (T t : nums) {
      result.add(t);
    }
    return result;
  }
  /***
   * 测试reloadUserPhotos, 测试之前用./FriendsPhotosDBOperator insertUserPhotosInfo向数据库中写入数据：
   * yes
   * 67913
   * 5 5 4 4 3 3 2 2 1 1 0 0
   * yes
   * 67813
   * 10 10 9 9 8 8 0 0
   * 36704
   * 100 100 99 99 95 95 89 89 0 0
   * yes
   * 45604
   * 21 21 0 0
   * yes
   * 42304
   * 1000 1000 22 22 0 0
   * yes
   * 33499
   * 79 79 45 45 0 0
   */
  @Test
  public void test1() {
    Integer[] userIds = new Integer[] {67913, 67813, 36704, 45604, 42304, 33499};
    Integer[][] uploadTimes = new Integer[][]{{5, 4, 3, 2, 1}, 
                                              {10, 9, 8}, 
                                              {100, 99, 95, 89}, 
                                              {21}, 
                                              {1000, 22}, 
                                              {79, 45}};
    Long[][] photoIds = new Long[][]{{new Long(5), new Long(4), new Long(3), new Long(2), new Long(1)}, 
                                     {new Long(10), new Long(9), new Long(8)}, 
                                     {new Long(100), new Long(99), new Long(95), new Long(89)}, 
                                     {new Long(21)}, 
                                     {new Long(1000), new Long(22)}, 
                                     {new Long(79), new Long(45)}};
    //FriendsRecentPhotosAdapter.getInstance().reloadUserPhotos(fromArrayToList(userIds));
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    for (int i = 0; i < userIds.length; ++i) {
      System.out.println("check for userId = " + userIds[i]);
      UserPhotosInfo tempUserPhotosInfo = formUserPhotosInfo(userIds[i], uploadTimes[i], photoIds[i]);
      UserPhotosInfo userPhotosInfoFromCache = FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(userIds[i].intValue());
      assert(tempUserPhotosInfo.equals(userPhotosInfoFromCache));
    }
  }
  
  /***
   * 测试deleteUserPhotos
   */
  @Test
  public void test2() {
    Integer[] userIds = new Integer[] {67913, 67813, 36704, 42304, 33499};
    Integer[][] uploadTimes = new Integer[][]{{4, 2, 1}, 
                                              {9}, 
                                              {100, 99, 95, 89}, 
                                              {22}, 
                                              {79}};
    Long[][] photoIds = new Long[][]{{new Long(4), new Long(2), new Long(1)}, 
                                     {new Long(9)}, 
                                     {new Long(100), new Long(99), new Long(95), new Long(89)}, 
                                     {new Long(22)}, 
                                     {new Long(79)}};
    List<UserPhotosInfo> userPhotosInfoToDelete = new ArrayList<UserPhotosInfo>();
    List<Integer> userIdsToReload = new ArrayList<Integer>();
    for (int i = 0; i < userIds.length; ++i) {
      userIdsToReload.add(userIds[i]);
      UserPhotosInfo tempUserPhotosInfo = formUserPhotosInfo(userIds[i], uploadTimes[i], photoIds[i]);
      userPhotosInfoToDelete.add(tempUserPhotosInfo);
    }
    FriendsRecentPhotosAdapter.getInstance().deleteUserPhotos(userPhotosInfoToDelete);
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    Integer[] userIdsAfterDelete = new Integer[] {67913, 67813, 36704, 42304, 33499};
    Integer[][] uploadTimesAfterDelete = new Integer[][]{{5, 3}, 
                                                         {10, 8}, 
                                                         {}, 
                                                         {1000},
                                                         {45}};
    Long[][] photoIdsAfterDelete = new Long[][]{{new Long(5), new Long(3)}, 
                                                {new Long(10), new Long(8)}, 
                                                {}, 
                                                {new Long(1000)},
                                                {new Long(45)}};
    for (int i = 0; i < userIdsAfterDelete.length; ++i) {
      UserPhotosInfo userPhotosInfoAfterDelete = 
        formUserPhotosInfo(userIdsAfterDelete[i], uploadTimesAfterDelete[i], photoIdsAfterDelete[i]);
      assert(userPhotosInfoAfterDelete.equals(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(userIdsAfterDelete[i])));
    }
    
    //reload一次，再次比较，检查数据库有没有正确修改
    FriendsRecentPhotosAdapter.getInstance().reloadUserPhotos(userIdsToReload);
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    for (int i = 0; i < userIdsAfterDelete.length; ++i) {
      UserPhotosInfo userPhotosInfoAfterDelete = 
        formUserPhotosInfo(userIdsAfterDelete[i], uploadTimesAfterDelete[i], photoIdsAfterDelete[i]);
      assert(userPhotosInfoAfterDelete.equals(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(userIdsAfterDelete[i])));
    }
  }
  
  /***
   * 测试addUserPhotosInfo
   */
  @Test
  public void test3() {
    Integer[] userIds = new Integer[] {67913, 67813, 36704, 42304, 33499};
    Integer[][] uploadTimes = new Integer[][]{{21, 11, 4}, 
                                              {34, 3},
                                              {67, 53, 29}, 
                                              {18},
                                              {47, 35}};
    Long[][] photoIds = new Long[][]{{new Long(21), new Long(11), new Long(4)},
                                     {new Long(34), new Long(3)}, 
                                     {new Long(67), new Long(53), new Long(29)}, 
                                     {new Long(18)}, 
                                     {new Long(47), new Long(35)}};
    List<UserPhotosInfo> userPhotosInfoToAdd = new ArrayList<UserPhotosInfo>();
    List<Integer> userIdsToReload = new ArrayList<Integer>();
    for (int i = 0; i < userIds.length; ++i) {
      userIdsToReload.add(userIds[i]);
      UserPhotosInfo tempUserPhotosInfo = formUserPhotosInfo(userIds[i], uploadTimes[i], photoIds[i]);
      userPhotosInfoToAdd.add(tempUserPhotosInfo);
    }
    FriendsRecentPhotosAdapter.getInstance().addUserPhotos(userPhotosInfoToAdd);
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    Integer[] userIdsAfterAdd = new Integer[] {67913, 67813, 36704, 42304, 33499};
    Integer[][] uploadTimesAfterAdd = new Integer[][]{{21, 11, 5, 4, 3}, 
                                                      {34, 10, 8, 3}, 
                                                      {67, 53, 29}, 
                                                      {1000, 18},
                                                      {47, 45, 35}};
    Long[][] photoIdsAfterAdd = new Long[][]{{new Long(21), new Long(11), new Long(5), new Long(4), new Long(3)},
                                                {new Long(34), new Long(10), new Long(8),  new Long(3)}, 
                                                {new Long(67), new Long(53), new Long(29)}, 
                                                {new Long(1000), new Long(18)}, 
                                                {new Long(47), new Long(45), new Long(35)}};
    for (int i = 0; i < userIdsAfterAdd.length; ++i) {
      UserPhotosInfo userPhotosInfoAfterAdd = 
        formUserPhotosInfo(userIdsAfterAdd[i], uploadTimesAfterAdd[i], photoIdsAfterAdd[i]);
      assert(userPhotosInfoAfterAdd.equals(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(userIdsAfterAdd[i])));
    }
    
    //reload一次，再次比较，检查数据库有没有正确修改
    FriendsRecentPhotosAdapter.getInstance().reloadUserPhotos(userIdsToReload);
    
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    for (int i = 0; i < userIdsAfterAdd.length; ++i) {
      UserPhotosInfo userPhotosInfoAfterDelete = 
        formUserPhotosInfo(userIdsAfterAdd[i], uploadTimesAfterAdd[i], photoIdsAfterAdd[i]);
      assert(userPhotosInfoAfterDelete.equals(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(userIdsAfterAdd[i])));
    }
  }
  
  /***
   * 测试getFriendsRecentPhotos, 将系统中所有的人互相都当成好友
   */
  //@Test
  public void test4() {
    PhotosResultInfo resultFromCache = FriendsRecentPhotosAdapter.getInstance().getFriendsRecentPhotos(67913, 0, 3);
    Integer[] userIds = new Integer[]{42304, 36704};
    Integer[][] uploadTimes = new Integer[][]{{1000}, 
                                              {67, 53}};
    Long[][] photoIds = new Long[][] {{new Long(1000)},
                                      {new Long(67), new Long(53)}};
    List<UserPhotosInfo> userPhotosInfoList = formUserPhotosInfoList(userIds, uploadTimes, photoIds);
    PhotosResultInfo resultToCompare = new PhotosResultInfo(12, userPhotosInfoList);
    assert(resultToCompare.equals(resultFromCache));
    
    PhotosResultInfo emptyResultFromCache = FriendsRecentPhotosAdapter.getInstance().getFriendsRecentPhotos(67913, -1, 3);
    assert(emptyResultFromCache.getTotal() == 0);
  }
  
  /***
   * 继续测试getFriendsRecentPhotos, 测试limit的边界条件
   */
  //@Test
  public void test5() {
    PhotosResultInfo resultFromCache = FriendsRecentPhotosAdapter.getInstance().getFriendsRecentPhotos(67913, 3, 5);
    Integer[] userIds = new Integer[]{33499, 67813, 36704};
    Integer[][] uploadTimes = new Integer[][]{{47, 45, 35}, 
                                              {34},
                                              {29}};
    Long[][] photoIds = new Long[][] {{new Long(47), new Long(45), new Long(35)},
                                      {new Long(34)},
                                      {new Long(29)}};
    List<UserPhotosInfo> userPhotosInfoList = formUserPhotosInfoList(userIds, uploadTimes, photoIds);
    PhotosResultInfo resultToCompare = new PhotosResultInfo(12, userPhotosInfoList);
    assert(resultToCompare.equals(resultFromCache));
    
    PhotosResultInfo resultNew = FriendsRecentPhotosAdapter.getInstance().getFriendsRecentPhotos(67913, 7, 10);
    Integer[] userIdsNew = new Integer[]{36704, 42304, 67813};
    Integer[][] uploadTimesNew = new Integer[][]{{29}, 
                                                 {18},
                                                 {10, 8, 3}};
    Long[][] photoIdsNew = new Long[][] {{new Long(29)},
                                         {new Long(18)},
                                         {new Long(10), new Long(8), new Long(3)}};
    List<UserPhotosInfo> userPhotosInfoListNew = formUserPhotosInfoList(userIdsNew, uploadTimesNew, photoIdsNew);
    PhotosResultInfo resultToCompareNew = new PhotosResultInfo(12, userPhotosInfoListNew);
    assert(resultNew.equals(resultToCompareNew));
  }
  
  //@Test
  public void test6() {
    Integer[] userIds = new Integer[] {67913, 67813, 36704, 45604, 42304, 33499};
    Integer[][] uploadTimes = new Integer[][]{{5, 4, 3, 2, 1}, 
                                              {10, 9, 8}, 
                                              {100, 99, 95, 89}, 
                                              {21}, 
                                              {1000, 22}, 
                                              {79, 45}};
    Long[][] photoIds = new Long[][]{{new Long(5), new Long(4), new Long(3), new Long(2), new Long(1)}, 
                                     {new Long(10), new Long(9), new Long(8)}, 
                                     {new Long(100), new Long(99), new Long(95), new Long(89)}, 
                                     {new Long(21)}, 
                                     {new Long(1000), new Long(22)}, 
                                     {new Long(79), new Long(45)}};
    //FriendsRecentPhotosAdapter.getInstance().reloadUserPhotos(fromArrayToList(userIds));
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    List<UserPhotosInfo> userPhotosInfoToAdd = new ArrayList<UserPhotosInfo>();
    for (int i = 0; i < userIds.length; ++i) {
      userPhotosInfoToAdd.add(formUserPhotosInfo(userIds[i], uploadTimes[i], photoIds[i]));
    }
    FriendsRecentPhotosAdapter.getInstance().addUserPhotos(userPhotosInfoToAdd);
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    
    for (UserPhotosInfo info : userPhotosInfoToAdd) {
      assert(info.equals(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(info.getUserId())));
    }
    FriendsRecentPhotosAdapter.getInstance().deleteUserPhotos(userPhotosInfoToAdd);
  }
  public static void main(String args[]) {
    System.out.println(FriendsRecentPhotosAdapter.getInstance().getUserRecentPhotos(67913));
  }
}
