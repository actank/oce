package com.xiaonei.xce.socialgraph.rcdhotpage;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.RcdMessageServicePrx;
import xce.socialgraph.RcdMessageServicePrxHelper;
import xce.util.channel.Channel;

import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.page.RcdPageData;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
 

/*
 * 功能：推荐中心聚合页热门page推荐缓存更新接口
 * 作者：熊君武
 * 时间：2011-12-06
 * 邮件：junwu.xiong@renren-inc.com
 * */


public class RcdHotPageAdapter extends ReplicatedClusterAdapter {
  private static RcdHotPageAdapter instance = null;
  private static final String biz = "HOTPAGE";
  private static final String ctrEndpoints = "ControllerSocialGraphRcdMessageService";
  private static int interval = 120;
  
  private RcdHotPageAdapter() {
	  super(ctrEndpoints, interval, Channel.newSocialGraphChannel(ctrEndpoints));
  }
  
  public static RcdHotPageAdapter getInstance() {
	  if (null == instance) {
		  synchronized (RcdHotPageAdapter.class) {
			  if (null == instance) {
				  try { 
					  instance = new RcdHotPageAdapter();					  
				  } catch(Exception e) {
					  e.printStackTrace();
					  instance = null;
				  }
			  }
			  
		  }
	  }
	  return instance;
  }
  
  public List<RcdPageData> getRcdHotPage(int hostId, int limit) {
	  List<RcdPageData> result = new ArrayList<RcdPageData>();
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
			  result.add(new RcdPageData(item));
		  }
	  } catch (InvalidProtocolBufferException e) {
		  e.printStackTrace();
	  }
	  return result;
  }
  	
  //删除推荐的hotpage：首先删除cache,然后将删除的数据放入到block表中 
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
	  int hostId = 2147000482;
	  int blockId = 600941521;
	  RcdHotPageAdapter.getInstance().removeBizCache(hostId,blockId);
	  List<RcdPageData> result = RcdHotPageAdapter.getInstance().getRcdHotPage(hostId, -1);
	  for (RcdPageData data : result) {
		  System.out.println(data.getPageId() + " : " + data.getFansNum());
	  }
	 System.out.println("result size: " + result.size());
	 System.exit(0);
  }
}
