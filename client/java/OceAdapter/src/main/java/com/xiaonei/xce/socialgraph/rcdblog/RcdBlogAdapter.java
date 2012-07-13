package com.xiaonei.xce.socialgraph.rcdblog;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.RcdMessageServicePrx;
import xce.socialgraph.RcdMessageServicePrxHelper;
import xce.util.channel.Channel;

import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
 

/*
 * 功能：推荐中心聚合页热门blog推荐缓存更新接口
 * 作者：熊君武
 * 时间：2011-12-09
 * 邮件：junwu.xiong@renren-inc.com
 * */


public class RcdBlogAdapter extends ReplicatedClusterAdapter {
  private static RcdBlogAdapter instance = null;
  private static final String biz = "RCDBLOG";
  private static final String ctrEndpoints = "ControllerSocialGraphRcdMessageService";
  private static int interval = 120;
  
  private RcdBlogAdapter() {
	  super(ctrEndpoints, interval, Channel.newSocialGraphChannel(ctrEndpoints));
  }
  
  public static RcdBlogAdapter getInstance() {
	  if (null == instance) {
		  synchronized (RcdBlogAdapter.class) {
			  if (null == instance) {
				  try { 
					  instance = new RcdBlogAdapter();					  
				  } catch(Exception e) {
					  e.printStackTrace();
					  instance = null;
				  }
			  }
			  
		  }
	  }
	  return instance;
  }
  
  public List<RcdBlogData> getRcdBlog(int hostId, int limit) {
	  List<RcdBlogData> result = new ArrayList<RcdBlogData>();
	  byte[] data = BusinessCacheAdapter.getInstance().get(biz, hostId);
	  try { 
		  
		  int count = 0;
		  Items pbResult = Items.parseFrom(data);
		  if (pbResult.getItemsCount() < limit || limit < 0) {
			  count = pbResult.getItemsCount();
		  } else {
			  count = limit;
		  }
		  for (Item item : pbResult.getItemsList()) {
			  if (--count < 0) {
				  break;
			  }
			  result.add(new RcdBlogData(item));
		  }
	  } catch (InvalidProtocolBufferException e) {
		  e.printStackTrace();
	  }
	  return result;
  }
  	
  //删除推荐的hotblog：首先删除cache,然后将删除的数据放入到block表中 
	public void removeBizCache(long source, long block) {
		getRcdMessageServicePrxOneWay(source).removeBizCache(source, block, biz);  //删除对应的业务cache数据
		getRcdMessageServicePrxOneWay(source).addSgBlock(source, block, biz);	//加入socialgraph block表
	}	
	
	  //获取oneway代理
	private RcdMessageServicePrx getRcdMessageServicePrxOneWay(long source) {
		Ice.ObjectPrx prx0 = getValidProxy((int)source, 300);
		if (null != prx0) {
			RcdMessageServicePrx prx = RcdMessageServicePrxHelper.uncheckedCast(
					prx0.ice_timeout(300).ice_oneway());
		  	return prx;
		}
		return null;
	}
 
  
  //获取twoway代理
  private RcdMessageServicePrx getRcdMessageServicePrxTwoWay(long source) {
	 Ice.ObjectPrx prx0 = getValidProxy((int)source, 300);
	 if (null != prx0) {
		 RcdMessageServicePrx prx = RcdMessageServicePrxHelper.uncheckedCast(
				 prx0.ice_timeout(300).ice_twoway());
		 return prx;
	 }
	 return null;
  }
	
  public static void main(String[] args) {
	  //int hostId = 2147463835;
	  int hostId = 99;
//	  int blockId = -2079367624;
//	  RcdBlogAdapter.getInstance().removeBizCache(hostId,blockId);
	  List<RcdBlogData> result = RcdBlogAdapter.getInstance().getRcdBlog(hostId, -1);
	  for (RcdBlogData data : result) {
		  System.out.println(data.getBlogId() + " : " + data.getShareId() + " : " + data.getShareUserId() 
				  + " : " + data.getShareCount());
	  }
	 System.out.println("result size: " + result.size());
	 System.exit(0);
  }
}
