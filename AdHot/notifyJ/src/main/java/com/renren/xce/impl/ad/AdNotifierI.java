package com.renren.xce.impl.ad;

import Ice.*;
import java.util.*;


import org.apache.log4j.Logger;

// import com.renren.xce.service.ad.AdHotPushService;

// import xce.ad.HotAdState;
// import xce.ad._AdNotifierOperations;
import xce.ad.AdNotifier;
import xce.ad.HotAdInfo;
import xce.ad.IntervalType;
import xce.ad.UserHotAdState;

// 应该是继承 AdNotifier
public final class AdNotifierI extends AdNotifier {

    private final Logger log = Logger.getLogger(AdNotifierI.class.getName());

    private final AdPushManager push_mgr_ = new AdPushManager();
    
    public AdNotifierI() {
		System.out.println("AdNotifierI");
    	push_mgr_.init();
    }
    
    @Override
    public Map<Integer, int[]> GetAdPartners(int uid, int[] ads, Current current) {
    	Map<Integer, int[]> ret = new HashMap<Integer, int[]>();
    	return ret;
    }

    @Override
    public void Login(int uid, Ice.Current __current) {
        log.info("Login uid=" + uid);
    	 push_mgr_.Login(uid);
    }

    @Override
    public void Block(int uid, int promotion, Ice.Current __current) {
    	
    }

    public void SetInterval(int uid, IntervalType time, Ice.Current __current) {
    	
    }

    @Override
    public void MarkReadByUid(int uid, Ice.Current __current) {
    	
    }

    @Override
    public void LoadOnePromotion(int promotion, Ice.Current __current) {
    	
    }

    @Override
    public void AddPromotion(int promotion, int[] partners, long starttime, long endtime, 
    		int level, boolean blockalways, Ice.Current __current) {
    	
    }

    @Override
    public void DelPromotion(int promotion, Ice.Current __current) {
    	
    }

    @Override
    public HotAdInfo[] GetAdInfoSeq(Ice.Current __current) {
    	return new HotAdInfo[0];
    }

    @Override
    public void Reload(Ice.Current current) {
    	
    }

    @Override
    public boolean ResetUserAd(int uid, int aid, Ice.Current __current) {
    	return false;
    }

    @Override
    public UserHotAdState GetUserAdState(int uid, int aid, Ice.Current __current) {
    	return new UserHotAdState();
    }
}
