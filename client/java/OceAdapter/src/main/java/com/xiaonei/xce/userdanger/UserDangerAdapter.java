package com.xiaonei.xce.userdanger;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.userdanger.UGCAUDITFIRST;
import xce.userdanger.MINISITEHIDE;
import xce.userdanger.MAILSUBSCRIPTIONSHIELDING;
import xce.userdanger.NOTACCEPTBUDDYAPPLICATION;
import xce.userdanger.NOTFEED;
import xce.userdanger.ONLYSEENBYFRIEND;
import xce.userdanger.ONLYSEENBYSELF;
import xce.userdanger.SEARCHSHIELDING;
import xce.userdanger.TEST;
import xce.userdanger.UGCALLCHECK;
import xce.userdanger.UGCONLYSEENBYSELF;
import xce.userdanger.MINIGROUPHIDE;
import xce.userdanger.UserDangerManagerPrx;
import xce.userdanger.UserDangerManagerPrxHelper;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.storm.XceStormAdapter;

public final class UserDangerAdapter extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerUserDanger";

    private static int _interval = 120;

    private static UserDangerAdapter _adapter = new UserDangerAdapter();

    protected UserDangerAdapter() {
	super(ctr_endpoints, _interval);
		XceStormAdapter.getInstance().subscribe("USER_DANGER_UPDATER", new UserDangerListener());
    }

    public static UserDangerAdapter getInstance() {
	return _adapter;
    }

    

    public List<Integer> getUserIds(int type) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.getUserIds type: "
		+ type);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(type,300);
	    return udmp.getUserIds(type);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + " .getUserIds [" + udmp
			    + "] type=" + type + " " + e);
	} finally {
	    cost.endFinally();
	}
	return null;
    }
    
    public void setNotFeedOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setNotFeedOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, NOTFEED.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setNotFeedOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    
    public void setNotFeedOff(int userid){
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setNotFeedOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, NOTFEED.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setNotFeedOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setTestOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setTestOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, TEST.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setTestOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setTestOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setTestOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, TEST.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setTestOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public boolean isTest(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isTest id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,TEST.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isTest [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }

    protected UserDangerManagerPrx getUserDangerManager(int userId, int timeout) {
	Ice.ObjectPrx prx0 = getValidProxy(userId, timeout);
	if (prx0 != null) {
	    UserDangerManagerPrx prx = UserDangerManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(timeout).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    protected UserDangerManagerPrx getUserDangerManagerOneway(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
	if (prx0 != null) {
	    UserDangerManagerPrx prx = UserDangerManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

    public void setOn(int userid, int type) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setOn id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, type);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setOff(int userid, int type) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.setOff id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, type);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOff [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    
    public Map<Integer, List<Integer>> getClientCache(){
    	return getUserDangerManager(0,300).getClientCache();
    }
    
    public Map<Integer, Boolean> isSearchShielding(List<Integer> ids){
	TimeCost cost = TimeCost
	.begin("UserDangerAdapter.isSearchShielding size: " + ids.size());
	try {
	    return isTypes(ids,SEARCHSHIELDING.value );
	} finally {
	    cost.endFinally();
	}
    }

    public Map<Integer, Boolean> isTypes(List<Integer> ids,int type){
	Map<Integer, Boolean> result=new HashMap<Integer, Boolean>();
	Set<Integer> s=UserDangerCache.getInstance().getSet(type);
	
	for(Integer id:ids){
	    result.put(id,false);
	}
	if(s!=null){
	    for(Integer id:ids){
		if(s.contains(id)){
		    result.put(id, true);
		}
	    }
	}else{
	    UserDangerManagerPrx udmp = null;
	    try{
		if(ids.size() > 0){
		    udmp=getUserDangerManager(0,300);
		    return udmp.isTypes(ids, type);
		}
	    }catch(Exception e){
		Oce.getLogger().error(
			this.getClass().getName()+".isTypes ["+udmp
				+"] type: "+type+" "+e);
	    }
	}
	return result;
    }
   
    public boolean isType(int userid, int type) {
	Set<Integer> s = UserDangerCache.getInstance().getSet(type);
	if (s != null) {
	    return s.contains(userid);
	} else {
	    UserDangerManagerPrx udmp = null;
	    try {
		if (userid <= 0) {
		    return false;
		}
		udmp = getUserDangerManager(userid,300);
		return udmp.isType(userid, type);
	    } catch (Exception e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".isType [" + udmp
				+ "] id=" + userid + " type: "+type+" "  + e);
		return false;
	    }
	}
    }
    public void setNotAcceptBuddyApplicationOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setNotAcceptBuddyApplicationOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, NOTACCEPTBUDDYAPPLICATION.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setNotAcceptBuddyApplicationOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setNotAcceptBuddyApplicationOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setNotAcceptBuddyApplicationOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, NOTACCEPTBUDDYAPPLICATION.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setNotAcceptBuddyApplicationOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setOnlySeenByFriendOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setOnlySeenByFriendOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, ONLYSEENBYFRIEND.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOnlySeenByFriendOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setOnlySeenByFriendOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setOnlySeenByFriendOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, ONLYSEENBYFRIEND.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOnlySeenByFriendOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setOnlySeenBySelfOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setOnlySeenBySelfOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, ONLYSEENBYSELF.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOnlySeenBySelfOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setOnlySeenBySelfOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setOnlySeenBySelfOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, ONLYSEENBYSELF.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setOnlySeenBySelfOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setUGCOnlySeenBySelfOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUGCOnlySeenBySelfOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, UGCONLYSEENBYSELF.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setUGCOnlySeenBySelfOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setUGCOnlySeenBySelfOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUGCOnlySeenBySelfOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, UGCONLYSEENBYSELF.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setUGCOnlySeenBySelfOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setUGCAllCheckOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUGCAllCheckOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, UGCALLCHECK.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setUGCAllCheckOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setUGCAllCheckOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUGCAllCheckOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, UGCALLCHECK.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setUGCAllCheckOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setSearchShieldingOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setSearchShieldingOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, SEARCHSHIELDING.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setSearchShieldingOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setSearchShieldingOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setSearchShieldingOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, SEARCHSHIELDING.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setSearchShieldingOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    public void setMailSubscriptionShieldingOn(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMailSubscriptionShieldingOn id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOn(userid, MAILSUBSCRIPTIONSHIELDING.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setMailSubscriptionShieldingOn [" + udmp + "] id="
			    + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }

    public void setMailSubscriptionShieldingOff(int userid) {
	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMailSubscriptionShieldingOff id: "
		+ userid);
	UserDangerManagerPrx udmp = null;
	try {
	    udmp = getUserDangerManager(userid,300);
	    udmp.setTypeOff(userid, MAILSUBSCRIPTIONSHIELDING.value);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".setMailSubscriptionShieldingOff [" + udmp
			    + "] id=" + userid + " " + e);
	} finally {
	    cost.endFinally();
	}
    }
    
    public void setUgcAuditFirstOn(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUgcAuditFirstOn id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOn(userid, UGCAUDITFIRST.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setUgcAuditFirstOn [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
    public void setUgcAuditFirstOff(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setUgcAuditFirstOff id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOff(userid, UGCAUDITFIRST.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setUgcAuditFirstOff [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
    public boolean isNotFeed(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isNotFeed id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,NOTFEED.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isNotFeed [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isNotAcceptBuddyApplication(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isNotAcceptBuddyApplication id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,NOTACCEPTBUDDYAPPLICATION.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isNotAcceptBuddyApplication [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isOnlySeenByFriend(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isOnlySeenByFriend id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,ONLYSEENBYFRIEND.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isOnlySeenByFriend [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isOnlySeenBySelf(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isOnlySeenBySelf id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,ONLYSEENBYSELF.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isOnlySeenBySelf [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isUGCOnlySeenBySelf(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isUGCOnlySeenBySelf id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,UGCONLYSEENBYSELF.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isUGCOnlySeenBySelf [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isUGCAllCheck(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isUGCAllCheck id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,UGCALLCHECK.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isUGCAllCheck [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isSearchShielding(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isSearchShielding id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,SEARCHSHIELDING.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isSearchShielding [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    public boolean isMailSubscriptionShielding(int userid) {
	TimeCost cost = TimeCost
		.begin("UserDangerAdapter.isMailSubscriptionShielding id: " + userid);
	UserDangerManagerPrx udmp = null;
	try {
	    return isType(userid,MAILSUBSCRIPTIONSHIELDING.value );
	} catch (Exception e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".isMailSubscriptionShielding [" + udmp + "] id="
			    + userid + " " + e);
	    return false;
	} finally {
	    cost.endFinally();

	}
    }
    
    public boolean isUgcAuditFirst(int userid) {
    	TimeCost cost = TimeCost
    		.begin("UserDangerAdapter.isUgcAuditFirst id: " + userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    return isType(userid,UGCAUDITFIRST.value );
    	} catch (Exception e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".isUgcAuditFirst [" + udmp + "] id="
    			    + userid + " " + e);
    	    return false;
    	} finally {
    	    cost.endFinally();

    	}
        }
    public boolean isMiniSiteHide(int userid) {
    	TimeCost cost = TimeCost
    		.begin("UserDangerAdapter.isMiniSiteHide id: " + userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    return isType(userid,MINISITEHIDE.value );
    	} catch (Exception e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".isMiniSiteHide [" + udmp + "] id="
    			    + userid + " " + e);
    	    return false;
    	} finally {
    	    cost.endFinally();

    	}
        }
    public boolean isMiniGroupHide(int userid) {
    	TimeCost cost = TimeCost
    		.begin("UserDangerAdapter.isMiniGroupHide id: " + userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    return isType(userid,MINIGROUPHIDE.value );
    	} catch (Exception e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".isMiniGroupHide [" + udmp + "] id="
    			    + userid + " " + e);
    	    return false;
    	} finally {
    	    cost.endFinally();

    	}
        }
  
    public void setMiniGroupHideOn(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMiniGroupHideOn id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOn(userid, MINIGROUPHIDE.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setMiniGroupHideOn [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
    public void setMiniGroupHideOff(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMiniGroupHideOff id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOff(userid, MINIGROUPHIDE.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setMiniGroupHideOff [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
    public void setMiniSiteHideOn(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMiniSiteHideOn id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOn(userid, MINISITEHIDE.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setMiniSiteHideOn [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
    public void setMiniSiteHideOff(int userid) {
    	TimeCost cost = TimeCost.begin("UserDangerAdapter.setMiniSiteHideOff id: "
    		+ userid);
    	UserDangerManagerPrx udmp = null;
    	try {
    	    udmp = getUserDangerManager(userid,300);
    	    udmp.setTypeOff(userid, MINISITEHIDE.value);
    	} catch (TimeoutException e) {
    	    Oce.getLogger().error(
    		    this.getClass().getName() + ".setMiniSiteHideOff [" + udmp + "] id="
    			    + userid + " " + e);
    	} finally {
    	    cost.endFinally();
    	}
        }
}
