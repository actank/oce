package com.xiaonei.xce.socialgraph.relation.emotionstate;

import java.util.List;

import xce.socialgraph.EmotionState;
import xce.socialgraph.kFirstLevelEmpty;
import xce.socialgraph.kFirstLevelEngaged;
import xce.socialgraph.kFirstLevelError;
import xce.socialgraph.kFirstLevelInlove;
import xce.socialgraph.kFirstLevelMarried;
import xce.socialgraph.kFirstLevelSingle;
import xce.socialgraph.kInloveColdwar;
import xce.socialgraph.kMarriedColdwar;
import xce.socialgraph.kMarriedHoneymoon;
import xce.socialgraph.kSecondLevelEmpty;
import xce.socialgraph.kSingleCourt;
import xce.socialgraph.kSingleDivorced;
import xce.socialgraph.kSingleLike;

/**
 * 感情状态客户端数据类
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class Emotion {

	private EmotionState es;

	public Emotion(EmotionState es) {
		if (es != null) {
			this.es = es;
		} else {
			es = EmotionStateFactory.create(-1);
		}

	}

	/** 取感情状态 */
	public int getState() {
		int first = es.et.lt.first;
		int second = es.et.lt.second;
		switch (first) {
		case kFirstLevelError.value:
			return EmotionStateConstant.ERROR;
		case kFirstLevelEmpty.value:
			return EmotionStateConstant.EMPTY;
		case kFirstLevelSingle.value:
			switch (second) {
			case kSecondLevelEmpty.value:
				return EmotionStateConstant.SINGLE;
			case kSingleDivorced.value:
				return EmotionStateConstant.DIVORCED;
			case kSingleCourt.value:
				return EmotionStateConstant.COURT;
			case kSingleLike.value:
				return EmotionStateConstant.LIKE;
			}
			break;
		case kFirstLevelInlove.value:
			switch (second) {
			case kSecondLevelEmpty.value:
				return EmotionStateConstant.INLOVE;
			case kInloveColdwar.value:
				return EmotionStateConstant.INLOVE_COLDWAR;
			}
			break;
		case kFirstLevelEngaged.value:
			if (second == kSecondLevelEmpty.value) {
				return EmotionStateConstant.ENGAGED;
			}
			break;
		case kFirstLevelMarried.value:
			switch (second) {
			case kSecondLevelEmpty.value:
				return EmotionStateConstant.MARRIED;
			case kMarriedHoneymoon.value:
				return EmotionStateConstant.MARRIED_HONEYMOON;
			case kMarriedColdwar.value:
				return EmotionStateConstant.MARRIED_COLDWAR;
			}
			break;
		}
		return EmotionStateConstant.ERROR;
	}

	/** 取喜欢列表 */
	public List<Integer> getLikeList() {
		return es.likeList;
	}

	/** 取被喜欢列表 */
	public List<Integer> getBeLikedList() {
		return es.beLikedList;
	}

	/** 取追求人 */
	public int getCourt() {
		return es.court;
	}

	/** 取被追求列表 */
	public List<Integer> getBeCourtedList() {
		return es.beCourtedList;
	}

	/** 取已婚的爱人 */
	public int getMarried() {
		return es.married;
	}

	/** 取订婚爱人 */
	public int getEngaged() {
		return es.engaged;
	}

	/** 取恋人 */
	public int getSweetHeart() {
		return es.sweetheart;
	}
}
