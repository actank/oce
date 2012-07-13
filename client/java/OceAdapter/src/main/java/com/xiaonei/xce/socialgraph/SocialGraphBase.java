package com.xiaonei.xce.socialgraph;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.Message;
import xce.socialgraph.SocialGraphLogicPrx;
import xce.socialgraph.SocialGraphLogicPrxHelper;
/**
 * SocialGraphAdapter基础类，封装了取distcache接口和通知socialgraphlogic操作
 */
public abstract class SocialGraphBase extends ReplicatedClusterAdapter {
	private static final String endpoints_ = "ControllerSocialGraphLogic";
	private static int interval_ = 120;	
	
	public SocialGraphBase() {
		// TODO Auto-generated constructor stub
		super(endpoints_, interval_);
	}
	
	/**
	 * 从distcache中取出数据
	 * @param type  业务类型
	 * @param userId
	 */
	public byte[] getData(String type, int userId) {
		return MemcachedClient.getInstance().getByDistCache(type, userId);
	}
	
	/**
	 * 向distcache中插入数据
	 * @param type 业务类型
	 * @param userId 用户id
	 * @param data 数据
	 * @return
	 */
	public boolean setData(String type, int userId, byte[] data) {
		return MemcachedClient.getInstance().setByDistCache(type, userId, data);
	}
	
	/**
	 * 通知socialgraphlogic接口
	 * @param userId 用户id
	 * @param message 发送约定的数据格式
	 */
	public void report(int userId, Message message) {
		getSocialGraphLogicManagerOneWay(userId).Report(message);
	}
	
	/**
	 * 获取代理(oneway)
	 * @param userId
	 * @return
	 */
	protected SocialGraphLogicPrx getSocialGraphLogicManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			SocialGraphLogicPrx prx = SocialGraphLogicPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} 
		return null;
	}
	
	/**
	 * 获取代理(twoway)
	 * @param userId
	 * @return
	 */
	protected SocialGraphLogicPrx getSocialGraphLogicManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			SocialGraphLogicPrx prx = SocialGraphLogicPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} 
		return null;
	}
}
