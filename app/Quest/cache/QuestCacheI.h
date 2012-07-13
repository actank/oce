#ifndef __QUEST_CACHE_I_H__
#define __QUEST_CACHE_I_H__

#include <Quest.h>
#include "UtilCxx/src/TaskManager.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <Ice/Ice.h>
#include "util/cpp/String.h"
#include "boost/detail/atomic_count.hpp"

namespace xce {
namespace quest {

class TipIndex : virtual public Ice::Object{
public:
	TipPtr get(int tipid) {
		TipMap::iterator it = tipMap.find(tipid);
		if ( it != tipMap.end() ) {
			return (*it).second;
		} else {
			return NULL;
		}
	};

	void insert(int tipid, TipPtr& tip) {
		tipMap.insert(make_pair<int, TipPtr>(tipid, tip));
	};

	const TipMap& getTipMap() {
		return tipMap;
	}
private:
	TipMap tipMap;
};
typedef IceUtil::Handle<TipIndex> TipIndexPtr;

class FlowIndex : virtual public Ice::Object{
public:
	FlowIndex(map<int, TipFlowPtr>& flowMap) {
		flowMap_.swap(flowMap);
		if ( flowMap_.size() > 0 ) {
			map<int, TipFlowPtr>::reverse_iterator rit = flowMap_.rbegin();
			currentFlow_ = (*rit).second;
			currentFlowId_ = (*rit).first;
		}
	}

	const TipFlowPtr getFlow(int flowid) {
		map<int, TipFlowPtr>::iterator it = flowMap_.find(flowid);
		if ( it != flowMap_.end() ) {
			return (*it).second;
		} else {
			return NULL;
		}
	};

	int getCurrentFlowId() {
		return currentFlowId_;
	};

	const TipFlowPtr getCurrentTipFlow() {
		return currentFlow_;	
	};

	const map<int, TipFlowPtr>& getFlowMap() {
		return flowMap_;
	};

private:
	std::map<int ,TipFlowPtr> flowMap_;
	int currentFlowId_;
	TipFlowPtr currentFlow_;
};
typedef IceUtil::Handle<FlowIndex> FlowIndexPtr;

class QuestData: virtual public Ice::Object {
public:
	QuestData():
		userid_(0),
		flowid_(0),
		tip_(NULL),
		tipPos_(0),
		showCount_(0) {
	};

	QuestData(int userid, int flowid, TipPtr& tip, int tipPos, int showCount):
		userid_(userid),
		flowid_(flowid),
		tip_(tip),
		tipPos_(tipPos),
		showCount_(showCount) {
	};

        TipPtr getQuestData();
	
	/* @brief	用户的流程滚动到当前tip的下一个计数(计数滚动到0时就滚动到下一tip)
	 */
	void nextCount();

	/* @brief	用户的流程滚动到下一个tip
	 */
	void nextData();

	/* @biref	响应url 
	 */
	void responseUrl(const string& url);

	string getTableName() const;

	bool deleteFromDB();

	int getFlowId(){
		return flowid_;
	};

	void setFlowId(int flowid) {
		flowid_ = flowid;
	};

	TipPtr getTip() {
		return tip_;
	};

	void setTip(TipPtr& tip) {
		tip_ = tip;
	};

	int getTipPos(){
		return tipPos_;	
	};

	void setTipPos(int tipPos) {
		tipPos_ = tipPos;
	};

	int getShowCount(){
		return showCount_;
	};

	void setShowCount(int showCount) {
		showCount_ = showCount;
	};

private:
	/* @brief	滚动到下一服务(不带锁, 不可对外使用)
	 */	
	void nextDataRoll();

	IceUtil::RWRecMutex mutexTip_;
	IceUtil::Monitor<IceUtil::Mutex> mutexCount_;

	int userid_;
	int flowid_;		//当前流程Id
	TipPtr tip_;		//当前任务
	int tipPos_;		//当前任务索引
	int showCount_;		//当前任务还允许显示的次数
};

typedef IceUtil::Handle<QuestData> QuestDataPtr;

//===================QuestCacheI 主类==========================
class QuestCacheI : virtual public QuestCache,
			public MyUtil::Singleton<QuestCacheI> {
public:
	void initialize();

        /**
         * @brief       为一个新用户开辟一个新的任务流程
         * @param       userid  用户id
         **/
        virtual void newFlow(int userid, const Ice::Current& crt=Ice::Current());

         /** 
         * @brief       更新任务列表    
         **/
        virtual void renewTips(const Ice::Current& crt=Ice::Current());

        /**
         * @brief       更新任务流程
         **/
        virtual void renewFlow(const Ice::Current& crt=Ice::Current());

       /**
         * @brief       显示给用户当前应该显示给他/她的任务
         * @param       用户id
         * @return      当前应显示的用户任务
         **/
        virtual TipPtr show(int userId, const Ice::Current& crt=Ice::Current());

        /**
         * @brief       用户访问了一个任务
         * @param       tipid   被访问的任务id
         * @param       userid  用户id
         **/
        virtual void click(int tipid, int userId, const Ice::Current& crt=Ice::Current());

	/**
	 * @brief	捕获url并作相应处理
	 * @param	userid	用户id
	 * @param	url	捕获的url
	 **/
	virtual void responseUrl(Ice::Int userId, const std::string& url, const Ice::Current& crt=Ice::Current());

        /**
         * @brief       显示所有Tips
         * @return      按id递增排列的所有Tips
         **/
        virtual TipSeq listTips(const Ice::Current& crt=Ice::Current());

        /**
         * @brief       显示所有Flow
         * @return      按id递增排列的所有flows (最后一个为当前使用的)
         **/
        virtual TipFlowSeq listFlows(const Ice::Current& crt=Ice::Current());

	const TipIndexPtr& getTipIndex() {
		IceUtil::RWRecMutex::RLock lock(indexMutex_);
		return tipIndex_;
	}

	void setTipIndex(TipIndexPtr& tipIndex) {
		IceUtil::RWRecMutex::WLock lock(indexMutex_);
		tipIndex_ = tipIndex;
	}

	const FlowIndexPtr& getFlowIndex() {
		IceUtil::RWRecMutex::RLock lock(indexMutex_);
		return flowIndex_;
	}

	void setFlowIndex(FlowIndexPtr& flowIndex) {
		IceUtil::RWRecMutex::WLock lock(indexMutex_);
		flowIndex_ = flowIndex;
	}

private:
	friend class MyUtil::Singleton<QuestCacheI>;
	QuestCacheI(){};

	/**
	 * @brief	locate一条QuestData记录
	 * @param	userid	用户id
	 * @param	用户对应的QuestData记录, 若db中没有记录则为null
	 **/
	QuestDataPtr locate(int userid);

	/**
	 * @brief	find一条QuestData记录
	 * @param	userid	用户id
	 * @param	用户对应的QuestData记录, 若cache中没有记录则为null
	 **/
	QuestDataPtr find(int userid);

	TipIndexPtr tipIndex_;
	FlowIndexPtr flowIndex_;

	IceUtil::Mutex mutex_;
	std::vector<pair<int,std::string> > actions_;

	IceUtil::RWRecMutex indexMutex_;
};

//====================Factory部分==========================
class QuestCacheFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(int userId);
};

//====================ResultHandler部分=====================
class QuestCacheHandler :  public com::xiaonei::xce::ResultHandler {
public:
        QuestCacheHandler(QuestDataPtr& questData, bool& getflag):
		questData_(questData),
		getflag_(getflag)  {
		MCE_DEBUG("[QuestCacheHandler::QuestCacheHandler] questData_" << questData_);
	};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        QuestDataPtr& questData_;
	bool& getflag_;

};

//====================TipHandler部分========================
class TipHandler :  public com::xiaonei::xce::ResultHandler {
public:
        TipHandler(TipIndexPtr& tipIndex):
		tipIndex_(tipIndex) {
	};
        virtual bool handle(mysqlpp::Row& row) const;
private:
	TipIndexPtr& tipIndex_;
};


//====================Flow部分==============================
class FlowHandler :  public com::xiaonei::xce::ResultHandler {
public:
        FlowHandler(map<int, TipFlowPtr>& flowMap):
		flowMap_(flowMap) {
	};
        virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, TipFlowPtr>& flowMap_;
};

class ActionTask : virtual public MyUtil::Task {
public:
	ActionTask(std::vector<pair<int,std::string> >& id2Url){
		id2Url_.swap(id2Url);
	};
	virtual void handle();
private:
	std::vector<pair<int, std::string> > id2Url_;
};

};
};

#endif
