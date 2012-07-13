package com.xiaonei.xce.buddyadapter.buddycore;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.BasicConfigurator;

import xce.clusterstate.ReplicatedClusterAdapter;

import com.xiaonei.service.buddy.ApplyInfo;
import com.xiaonei.service.buddy.Applyship;
import com.xiaonei.service.buddy.Blockship;
import com.xiaonei.service.buddy.BuddyCoreManagerPrx;
import com.xiaonei.service.buddy.BuddyCoreManagerPrxHelper;
import com.xiaonei.service.buddy.BuddyDesc;
import com.xiaonei.service.buddy.Friendship;
import com.xiaonei.service.buddy.Relationship;
import com.xiaonei.xce.buddyadapter.buddyrelationcache.BuddyRelationCacheAdapter;
import com.xiaonei.xce.domain.buddy.BuddyApply;
import com.xiaonei.xce.domain.buddy.BuddyApplyWithInfo;
import com.xiaonei.xce.domain.buddy.BuddyBlock;
import com.xiaonei.xce.domain.buddy.BuddyRelation;
import com.xiaonei.xce.domain.buddy.CheckedBuddyRelation;
import com.xiaonei.xce.log.Oce;

/**
 * BuddyCoreAdapterImpl
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyCoreAdapterImpl extends ReplicatedClusterAdapter {

    private static BuddyCoreAdapterImpl _adapter = new BuddyCoreAdapterImpl();

    private static final String ctr_endpoints = "ControllerBuddyCore";
    private static int _interval = 120;

    public BuddyCoreAdapterImpl() {
	super(ctr_endpoints, _interval);
    }

    /**
     * @return
     */
    public static BuddyCoreAdapterImpl getInstance() {
	return _adapter;
    }

    /**
     * @param request
     */
    @Deprecated
    public void addApply(BuddyApplyWithInfo request) {
	addApply(request, "UNKNOWN");
    }

    /**
     * @param request
     * @param ref
     */
    public void addApply(BuddyApplyWithInfo request, String ref) {
	Applyship apply = new Applyship();
	apply.accepter = request.getAccepter();
	apply.applicant = request.getApplicant();
	ApplyInfo info = new ApplyInfo();
	info.props = request.getProperties();
	Map<String, String> ctx = new HashMap<String, String>();
	ctx.put("REF", ref);
	try {
	    getBuddyCoreManager(apply.applicant).addApply(apply, info, ctx);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addApply (" +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addApply (" +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".addApply (" + apply.applicant
			    + ":" + apply.accepter + ") ", e);
	}
    }

    /**
     * @param requests
     */
    public void addApplyBatch(List<BuddyApplyWithInfo> requests) {
	addApplyBatch(requests, "UNKNOWN");
    }

    /**
     * @param requests
     * @param ref
     */
    public void addApplyBatch(List<BuddyApplyWithInfo> requests, String ref) {
	Map<Integer, Applyship> applies = new HashMap<Integer, Applyship>();
	Map<Integer, ApplyInfo> infos = new HashMap<Integer, ApplyInfo>();
	if (requests.size() == 0) {
	    return;
	}
	int userId = requests.get(0).getAccepter();
	for (BuddyApplyWithInfo one : requests) {
	    if (one.getAccepter() != userId) {
		Oce.getLogger()
			.error("err in BuddyCoreAdapterImpl.addApplyBatch(): same accepter id expected");
		return;
	    }
	    Applyship apply = new Applyship();
	    apply.accepter = userId;
	    apply.applicant = one.getApplicant();
	    ApplyInfo info = new ApplyInfo();
	    info.props = one.getProperties();

	    applies.put(apply.applicant, apply);
	    infos.put(apply.applicant, info);
	}
	Map<String, String> ctx = new HashMap<String, String>();
	ctx.put("REF", ref);
	try {
	    getBuddyCoreManager(userId).addApplyBatch(applies, infos, ctx);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addApplyBatch ( * :"
	     * + userId + ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addApplyBatch ( * :"
	     * + userId + ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".addApplyBatch ("
			    + applies.size() + ") ", e);
	}

    }

    /**
     * @param request
     */
    public void acceptApply(BuddyApply request) {
	Applyship apply = new Applyship();
	apply.accepter = request.getAccepter();
	apply.applicant = request.getApplicant();
	try {
	    getBuddyCoreManager(apply.accepter).acceptApply(apply);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.acceptApply ( " +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.acceptApply (" +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".acceptApply ("
			    + apply.applicant + ":" + apply.accepter + ") ", e);
	}

    }

    /**
     * @param accepter
     */
    public void acceptAllApply(int accepter) {
	try {
	    getBuddyCoreManager(accepter).acceptAllApplied(accepter);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.acceptAllApply ( " +
	     * accepter + ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.acceptAllApply (" +
	     * accepter + ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".acceptAllApply (" + accepter
			    + ") ", e);
	}
    }

    /**
     * @param request
     */
    public void denyApply(BuddyApply request) {
	Applyship apply = new Applyship();
	apply.accepter = request.getAccepter();
	apply.applicant = request.getApplicant();
	try {
	    getBuddyCoreManager(apply.accepter).denyApply(apply);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.denyApply ( " +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.denyApply (" +
	     * apply.applicant + ":" + apply.accepter +
	     * ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".denyApply ("
			    + apply.applicant + ":" + apply.accepter + ") ", e);
	}
    }

    /**
     * @param accepter
     */
    public void denyAllApply(int accepter) {
	getBuddyCoreManager(accepter).denyAllApplied(accepter);
    }

    /**
     * @param applicant
     */
    public void denyAllApplyByApplicant(int applicant) {
	getBuddyCoreManager(applicant).denyAllApply(applicant);
    }

    /**
     * @param accepter
     * @return
     */
    public int getAppliedCount(int accepter) {
	int count = 0;
	try {
	    count = getBuddyCoreManager(accepter).getAppliedCount(accepter);
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getAppliedCount ( " + accepter
			    + ") Ice.ConnectTimeoutException");
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getAppliedCount (" + accepter
			    + ") Ice.TimeoutException");
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getAppliedCount (" + accepter
			    + ") ", e);
	}
	return count;
    }

    /**
     * @param applicant
     * @param begin
     * @param limit
     * @return
     */
    public List<BuddyRelation> getApplySeq(int applicant, int begin, int limit) {
	List<BuddyRelation> result = new ArrayList<BuddyRelation>();
	int[] accepters;
	BuddyCoreManagerPrx prx = null;
	try {
	    prx = getBuddyCoreManager(applicant);
	    accepters = prx.getApplySeq(applicant, begin, limit);
	    for (int accepter : accepters) {
		result.add(new BuddyRelation(applicant, accepter,
			BuddyRelation.BuddyDesc.Apply));
	    }
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getApplySeq ( " + applicant
			    + ") Ice.ConnectTimeoutException " + prx);
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getApplySeq (" + applicant
			    + ") Ice.TimeoutException " + prx);
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getApplySeq (" + applicant
			    + ") " + prx, e);
	}

	return result;
    }

    /**
     * @param fromId
     * @param toId
     * @return
     */
    public BuddyRelation getRelation(int fromId, int toId) {
	return BuddyRelationCacheAdapter.getInstance()
		.getRelation(fromId, toId);
    }

    /**
     * @param fromId
     * @param toId
     * @return
     */
    public BuddyRelation getBuddyCoreRelation(int fromId, int toId) {
	Relationship ship = new Relationship();
	ship.from = fromId;
	ship.to = toId;
	BuddyDesc desc = com.xiaonei.service.buddy.BuddyDesc.NoPath;
	BuddyCoreManagerPrx prx = null;
	try {
	    prx = getBuddyCoreManager(ship.from);
	    desc = prx.getRelation(ship);
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getRelation ( " + fromId + ":" + toId
			    + ") Ice.ConnectTimeoutException " + prx);
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getRelation (" + fromId + ":" + toId
			    + ") Ice.TimeoutException " + prx);
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getRelation (" + fromId + ":"
			    + toId + ") " + prx, e);
	}

	return new BuddyRelation(ship.from, ship.to,
		com.xiaonei.xce.domain.buddy.BuddyRelation.parseBuddyDesc(desc));
    }

    /**
     * @param fromId
     * @param toId
     * @return
     */
    public CheckedBuddyRelation getBuddyCoreCheckedRelation(int fromId, int toId) {
	Relationship ship = new Relationship();
	ship.from = fromId;
	ship.to = toId;
	BuddyDesc desc = com.xiaonei.service.buddy.BuddyDesc.NoPath;
	BuddyCoreManagerPrx prx = null;
	boolean success = false;
	try {
	    prx = getBuddyCoreManager(ship.from);
	    desc = prx.getRelation(ship);
	    success = true;
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getRelation ( " + fromId + ":" + toId
			    + ") Ice.ConnectTimeoutException " + prx);
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getRelation (" + fromId + ":" + toId
			    + ") Ice.TimeoutException " + prx);
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getRelation (" + fromId + ":"
			    + toId + ") " + prx, e);
	}

	BuddyRelation r = new BuddyRelation(ship.from, ship.to,
		com.xiaonei.xce.domain.buddy.BuddyRelation.parseBuddyDesc(desc));
	return new CheckedBuddyRelation(r, success);
    }

    /**
     * @param block
     */
    public void addBlock(BuddyBlock block) {
	Blockship blockship = new Blockship();
	blockship.from = block.fromId;
	blockship.to = block.toId;
	try {
	    getBuddyCoreManager(blockship.from).addBlock(blockship);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addBlock ( " +
	     * blockship.from + ":" + blockship.to +
	     * ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.addBlock (" +
	     * blockship.from + ":" + blockship.to + ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".addBlock (" + +blockship.from
			    + ":" + blockship.to + ") ", e);
	}
    }

    /**
     * @param from
     * @return
     */
    public int getBlockCount(int from) {
	int count = 0;
	try {
	    count = getBuddyCoreManager(from).getBlockCount(from);
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getBlockCount ( " + from
			    + ") Ice.ConnectTimeoutException");
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getBlockCount (" + from
			    + ") Ice.TimeoutException");
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getBlockCount (" + from
			    + ") ", e);
	}

	return count;
    }

    /**
     * @param userId
     * @return
     */
    public int getBuddyCount(int userId) {
	int count = 0;
	try {
	    count = getBuddyCoreManager(userId).getFriendCount(userId);
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getBuddyCount ( " + userId
			    + ") Ice.ConnectTimeoutException");
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getRelation (" + userId
			    + ") Ice.TimeoutException");
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getBuddyCount (" + userId
			    + ") ", e);
	}

	return count;
    }

    /**
     * @param block
     */
    public void removeBlock(BuddyBlock block) {
	Blockship blockship = new Blockship();
	blockship.from = block.fromId;
	blockship.to = block.toId;
	try {
	    getBuddyCoreManager(blockship.from).removeBlock(blockship);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.removeBlock ( " +
	     * blockship.from + ":" + blockship.to +
	     * ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.removeBlock (" +
	     * blockship.from + ":" + blockship.to + ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".removeBlock ("
			    + blockship.from + ":" + blockship.to + ") ", e);
	}
    }

    /**
     * @param relation
     */
    public void removeFriend(BuddyRelation relation) {
	Friendship buddy = new Friendship();
	buddy.from = relation.fromId;
	buddy.to = relation.toId;
	try {
	    getBuddyCoreManager(buddy.from).removeFriend(buddy);
	} catch (Ice.ConnectTimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.removeFriend ( " +
	     * buddy.from + ":" + buddy.to + ") Ice.ConnectTimeoutException");
	     */
	} catch (Ice.TimeoutException e) {
	    /*
	     * Oce.getLogger().error("BuddyCoreAdapterImpl.removeFriend (" +
	     * buddy.from + ":" + buddy.to + ") Ice.TimeoutException");
	     */
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".removeFriend (" + buddy.from
			    + ":" + buddy.to + ") ", e);
	}
    }

    /**
     * @param userId
     * @param begin
     * @param limit
     * @return
     */
    public List<BuddyRelation> getFriendList(int userId, int begin, int limit) {
	List<BuddyRelation> result = new ArrayList<BuddyRelation>();
	int[] friends;
	try {
	    friends = getBuddyCoreManager(userId).getFriendSeq(userId, begin,
		    limit);
	    for (int f : friends) {
		result.add(new BuddyRelation(userId, f,
			BuddyRelation.BuddyDesc.Friend));
	    }
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getFriendList ( " + userId
			    + ") Ice.ConnectTimeoutException");
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.getFriendList (" + userId
			    + ") Ice.TimeoutException");
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".getFriendList (" + userId
			    + ") ", e);
	}

	return result;
    }

    /**
     * @param userId
     */
    public void reload(int userId) {
	try {
	    getBuddyCoreManager(userId).reload(userId);
	} catch (Ice.ConnectTimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.reload ( " + userId
			    + ") Ice.ConnectTimeoutException");
	} catch (Ice.TimeoutException e) {
	    Oce.getLogger().error(
		    "BuddyCoreAdapterImpl.reload (" + userId
			    + ") Ice.TimeoutException");
	} catch (java.lang.Throwable e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".reload (" + userId + ") ", e);
	}
    }

    /**
     * @param id
     * @return
     */
    public BuddyCoreManagerPrx getBuddyCoreManager(int id) {
	Ice.ObjectPrx prx0 = getValidProxy(id, 300);
	if (prx0 != null) {
	    BuddyCoreManagerPrx prx = BuddyCoreManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    /**
     * @param id
     * @return
     */
    public BuddyCoreManagerPrx getBuddyCoreManagerOneway(int id) {
	Ice.ObjectPrx prx0 = getValidProxy(id, 300);
	if (prx0 != null) {
	    BuddyCoreManagerPrx prx = BuddyCoreManagerPrxHelper
		    .uncheckedCast(prx0.ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

    /**
     * @param fromId
     * @param toId
     */
    public void addFriendDirectly(int fromId, int toId) {
	BuddyRelation br = getRelation(fromId, toId);
	switch (br.getDesc()) {
	case Self:
	case Friend:
	case Block:
	case Blocked:
	case BiBlock:
	    return;
	case Apply: {
	    acceptApply(new BuddyApply(fromId, toId));
	}
	    break;
	case Applied: {
	    acceptApply(new BuddyApply(toId, fromId));
	}
	    break;
	case NoPath: {
	    try {
		getBuddyCoreManager(fromId).addFriend(fromId, toId);
	    } catch (Ice.ConnectTimeoutException e) {
		/*
		 * Oce.getLogger().error("BuddyCoreAdapterImpl.addFriendDirectly ( "
		 * + fromId + ":" + toId + ") Ice.ConnectTimeoutException");
		 */
	    } catch (Ice.TimeoutException e) {
		/*
		 * Oce.getLogger().error("BuddyCoreAdapterImpl.addFriendDirectly ("
		 * + fromId + ":" + toId + ") Ice.TimeoutException");
		 */
	    } catch (java.lang.Throwable e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".addFriendDirectly ("
				+ fromId + ":" + toId + ") ", e);
	    }

	}
	    break;
	default:
	    break;
	}

    }

//    public static void main(String args[]) {
//	BasicConfigurator.configure();
//	BuddyCoreAdapterImpl core = new BuddyCoreAdapterImpl();
//
//	List<Integer> list = new ArrayList<Integer>();
//	list.add(263353981);
//	list.add(263354209);
//	list.add(263385508);
//	list.add(263385722);
//	list.add(268368877);
//	list.add(263328740);
//	list.add(330512092);
//	list.add(364513305);
//	list.add(342270153);
//	list.add(342376168);
//	list.add(222222222);
//	list.add(367599331);
//	for (Integer id : list) {
//	    System.out.println(id);
//	    core.acceptAllApply(id);
//	    try {
//		TimeUnit.MILLISECONDS.sleep(100);
//	    } catch (InterruptedException e) {
//		e.printStackTrace();
//	    }
//	}
//
//	System.exit(0);
//    }
}
