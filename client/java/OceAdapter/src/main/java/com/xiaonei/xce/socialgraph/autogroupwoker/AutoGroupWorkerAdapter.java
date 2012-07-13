package com.xiaonei.xce.socialgraph.autogroupwoker;

import java.util.HashMap;

import xce.socialgraph.ActionType;
import xce.socialgraph.Info;
import xce.socialgraph.Message;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.SocialGraphBase;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
/**
 * 自动分组从distcache取数据(异步adapter)
 * @author zhangnan
 *
 */
public class AutoGroupWorkerAdapter extends SocialGraphBase {
	private static AutoGroupWorkerAdapter instance_ = null;
	private static String type_ = "FFAG";
	
	private AutoGroupWorkerAdapter() {
		// TODO Auto-generated constructor stub
		super();
	}
	
	public static AutoGroupWorkerAdapter getInstance() {
		if (null == instance_) {
			synchronized (AutoGroupWorkerAdapter.class) {
				if (null == instance_) {
					try {
						instance_ = new AutoGroupWorkerAdapter();
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
	 * 通知autogroup服务计算该用户分组
	 * @param userId
	 */
	public void notifyGetOperation(int userId) {
		Info information = new Info();
		information.hostId = userId;
		information.type = type_;
		information.operation = ActionType.GET;
		
		Message message = new Message();
		message.content = new HashMap<String, Info>();
		message.content.put(type_, information);
		
		getSocialGraphLogicManagerOneWay(userId).Report(message);
	}
	
	/**
	 * 通知autogroup服务更新该用户分组
	 * @param userId
	 */
	public void updateOperation(int userId) {
		Info information = new Info();
		information.hostId = userId;
		information.type = type_;
		information.operation = ActionType.UPDATE;
		
		Message message = new Message();
		message.content = new HashMap<String, Info>();
		message.content.put(type_, information);
		
		getSocialGraphLogicManagerOneWay(userId).Report(message);
	}
	
	/**
	 * 从distcache中获取该用户分组数据
	 * @param userId
	 * @return
	 */
	public GroupManager getGroupRecommend(int userId) {
		GroupManager result = null;
		byte[] data = getData(type_, userId);
		if (null != data) {
			try {
				Items groupResult = Items.parseFrom(data);
				result = new GroupManager(groupResult);
				result.initialize();
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return result;
	}
	
	/**
	 * rebuild该用户分组数据
	 * @param userId
	 */
	public void rebuildGroup(int userId) {
		Info information = new Info();
		information.hostId = userId;
		information.type = type_;
		information.operation = ActionType.CREATE;
		
		Message message = new Message();
		message.content = new HashMap<String, Info>();
		message.content.put(type_, information);
		
		report(userId, message);
	}
	
//	public static void main(String[] args) {
//		int userId = 238111132;
////		AutoGroupWorkerAdapter.getInstance().rebuildGroup(userId);
//		AutoGroupWorkerAdapter.getInstance().updateOperation(userId);
//		GroupManager result = AutoGroupWorkerAdapter.getInstance().getGroupRecommend(userId);
//		Map<Integer, Group> groupMap = result.getGroupMap();
//		System.out.println("groupMap size : " + groupMap.size());
//		for (Entry<Integer, Group> item : groupMap.entrySet()) {
//			System.out.println(item.getKey());
//			for (String name : item.getValue().getGroupLabel()) {
//				System.out.print(name + " ");
//			}
//			System.out.println();
//			for (Integer id : item.getValue().getGroupMember()) {
//				System.out.print(id + " ");
//			}
//			System.out.println();
//		}
//		
//		FliterCondition condition = new FliterCondition();
//		condition.setSchoolName("");
//		condition.setAcademe("公共管学院");
//		condition.setEnrollYear(1999);
//		List<Integer> reids = result.getGroupMember(condition);
//		System.out.println("reids size : " + reids.size());
//		for(Integer rid : reids) {
//			System.out.print(rid + " ");
//		}
//		System.out.println();
//		System.out.println();
//		System.exit(0);
//	}
}
