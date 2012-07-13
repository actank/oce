package com.xiaonei.xce.fof.service;

public interface NorbertServerAdminMBean
{
  public int getId();
  public int getPort();
  public String getPartitions();
  boolean isAvailable();
  void setAvailable(boolean available);
}
