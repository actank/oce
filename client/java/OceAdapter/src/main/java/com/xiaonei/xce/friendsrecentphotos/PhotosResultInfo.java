package com.xiaonei.xce.friendsrecentphotos;

import java.util.ArrayList;
import java.util.List;

import xce.friendsphotos.PhotoResult;
import xce.friendsphotos.UserPhotos;

public class PhotosResultInfo {
  private int total = 0;
  private List<UserPhotosInfo> userPhotos = new ArrayList<UserPhotosInfo>();
  
  public PhotosResultInfo(final PhotoResult photoResult) {
    if (photoResult !=  null) {
      this.total = photoResult.total;
      for (UserPhotos userPhoto : photoResult.userPhotos) {
        this.userPhotos.add(new UserPhotosInfo(userPhoto));
      }
    }
  }
  
  public PhotosResultInfo(int total, final List<UserPhotosInfo> userPhotosInfo) {
    this.total = total;
    this.userPhotos = userPhotosInfo;
  }
  
  public int getTotal() {
    return total;
  }
  public void setTotal(int total) {
    this.total = total;
  }
  public List<UserPhotosInfo> getUserPhotos() {
    return userPhotos;
  }
  public void setUserPhotos(final List<UserPhotosInfo> userPhotos) {
    this.userPhotos = userPhotos;
  }
  
  public void addUserPhotos(final UserPhotosInfo userPhotoInfo) {
    if (userPhotoInfo != null) {
      this.userPhotos.add(new UserPhotosInfo(userPhotoInfo.getUserId(), userPhotoInfo.getPhotoInfos()));
    }
  }
  
  @Override
  public boolean equals(Object obj) {
    if (obj == null || !(obj instanceof PhotosResultInfo)) {
      return false;
    }
    PhotosResultInfo info = (PhotosResultInfo)obj;
    if (this.total != info.total || this.userPhotos.size() != info.getUserPhotos().size()) {
      return false;
    }
    for (int i = 0; i < this.userPhotos.size(); ++i) {
      if (!this.userPhotos.get(i).equals(info.getUserPhotos().get(i))) {
        return false;
      }
    }
    return true;
  }
}
