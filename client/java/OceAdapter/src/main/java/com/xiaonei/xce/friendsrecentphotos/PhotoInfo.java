package com.xiaonei.xce.friendsrecentphotos;

import xce.friendsphotos.Photo;

public class PhotoInfo {
  private int uploadTime = 0;
  private long photoId = 0;
  
  public PhotoInfo(int uploadTime, long photoId) {
    this.uploadTime = uploadTime;
    this.photoId = photoId;
  }
  
  public PhotoInfo(final Photo photo) {
    if (photo != null) {
      this.uploadTime = photo.uploadTime;
      this.photoId = photo.photoId;
    }
  }

  public int getUploadTime() {
    return uploadTime;
  }

  public void setUploadTime(int uploadTime) {
    this.uploadTime = uploadTime;
  }

  public long getPhotoId() {
    return photoId;
  }

  public void setPhotoId(long photoId) {
    this.photoId = photoId;
  }
  
  @Override
  public String toString() {
    return "[photoId = " + photoId + ", uploadTime = " + uploadTime + "]";
  }
}
