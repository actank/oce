package com.xiaonei.xce.socialgraph.autogroupwoker;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.AutoGroupingPrx;
import xce.socialgraph.AutoGroupingPrxHelper;
import xce.socialgraph.Info;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
/**
 * 实时计算好友分组adapter
 * @author zhangnan
 *
 */
public class AutoGroupCommonAdapter extends ReplicatedClusterAdapter {
	private static final String endpoints_ = "ControllerAutoGroup";
	private static int interval_ = 120;	
	private static AutoGroupCommonAdapter instance_ = null;
	
	private AutoGroupCommonAdapter() {
		// TODO Auto-generated constructor stub
		super(endpoints_, interval_);
	}
	
	public static AutoGroupCommonAdapter getInstance() {
		if (null == instance_) {
			synchronized (AutoGroupCommonAdapter.class) {
				if (null == instance_) {
					try {
						instance_ = new AutoGroupCommonAdapter();
					} catch (Exception e) {
						// TODO: handle exception
						e.printStackTrace();
						instance_ = null;
					}
				}
			}
		}
		
		return instance_;
	}
	
	/**
	 * 获取用户推荐分组
	 * @param userId
	 * @return
	 */
	public GroupRecommend getGroupRecommend(int userId) {
		GroupRecommend result = null;
		Info info = getAutoGroupingManager(userId).GetInfo(userId);
		if (null != info.data) {
			Items items;
			try {
				items = Items.parseFrom(info.data);
				result = new GroupRecommend(items);
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		return result;
	}
	
	private AutoGroupingPrx getAutoGroupingManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			AutoGroupingPrx prx = AutoGroupingPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} 
		return null;
	}
	
	public static void main(String[] args) {
		int userId = 238111132;
		GroupRecommend result = AutoGroupCommonAdapter.getInstance().getGroupRecommend(userId);
		Map<String, List<Integer>> map = result.getGroupRecommendMap();
		for (Entry<String, List<Integer>> m : map.entrySet()) {
			System.out.println(m.getKey() + ":");
			for (Integer i : m.getValue()) {
				System.out.print(i + " ");
			}
			System.out.println();
			System.out.println();
		}
	}
}
