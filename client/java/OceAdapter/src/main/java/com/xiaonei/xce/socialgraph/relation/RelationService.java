package com.xiaonei.xce.socialgraph.relation;

import java.util.List;

import com.xiaonei.xce.socialgraph.relation.emotionstate.Emotion;

/**
 * Socialgraph Relation Service接口定义
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public interface RelationService {

    /** 读接口 */
    Emotion getEmotion(int userId);

    /** 设置单身 */
    boolean single(int userId);

    /** 离异 */
    boolean divorced(int userId);

    /** 喜欢某人 */
    boolean like(int host);

    boolean like(int host, List<Integer> userIdList);

    /** 追求某人 */
    boolean court(int userId);

    boolean court(int host, int guest);

    /** 恋爱 */
    boolean inlove(int userId);

    boolean inlove(int host, int guest);

    /** 恋爱冷战 */
    boolean inloveColdwar(int userId);

    boolean inloveColdwar(int host, int guest);

    /** 订婚 */
    boolean engaged(int userId);

    boolean engaged(int host, int guest);

    /** 已婚 */
    boolean married(int userId);

    boolean married(int host, int guest);

    /** 已婚蜜月 */
    boolean marriedHoneymoon(int userId);

    boolean marriedHoneymoon(int host, int guest);

    /** 已婚冷战 */
    boolean marriedColdwar(int userId);

    boolean marriedColdwar(int host, int guest);

}
