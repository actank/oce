package com.xiaonei.xce.socialgraph.relation.impl;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.EmotionState;
import xce.socialgraph.EmotionTuple;
import xce.socialgraph.SgRelationServicePrx;
import xce.socialgraph.SgRelationServicePrxHelper;
import xce.util.channel.Channel;

import com.xiaonei.xce.socialgraph.relation.RelationService;
import com.xiaonei.xce.socialgraph.relation.emotionstate.Emotion;
import com.xiaonei.xce.socialgraph.relation.emotionstate.EmotionStateFactory;
import com.xiaonei.xce.socialgraph.relation.emotionstate.LevelTupleFactory;

/**
 * 客户端实现
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class RelationServiceClientImpl extends ReplicatedClusterAdapter
	implements RelationService {

    private static final String endpoints = "ControllerSocialGraphRelationService";

    private static int interval = 120;

    private static final int TIME_OUT = 2000;

    private static RelationService instance = null;

    private RelationServiceClientImpl() {
	super(endpoints, interval, Channel.newSocialGraphChannel(endpoints));
    }

    public static RelationService getInstance() {
	if (instance == null) {
	    synchronized (RelationServiceClientImpl.class) {
		if (instance == null) {
		    try {
			instance = new RelationServiceClientImpl();
		    } catch (Exception e) {
			e.printStackTrace();
		    }
		}
	    }
	}
	return instance;
    }

    private SgRelationServicePrx getSgRelationServicePrxTwoWay(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId % 10, TIME_OUT);
	if (prx0 != null) {
	    SgRelationServicePrx prx = SgRelationServicePrxHelper
		    .uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    private SgRelationServicePrx getSgRelationServicePrxOneWay(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId % 10, TIME_OUT);
	if (prx0 != null) {
	    SgRelationServicePrx prx = SgRelationServicePrxHelper
		    .uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

    @Override
    public Emotion getEmotion(int userId) {
	Emotion emotion = null;
	EmotionState es = null;
	try {
	    es = getSgRelationServicePrxTwoWay(userId).getEmotionState(userId);
	    emotion = new Emotion(es);
	} catch (Exception e) {
	    e.printStackTrace();
	    emotion = new Emotion(EmotionStateFactory.create(userId));
	}
	return emotion;
    }

    @Override
    public boolean court(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.courtLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean court(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.courtLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.court = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean divorced(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.divorcedLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean engaged(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.engagedLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean engaged(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.engagedLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.engaged = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean inlove(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.inloveLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean inlove(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.inloveLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.sweetheart = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean inloveColdwar(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.inloveColdwarLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean inloveColdwar(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.inloveColdwarLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.sweetheart = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean like(int host) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.likeLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean like(int host, List<Integer> userIdList) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.likeLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	if (userIdList != null && !userIdList.isEmpty()) {
	    es.likeList = userIdList;
	} else {
	    es.likeList = new ArrayList<Integer>();
	}
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean married(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.marriedLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean married(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.marriedLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.married = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean marriedColdwar(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.marriedColdwarLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean marriedColdwar(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.marriedColdwarLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.married = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean marriedHoneymoon(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.marriedHoneymoonLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean marriedHoneymoon(int host, int guest) {
	EmotionTuple et = new EmotionTuple(host, LevelTupleFactory
		.marriedHoneymoonLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	es.married = guest;
	try {
	    getSgRelationServicePrxTwoWay(host).setEmotionState(host, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

    @Override
    public boolean single(int userId) {
	EmotionTuple et = new EmotionTuple(userId, LevelTupleFactory
		.singleLevelTuple());
	EmotionState es = new EmotionState();
	es.et = et;
	try {
	    getSgRelationServicePrxTwoWay(userId).setEmotionState(userId, es);
	} catch (Exception e) {
	    e.printStackTrace();
	    return false;
	}
	return true;
    }

}
