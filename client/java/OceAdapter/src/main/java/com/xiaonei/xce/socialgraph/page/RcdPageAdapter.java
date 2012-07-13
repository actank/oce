package com.xiaonei.xce.socialgraph.page;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.RcdMessageServicePrx;
import xce.socialgraph.RcdMessageServicePrxHelper;
import xce.util.channel.Channel;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.common.RcdBlockBase;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 为page提供删除右侧推荐位page类型接口
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 * 修改：为推荐中心聚合页添加取cache数据接口
 */
public class RcdPageAdapter extends ReplicatedClusterAdapter implements
		RcdBlockBase {
	private static final String endpoints = "ControllerSocialGraphRcdMessageService";
	private static int interval = 120;
	private static RcdPageAdapter instance = null;
	private static final String pageType = "PAGE";
	private static final String biz = "PAGE";
	
	private RcdPageAdapter() {
		super(endpoints, interval, Channel.newSocialGraphChannel(endpoints));
	}

	public static RcdPageAdapter getInstance() {
		if (null == instance) {
			synchronized (RcdPageAdapter.class) {
				if (null == instance) {
					try {
						instance = new RcdPageAdapter();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	//推荐中心聚合页page推荐缓存数据取数据接口
	  public List<RcdPageData> getRcdPage(int hostId, int limit) {
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
	
	/**
	 * 删除首页page推荐类型接口,先删cache数据，再存储block表
	 */
	@Override
	public void removeRcd(long source, long block) {
		getRcdMessageServicePrxOneWay(source).removeBizRcd(source, block, pageType);	//删除cache数据
		getRcdMessageServicePrxOneWay(source).addSgBlock(source, block, pageType);	//加入socialgraph block表
	}
	
	  //删除推荐中心页聚合页推荐的page：首先删除cache,然后将删除的数据放入到block表中 
	public void removeBizCache(long source, long block) {
		getRcdMessageServicePrxOneWay(source).removeBizCache(source, block, biz);  //删除对应的业务cache数据
		getRcdMessageServicePrxOneWay(source).addSgBlock(source, block, biz);	//加入socialgraph block表
	}	
	
	/**
	 * 获取twoway代理
	 * @param source
	 * @return
	 */
	private RcdMessageServicePrx getRcdMessageServicePrxTwoWay(long source) {
		Ice.ObjectPrx prx0 = getValidProxy((int)source, 300);
		if (null != prx0) {
			RcdMessageServicePrx prx = RcdMessageServicePrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		}
		return null;
	}
	
	/**
	 * 获取oneway代理
	 * @param source
	 * @return
	 */
	private RcdMessageServicePrx getRcdMessageServicePrxOneWay(long source) {
		Ice.ObjectPrx prx0 = getValidProxy((int)source, 300);
		if (null != prx0) {
			RcdMessageServicePrx prx = RcdMessageServicePrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_oneway());
			return prx;
		}
		return null;
	}

	public static void main(String[] args) {
		//int hostId = 2147463835;
		int hostId = 99995614;
		int blockId = 600002886;
		RcdPageAdapter.getInstance().removeBizCache(hostId,blockId);
		List<RcdPageData> result = RcdPageAdapter.getInstance().getRcdPage(hostId, -1);
		for (RcdPageData data : result) {
			System.out.println("page id: " + data.getPageId() + 
					" fans num: " + data.getFansNum() + " comm frd ids: ");
			String comFrdIds="";
			for (Integer idx : data.getCommonFriends()) {
				comFrdIds += idx + " "; 
			}
			System.out.println(comFrdIds);
		}
		System.out.println("result size: " + result.size());
		System.exit(0);
	}
}
