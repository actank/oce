package com.xiaonei.xce.socialgraph.relation.emotionstate;

import java.util.ArrayList;

import xce.socialgraph.EmotionState;
import xce.socialgraph.EmotionTuple;

/**
 * 帮助初始化Slice中定义的结构体
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class EmotionStateFactory {

    /**
     * 默认生产出来是的Error状态的
     * 
     * @param userId
     * @return
     */
    public static EmotionState create(int userId) {
	EmotionState es = new EmotionState();
	es.et = new EmotionTuple(userId, LevelTupleFactory.errorLevelTuple());
	es.court = 0;
	es.sweetheart = 0;
	es.engaged = 0;
	es.married = 0;
	es.likeList = new ArrayList<Integer>();
	es.beLikedList = new ArrayList<Integer>();
	es.beCourtedList = new ArrayList<Integer>();
	return es;
    }

}
