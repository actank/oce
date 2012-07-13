package com.xiaonei.xce.completeuserinfo;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import com.xiaonei.model.BuddyGroupManagerPrx;
import com.xiaonei.model.BuddyGroupManagerPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.completeuserinfo.*;

public class CompleteUserInfoAdapter extends ReplicatedClusterAdapter {
	public CompleteUserInfoAdapter() {
		super(ctr_endpoints_, interval_);	
		runThread_ = new RunClass();
		runThread_.start();
	}
	public void completeUserInfo(int[] userIds) {
		synchronized(userIds_) {
			for(int i = 0; i < userIds.length; ++i) {
				userIds_.add(userIds[i]);
			}
		}
	}
	public void completeUserInfo(int userId) {
		synchronized(userIds_) {
			userIds_.add(userId);
		}
	}
	public void setValid(boolean valid) {
		getCompleteUserInfoManager(0).setValid(valid);
	}
	public boolean isValid() {
		return getCompleteUserInfoManager(0).isValid();
	}
	private CompleteUserInfoManagerPrx getCompleteUserInfoManager(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
                CompleteUserInfoManagerPrx prx = CompleteUserInfoManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
                return prx;
        } else {
                return null;
        }
	}
	private class RunClass extends Thread {
		public void run() {
			while(true) {
				try {
					Vector<Integer> ids = null;
					synchronized(userIds_) {
						if(userIds_.size() > 0) {
							ids = (Vector<Integer>) userIds_.clone();
							userIds_.clear();
						}
					}
					if(ids != null && ids.size() > 0) {
						int size = ids.size();
						int[] userIds = new int[size];
						for(int i = 0; i < size; ++i) {
							userIds[i] = ids.get(i);
						}
						getCompleteUserInfoManager(0).completeUserInfo(userIds);
					}
					Thread.sleep(1000);	
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}
	private Vector<Integer> userIds_ = new Vector<Integer>();
	private RunClass runThread_;
	private static final String ctr_endpoints_ = "ControllerCompleteUserInfo";
	private static final String logic_endpoints_ = "CompleteUserInfo";
	private static int interval_ = 120;
	public static void main(String args[]) {
		int[] userIds = new int[3];
		System.out.println("new a CompleteUserInfoAdapter");
		CompleteUserInfoAdapter adapter = new CompleteUserInfoAdapter();
		System.out.println("start to completeUserInfo with id 832541251");
		adapter.completeUserInfo(832541251);
		userIds[0] = 800000000;
		userIds[1] = 711111111;
		userIds[2] = 832541251;
		System.out.println("start to completeUserInfo with 3 ids");
		adapter.completeUserInfo(userIds);
	}
}
