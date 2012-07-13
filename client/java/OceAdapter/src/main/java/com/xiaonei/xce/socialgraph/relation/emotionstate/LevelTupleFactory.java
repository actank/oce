package com.xiaonei.xce.socialgraph.relation.emotionstate;

import xce.socialgraph.LevelTuple;
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
import xce.socialgraph.kSecondLevelError;
import xce.socialgraph.kSingleBeCourted;
import xce.socialgraph.kSingleBeliked;
import xce.socialgraph.kSingleCourt;
import xce.socialgraph.kSingleDivorced;
import xce.socialgraph.kSingleLike;

/**
 * java版本的LevelTupleFactory与server端一样
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class LevelTupleFactory {

    public static LevelTuple errorLevelTuple() {
	return new LevelTuple(kFirstLevelError.value, kSecondLevelError.value);
    }

    public static LevelTuple emptyLevelTuple() {
	return new LevelTuple(kFirstLevelEmpty.value, kSecondLevelEmpty.value);
    }

    public static LevelTuple singleLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSecondLevelEmpty.value);
    }

    public static LevelTuple divorcedLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSingleDivorced.value);
    }

    public static LevelTuple courtLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSingleCourt.value);
    }

    public static LevelTuple beCourtedLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSingleBeCourted.value);
    }

    public static LevelTuple likeLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSingleLike.value);
    }

    public static LevelTuple beLikedLevelTuple() {
	return new LevelTuple(kFirstLevelSingle.value, kSingleBeliked.value);
    }

    public static LevelTuple inloveLevelTuple() {
	return new LevelTuple(kFirstLevelInlove.value, kSecondLevelEmpty.value);
    }

    public static LevelTuple inloveColdwarLevelTuple() {
	return new LevelTuple(kFirstLevelInlove.value, kInloveColdwar.value);
    }

    public static LevelTuple engagedLevelTuple() {
	return new LevelTuple(kFirstLevelEngaged.value, kSecondLevelEmpty.value);
    }

    public static LevelTuple marriedLevelTuple() {
	return new LevelTuple(kFirstLevelMarried.value, kSecondLevelEmpty.value);
    }

    public static LevelTuple marriedHoneymoonLevelTuple() {
	return new LevelTuple(kFirstLevelMarried.value, kMarriedHoneymoon.value);
    }

    public static LevelTuple marriedColdwarLevelTuple() {
	return new LevelTuple(kFirstLevelMarried.value, kMarriedColdwar.value);
    }
}
