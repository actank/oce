#include "QuestCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "app/Quest/share/QuestUtil.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::quest;
using namespace xce::serverstate;
using namespace xce::clusterstate;

// ================ MyUtil::initialize =====================
void MyUtil::initialize() {

        ServiceI& service = ServiceI::instance();
        service.registerObjectCache(QUEST_CACHE, "QE", new QuestCacheFactoryI);
        service.getAdapter()->add(&QuestCacheI::instance(), service.createIdentity("M", ""));
	QuestCacheI::instance().initialize();

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("QuestCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("QuestCache.Interval", 5);
        // 加入向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerQuestCache",
                        &QuestCacheI::instance(), mod, interval);
}

//===================QuestData类============================
TipPtr QuestData::getQuestData(){

	// 返回当前的Tip
	{
		IceUtil::RWRecMutex::RLock lock(mutexTip_);
		if ( tip_ ) {
			return tip_;
		}
	}	
	MCE_WARN("[QuestData::getQuestData] Invalid Tip");
	return ::xce::quest::emptyTip();
}

void QuestData::nextCount(){

	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutexCount_);
		MCE_DEBUG("[QuestData::nextCount] user " << userid_ << " tip " << tip_->id << " count " << showCount_);
		showCount_--;
		if( showCount_ > 0) {
			return;
		}
	}
	IceUtil::RWRecMutex::WLock lock(mutexTip_);
	nextDataRoll();	// 当showCount_<=0的时候就滚动到下一轮 	
}

void QuestData::nextData() {

	IceUtil::RWRecMutex::WLock lock(mutexTip_);
	nextDataRoll();
}

void QuestData::responseUrl(const string& url) {

	{
		IceUtil::RWRecMutex::RWRecMutex::RLock lock(mutexTip_);
		if ( ! ::xce::quest::startsWith(url, tip_->url) ) {
			return;
		}
	}
	{
		IceUtil::RWRecMutex::RWRecMutex::WLock lock(mutexTip_);
		nextDataRoll();
	}
}

string QuestData::getTableName() const {
	ostringstream result;
	result << TABLE_PROCESS << (userid_ % TABLE_PROCESS_CLUSTER);
	return result.str();
}

void QuestData::nextDataRoll() {
	// 这个函数里面不加锁, 调用前必须保证写锁

	MCE_DEBUG("QuestData::nextDataRoll");

	// 获取FlowIndex
	FlowIndexPtr flowIndex = QuestCacheI::instance().getFlowIndex();
	TipIndexPtr tipIndex = QuestCacheI::instance().getTipIndex();

	TipFlowPtr flow = flowIndex->getFlow(flowid_);
	if ( flow ) {
		std::vector<TipOrder>& flows = flow->seq;
		tipPos_++;
		if(tipPos_ < int(flows.size()) ) {
      			int tipid = flows.at(tipPos_).tipId;
			TipPtr tip = tipIndex->get(tipid);
			if ( ! tip ) {
				MCE_WARN("[QuestData::nextData] next tip with id " << tipid << " is invalid");
				ServiceI::instance().removeObject(QUEST_CACHE, userid_);
				deleteFromDB();
				return;
			}
			MCE_INFO("[QuestData::nextDataRoll] roll to next tip : " << tip->id);

			// 更新信息
			showCount_ = flows.at(tipPos_).showCount;
			tip_ = tip;

			//Update数据库（异步）
			string table = getTableName();
			Statement sqlAdd;
			sqlAdd << "REPLACE INTO " << table << "(" 
				<< FIELD_PROCESS_USERID << ", "	<< FIELD_PROCESS_FLOWID << ", "
				<< FIELD_PROCESS_CURRENTPOS << ", " << FIELD_PROCESS_REMAINCOUNT << ")" 
				<< "  values" << " (" << userid_ << "," << flowid_ << ","  << tipPos_ << ","  << showCount_ << ")";
			QueryRunner(DESC_QUEST_PROCESS, CDbWServer, table).schedule(sqlAdd);
		} else {
			//全部任务完成，从Cache中移除
			ServiceI::instance().removeObject(QUEST_CACHE, userid_);
			//从数据库中清除（异步）
			deleteFromDB();
		}
	} else {
		//不存在该任务流
		//从cache中移除
		ServiceI::instance().removeObject(QUEST_CACHE, userid_);
		//从数据库中清除（异步）
               	deleteFromDB();
       }		
}

bool QuestData::deleteFromDB() {
	string table = getTableName();
        Statement sqlDelete;
        sqlDelete << " DELETE FROM " << table << " WHERE USERID = " << userid_;
        QueryRunner(DESC_QUEST_PROCESS, CDbWServer, table).schedule(sqlDelete);
	
	return true;
}

//====================QuestCacheI类=========================
void QuestCacheI::initialize() {

	renewTips();
	renewFlow();
}

void QuestCacheI::newFlow(int userid, const Ice::Current& crt){

	MCE_INFO("[QuestCacheI::newFlow] new flow for " << userid);
	const FlowIndexPtr flowIndex = getFlowIndex();
	const TipIndexPtr tipIndex = getTipIndex();
	const TipFlowPtr currentFlow = flowIndex->getCurrentTipFlow();
	if ( currentFlow->seq.size() <= 0 ) {
		// 如果现在没有任务列表在, 就不加入
		return;
	}
		
	TipOrder tipOrder = currentFlow->seq.front();
	int flowId = flowIndex->getCurrentFlowId();
	int tipId = tipOrder.tipId;
	int tipPos = 0;
	int showCount = tipOrder.showCount;
	TipPtr tip = tipIndex->get(tipId);
	if ( !tip ) {
		MCE_WARN("[QuestCacheI::newFlow] 1st tip with id " << tipId << " is valid, new flow failed");
		return;
	}

	// 向cache加入新的QuestData
	QuestDataPtr data = new QuestData(userid, flowId, tip, tipPos, showCount);
	ServiceI::instance().addObject(data, QUEST_CACHE, userid);

	// 写入DB
        ostringstream tableTmp;
        tableTmp << TABLE_PROCESS << (userid % TABLE_PROCESS_CLUSTER);
        string table = tableTmp.str();
	Statement sqlAdd;
	sqlAdd << "REPLACE INTO " << table << "(" << FIELD_PROCESS_USERID << ","
		<< FIELD_PROCESS_FLOWID << ","
		<< FIELD_PROCESS_CURRENTPOS << ","
		<< FIELD_PROCESS_REMAINCOUNT << ")" 
		<< " values" << " ("<< userid << "," << flowId << ","  << tipPos << ","  << showCount  <<")";
	QueryRunner(DESC_QUEST_PROCESS, CDbWServer, table).schedule(sqlAdd);
}

void QuestCacheI::renewTips(const Ice::Current& crt){
        
	MCE_INFO("[QuestCacheI::renewTips] load tips from DB " );

	Statement sql;
	sql << "SELECT * FROM " << TABLE_TIP;
	TipIndexPtr newTipIndex = new TipIndex;

	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbRServer).query(sql, TipHandler(newTipIndex));
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[QuestCacheI::renewTips] renew from sql mysqlpp::exception : " << e.what());
		return;
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestCacheI::renewTips] renew from sql Ice::Exception : " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[QuestCacheI::renewTips] renew from sql Unknown Exception ");
		return;
	}

	// 将TipIndex替换为新的
	if ( newTipIndex ) {
		setTipIndex(newTipIndex);
	}
        MCE_DEBUG("[QuestCacheI::renewTips]Loading  done.");	
}

void QuestCacheI::renewFlow(const Ice::Current& crt){

	MCE_INFO("[QuestCacheI::renewFlow] load flow from DB " );

	map<int, TipFlowPtr> flowMap;
	Statement sql;
	sql << "SELECT * FROM " << TABLE_FLOW;
	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbRServer).query(sql, FlowHandler(flowMap));
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestCacheI::renewFlow] renew from sql Ice::Exception : " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[QuestCacheI::renewFlow] renew from sql Ice::Exception ");
		return;
        }        
        MCE_DEBUG("[QuestCacheI::renewFlow]Loading  done.");

	// 将FlowIndex替换成新的
	FlowIndexPtr newFlowIndex = new FlowIndex(flowMap);
	if ( newFlowIndex ) {
		setFlowIndex(newFlowIndex);
	} else {
		MCE_WARN("[QuestCacheI::renewFlow] new flowIndex is invalid. DONOT renew flowIndex");
	}
}

TipPtr QuestCacheI::show(int userid, const Ice::Current& crt){

	QuestDataPtr questData = locate(userid);
	if ( ! questData ) {
		MCE_INFO("[QuestCacheI::show] " << "userid: "<< userid << " empty Tip");

	return emptyTip();
	} else {
		TipPtr result = questData->getQuestData();
		questData->nextCount();
		MCE_INFO("[QuestCacheI::show] " << "userid: "<< userid << " tip: " << result->id );
		return result;
	}
}

void QuestCacheI::click(int tipId, int userid, const Ice::Current& crt){

	MCE_INFO("[QuestCacheI::click] " << "userid: " << userid <<" tipId: " << tipId);
	QuestDataPtr questData = locate(userid);
	if ( questData ) {
		if ( questData->getTip()->id == tipId ) {
			questData->nextData();
		}
	}
	// locate不到这个用户的questData就不用操作
}

void QuestCacheI::responseUrl(Ice::Int userid, const std::string& url, const Ice::Current& crt) {

	MCE_DEBUG("[QuestCacheI::responseUrl] userid: " << userid << " url : " << url);
	IceUtil::Mutex::Lock lock(mutex_);
	actions_.push_back(make_pair<int,string>(userid,url));
	if(actions_.size()>=1000){
		MCE_DEBUG("[QuestCacheI::ActionTask]: "<<actions_.size());
		TaskManager::instance().execute(new ActionTask(actions_));     
	}
}

TipSeq QuestCacheI::listTips(const Ice::Current& crt){

	TipSeq tipSeq;
	TipIndexPtr tipIndex = getTipIndex();
	const TipMap& tipMap = tipIndex->getTipMap();
	for(TipMap::const_iterator itTip = tipMap.begin(); itTip != tipMap.end(); ++itTip) {
		TipPtr tip = itTip->second;
		tipSeq.push_back(tip);
	}
	return tipSeq;
}

TipFlowSeq QuestCacheI::listFlows(const Ice::Current& crt){

	TipFlowSeq tipFlowSeq;
	FlowIndexPtr flowIndex = getFlowIndex();
	const map<int, TipFlowPtr>& flowMap = flowIndex->getFlowMap();
	for(std::map<int ,TipFlowPtr>::const_iterator itFlow = flowMap.begin(); itFlow != flowMap.end(); ++itFlow){
		TipFlowPtr flow = itFlow->second;
		tipFlowSeq.push_back(flow);
	}
	return tipFlowSeq;
}

QuestDataPtr QuestCacheI::locate(int userid) {
	
	QuestDataPtr result = ServiceI::instance().locateObject<QuestDataPtr>(QUEST_CACHE, userid);
	if ( result->getFlowId() < 0 ) {
		ServiceI::instance().removeObject(QUEST_CACHE, userid);
		return NULL;
	} else {
		return result;
	}
}

QuestDataPtr QuestCacheI::find(int userid) {
	return ServiceI::instance().findObject<QuestDataPtr>(QUEST_CACHE, userid);
}

//===================Factory部分============================
Ice::ObjectPtr QuestCacheFactoryI::create(int userid) {
 
        MCE_DEBUG("[QuestCacheFactoryI::create] Loading " << userid);
        QuestDataPtr result = new QuestData;
	bool getflag = false;
	Statement sql;
	ostringstream table;
	table << TABLE_PROCESS << (userid % TABLE_PROCESS_CLUSTER);
	sql << "SELECT * FROM " << table.str() << " WHERE " << FIELD_PROCESS_USERID  << " = " << userid;
	
	QueryRunner(DESC_QUEST_PROCESS, CDbRServer).query(sql, QuestCacheHandler(result, getflag));
        
        MCE_DEBUG("[QuestCacheFactoryI::create]Loading " << userid << " done.");
	if ( getflag ) {
	        return result;
	} else {
		result = new QuestData;
		result->setFlowId(-1);
		return result;
	}
}

//====================ResultHandler部分=====================
bool QuestCacheHandler::handle(mysqlpp::Row& row) const {

	int flowid = (int) row[FIELD_PROCESS_FLOWID.c_str()];
	int tipPos = (int) row[FIELD_PROCESS_CURRENTPOS.c_str()];
	int showCount = (int) row[FIELD_PROCESS_REMAINCOUNT.c_str()];

	FlowIndexPtr flowIndex = QuestCacheI::instance().getFlowIndex();
	TipIndexPtr tipIndex = QuestCacheI::instance().getTipIndex();
	if ( ! flowIndex ) {
		MCE_WARN("[QuestCacheHandler::handle] get FlowIndex failed");
		return false;
	}
	if ( ! tipIndex ) {
		MCE_WARN("[QuestCacheHandler::handle] get TipIndex failed");
		return false;
	}

	TipFlowPtr flow = flowIndex->getFlow(flowid);
	if ( ! flow ) {
		MCE_WARN("[QuestCacheHandler::handle] no flow with num : " << flowid);
		return false;
	}

	if ( (tipPos>=0)&&(int(flow->seq.size()) > tipPos) ) {
		int tipid = flow->seq[tipPos].tipId;
		TipPtr tip = tipIndex->get(tipid);
		if ( ! tip ) {
			MCE_WARN("[QuestCacheHandler::handle] tip with id " << tipid << " is invalid");
			return false;
		}
		questData_->setFlowId(flowid);
		questData_->setTipPos(tipPos);
		questData_->setTip(tip);
		questData_->setShowCount(showCount);	
		getflag_ = true;
	}

	return false;	// 每个用户只读1条记录
}

bool TipHandler::handle(mysqlpp::Row& row) const {

	int tipid = (int) row[FIELD_TIP_TIPID.c_str()];
	std::string title = (string) row[FIELD_TIP_TITLE.c_str()];
	std::string content = (string) row[FIELD_TIP_CONTENT.c_str()];
	std::string url = (string) row[FIELD_TIP_URL.c_str()];

	TipPtr tip = new Tip;
	tip->id = tipid;
	tip->title = title;
	tip->content = content;
	tip->url = url;

	// 加入这条tip
	tipIndex_->insert(tipid, tip);

	return true;
} 

bool FlowHandler::handle(mysqlpp::Row& row) const {
	
	int flowid = (int) row[FIELD_FLOW_FLOWID.c_str()];
	std::string tips = (string) row[FIELD_FLOW_TIPS.c_str()];	//tips=1-5;2-1;3-10;4-5
	
	std::vector<string> tipSeq = StrUtil::split(tips, ";");
	
	TipOrderSeq tipOrderSeq;
	TipFlowPtr tipFlow = new TipFlow;

	for(std::vector<string>::const_iterator it = tipSeq.begin(); it != tipSeq.end(); ++it){
		string tipTail = *it;	//tipTail=1-5....
		std::vector<string> tipTailSeq = StrUtil::split(tipTail, "-");
		
		TipOrder tipOrder;
		tipOrder.tipId = atoi(tipTailSeq.front().c_str());
		tipOrder.showCount = atoi(tipTailSeq.back().c_str());

		tipOrderSeq.push_back(tipOrder);
		
	}

	tipFlow->seq = tipOrderSeq;

	// 加入这条flow
	flowMap_.insert(make_pair<int, TipFlowPtr>(flowid, tipFlow));

	return true;
}


void ActionTask::handle() {
	
	for(size_t i=0;i<id2Url_.size();++i){
		QuestDataPtr quest = ServiceI::instance().findObject<QuestDataPtr>(QUEST_CACHE, id2Url_[i].first);
		if (! quest ) {
			return;
		}
		quest->responseUrl(id2Url_[i].second);
	}
}

