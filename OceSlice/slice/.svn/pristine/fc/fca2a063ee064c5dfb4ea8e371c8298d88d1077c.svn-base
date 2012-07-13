#ifndef _FRIENDS_RECENT_PHOTOS_ICE
#define _FRIENDS_RECENT_PHOTOS_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module friendsphotos {
  class Photo {
    int uploadTime;
    long photoId;
  };
  
  sequence<Photo> PhotoSeq;

  //Cache中实际存放的数据结构
  class UserPhotos {
    int userId;
    PhotoSeq photos; //按照时间有序
  };

  sequence<UserPhotos> UserPhotosSeq;

  //返回给用户的结果，total表示用户的好友最近一共上传了多少张照片，用于用户翻页
  class PhotoResult {
    int total;
    UserPhotosSeq userPhotos;
  };

  dictionary<int, MyUtil::LongSeq> Int2LongSeqMap;

  class FriendsRecentPhotosManager {

    //获取好友最近上传的照片列表，按照时间有序,用于翻页
    PhotoResult getFriendsRecentPhotos(int userId, int begin, int limit); 
   
    //获取指定用户最近上传的照片，按照时间有序
    UserPhotos getUserRecentPhotos(int userId);

    //下面的接口只给FriendsRecentPhotoWriter使用，java adapter中不会暴露

    //批量添加用户最近上传的照片信息。如果用户原来没有要添加的相册，那么就添加；
    //如果原来有就把照片加到原来相册照片的前面，有可能会把原来相册的照片挤掉，因为每个相册的大小有限制
    void addRecentPhotos(UserPhotosSeq userPhotos);
    
    //批量删除用户上传的照片信息
    void deleteUserPhotos(UserPhotosSeq userPhotos);
    
    void setValid(bool valid);
    
    void setData(MyUtil::ObjectResult userPhotoData);
  };

  class FriendsRecentPhotosWriter {
    //以下5个接口会调用FriendsRecentAlbumManager的c++ adapter来完成相应的工作
    //批量用户最近上传的相册信息
    void addRecentPhotos(UserPhotosSeq userPhotos);

    //批量删除用户最近上传的相册信息
    void deleteUserPhotos(UserPhotosSeq userPhotos);
    
    //更新用户上传照片的相关信息, 在其中调用Manager的FriendsRecentAlbumManager的setData方法
    void reloadUserPhotos(MyUtil::IntSeq userIds);
  };
};
};

#endif
