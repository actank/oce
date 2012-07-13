#ifndef __QUEST_ICE__
#define __QUEST_ICE__

#include <Util.ice>

module xce {
module quest {

// ------------ 接口定义部分 -------------------------
/**
 * 用户任务
 */
class Tip {
	int id;
	string title;
	string content;
	string url;
};
sequence<Tip> TipSeq;
dictionary<int, Tip> TipMap;

/**
 * 一个任务流程中的任务成员
 */
struct TipOrder {
	int tipId;		/* 任务id */
	int showCount;		/* 最多在不被点击的情况下可被显示的次数 */
};
sequence<TipOrder> TipOrderSeq;


/**
 * 任务流程
 */
class TipFlow {
	TipOrderSeq seq;
};
sequence<TipFlow> TipFlowSeq;

/**
 * Logic服务接口
 */

class QuestLogic {

	/** 
	 * @brief	增加一个新的任务
	 * @param	title	任务标题
	 * @param	content	任务正文
	 * @param	url	任务特征url
	 * @param	showcount	该任务最多被显示的次数
	 **/
	void addTip(string title, string content, string url, int showcount);

	/**
	 * @brief	删除一个任务
	 * @param	tipid	任务id
	 **/
	void removeTip(int tipid);

	/**
	 * @brief	修改任务信息
	 * @param	tipid	任务id
	 * @param	title	任务标题(置为空串则为不修改)
	 * @param	content	任务正文(置为空串则为不修改)
	 * @param	url	任务特征url(置为空串则为不修改)
	 **/
	void modifyTip(int tipid, string title, string content, string url);

	/**
	 * @brief	设置当前的任务流程
	 * @param	flow	新设的任务流程
	 **/
	void setTipFlow(TipFlow flow);

	/**
	 * @brief	更新所有Tips和Flows
	 **/
	void renew();
};

/**
 * Cache服务接口
 */
class QuestCache {
	
	/**
	 * @brief	为一个新用户开辟一个新的任务流程
	 * @param	userid	用户id
	 **/	
	void newFlow(int userid);

	/** 
	 * @brief	更新任务列表	
	 **/
	void renewTips();
	
	/**
	 * @brief	更新任务流程
	 **/
	void renewFlow();

	/**
	 * @brief	显示给用户当前应该显示给他/她的任务
	 * @param	用户id
	 * @return	当前应显示的用户任务
	 **/
	Tip show(int userid);

	/**
	 * @brief	用户访问了一个任务
	 * @param 	tipid 	被访问的任务id
	 * @param 	userid	用户id
	 **/
	void click(int tipid, int userid);

	/**
	 * @brief	显示所有Tips
	 * @return	按id递增排列的所有Tips
	 **/
	TipSeq listTips();

	/**
	 * @brief	显示所有Flow
	 * @return	按id递增排列的所有flows (最后一个为当前使用的)
	 **/
	TipFlowSeq listFlows();

	/**
         * @brief       捕获url并作相应处理
         * @param       userid  用户id
         * @param       url     当前访问的url
         **/
        void responseUrl(int userid, string url);

};

};
};

#endif
