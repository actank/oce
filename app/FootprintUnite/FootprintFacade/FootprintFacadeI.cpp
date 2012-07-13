#include "FootprintFacadeI.h"
#include "util/cpp/String.h"
#include "HomeFootprintAdapter.h"
#include "ConnectionQuery.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "util/cpp/IpAddr.hpp"
#include "UtilCxx/src/GlobalId.h"
#include "Util.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "VisitFootprintAdapter.h"
#include "LoveFootprintByIntervieweeAdapter.h"
#include "LoveFootprintByInterviewerAdapter.h"
#include "ActiveTrackAdapter.h"
#include <Monitor/client/service/ServiceMonitor.h>


using namespace xce::serverstate;
using namespace MyUtil;
using namespace xce::footprint;

using com::xiaonei::xce::ConnectionQuery;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::CDbRServer;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;
using std::vector;
using std::string;
using std::map;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service= ServiceI::instance();
	//service.registerObjectCache(STEP, "S", new SyncServantFactory(new FootprintFacadeFactoryI));
	service.registerObjectCache(VISITORMAP, "VL", new SyncServantFactory(new VisitorMapFactoryI));

	service.registerObjectCache(INTERVIEWEESEQ, "IM", new SyncServantFactory(new IntervieweeSeqFactoryI));

	TaskManager::instance().config(TASK_LEVEL_BATCH, ThreadPoolConfig(0, 100));
	TaskManager::instance().config(TASK_LEVEL_REMOVE, ThreadPoolConfig(1, 1));
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();	
	int tableCount = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
	string privacy= props->getPropertyWithDefault("Service." + service.getName() + ".Privacy", "false");
	string loggerTopCategory = props->getPropertyWithDefault("Service." + service.getName() + ".LoggerTopCategory", "ActiveTrack");

	service.getAdapter()->add(new FootprintFacadeManagerI, service.createIdentity("FFM",	""));
	ActiveTrack::instance().initialize();

        int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FootprintFacade.Mod", 10);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FootprintFacade.Interval", 5);

        string ctrEndpoints = service.getCommunicator()->getProperties()
                ->getPropertyWithDefault("Footprint.ControllerEndpoint", "ControllerFootprintFacade");

        ServerStateSubscriber::instance().initialize(ctrEndpoints,
                       &FootprintFacadeManagerI::instance(), mod, interval);

	MCE_DEBUG("[MyUtil::initialize] tableCount:" << tableCount << " loggerTopCategory: " << loggerTopCategory
			<< " privacy: " << privacy);
	//统计log
	ServiceMonitorManager::instance().start();
}

/*
//------------------------FootprintFacadeFactoryI---------------------------------------
Ice::ObjectPtr FootprintFacadeFactoryI::create(Ice::Int id) {
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr result = new Step;
	return result;
}
*/
Ice::ObjectPtr VisitorMapFactoryI::create(Ice::Int id) {
	VisitorMapPtr result = new VisitorMap;
	return result; 
}

Ice::ObjectPtr IntervieweeSeqFactoryI::create(Ice::Int id) {
	IntervieweeSeqPtr result = new IntervieweeSeq;
	return result; 
}
//---------------------------------------------------------------------------
FootprintFacadeManagerI::FootprintFacadeManagerI(){
		filter_list_.resize(4);
}
/*
FootprintFacadeManagerI::FootprintFacadeManagerI(int tableCount,const XceLoggerPtr& xce_logger, string loggerTopCategory,string privacy)
	: _privacy(privacy),_tableCount(tableCount), _xce_logger(xce_logger) {
		_loggerCategories.push_back(loggerTopCategory);
		_loggerCategories.push_back("action");
		_loggerCategories.push_back("Footprint");
		filter_list_.resize(4);
}
*/
HomeInfoPtr FootprintFacadeManagerI::visitAndGet(const FootprintInfoPtr& footprint, const Ice::Current& current) {

	HomeInfoPtr result = NULL;
	ostringstream oss;
	oss << " owner: " << footprint->owner << " visitor: " << footprint->visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);


	/* ostringstream inMsg;
	inMsg << "FootprintFacadeManagerI::visitHomepage [owner=" << footprint->owner << " guest="<< footprint->visitor 
		<< " ip=" << current.con->toString() << "]";
	TimeCost tc = TimeCost::create(inMsg.str()); */

	if ( ! footprint ) {
		MCE_WARN("FootprintFacadeManagerI::visitAndGet] footprint it null");
		result = new HomeInfo; 
		result->visitcount = 0;
		return result;
	}

	//  step计算
	if ( !canPassList(footprint) ) {
		MCE_INFO("[FootprintFacadeManagerI::visitAndGet] blocked visitor: "<<footprint->visitor<<" owner: "<<footprint->owner);
		result = HomeFootprintAdapter::instance().getAll(footprint->owner, 0, 9, 0);
		return result;
	}

	// 踩脚印
	Ice::Context ctx;
	//ctx["nowriteviewcount"]="true";
	footprint->timestamp = time(NULL);
	result = HomeFootprintAdapter::instance().visitAndGet(footprint, 0,9,0,ctx);
/*
	xce::fptn::FootprintPtr fpt = new xce::fptn::Footprint;
	fpt->host = footprint->owner;
	fpt->tinyinfo = new xce::fptn::FootprintTiny;
	fpt->tinyinfo->guest = footprint->visitor;
	fpt->tinyinfo->timestamp = footprint->timestamp;
*/
//	xce::fptn::adapter::FootprintNAdapter::instance().visitHomepageOneway(fpt, current.ctx);


	return result;
}

void FootprintFacadeManagerI::visitBatch(const FootprintInfoSeq& fpts, const Ice::Current& current) {

	ostringstream oss;
	oss << " called: " << fpts.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	for (FootprintInfoSeq::const_iterator it = fpts.begin(); it != fpts.end(); ++it ) {
		// 踩脚印
		visit(*it, current);
	}

	if ( fpts.size() > 0 ) {
		// 异步批量踩脚印
//		TaskManager::instance().execute(new BatchVisitTask(fpts, *this, current));
	}
}

void FootprintFacadeManagerI::visit(const FootprintInfoPtr& footprint, const Ice::Current& current) {

	ostringstream oss;
	oss << " owner: " << footprint->owner << " visitor: " << footprint->visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	footprint->timestamp = time(NULL);
	MCE_DEBUG("[FootprintFacade::visit] owner: "<<footprint->owner<<" visitor: "<<footprint->visitor);
	if ( ! footprint ) {
		MCE_WARN("FootprintFacadeManagerI::visit] footprint it null");
		return;
	}	
	if(canPassList(footprint))
		dispatch(footprint);

	return ;
}

void FootprintFacadeManagerI::visitByIp(const IpFootprintInfoPtr& ipfootprint, const Ice::Current& current) {

	ostringstream oss;
	oss << " owner: " << ipfootprint->owner << " visitor: " << ipfootprint->visitorIp;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ipfootprint->timestamp = time(NULL);
	MCE_DEBUG("[FootprintFacade::visitByIp] owner: " << ipfootprint->owner << " visitor: " << ipfootprint->visitorIp);
	if (!ipfootprint) {
		MCE_WARN("FootprintFacadeManagerI::visitByIp] ipfootprint it null");
		return;
	}

	//变换成统一脚印数据，因为当前数据库还不能存字符串
	FootprintInfoPtr footprint = new FootprintInfo;
	footprint->gid = ipfootprint->gid;
	footprint->owner = ipfootprint->owner;
	footprint->visitor = -MyUtil::IpAddr::translate(ipfootprint->visitorIp.c_str());
	footprint->timestamp = ipfootprint->timestamp;

	//走通用的逻辑，包括过滤和有效重复时间.(guanghe.ge 2011-4-6	)
	if(canPassList(footprint))
		dispatch(footprint);

	return ;
}

/*
bool FootprintFacadeManagerI::stepFootprint(const FootprintInfoPtr& footprint, const map<string, string>& ctx) {
	
	StepPtr step = getStep(footprint->visitor);
	step->add(footprint->timestamp/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(footprint->visitor, footprint->owner);
	if (count >= OVER_COUNT) {
		step->incMark();
	} else {
		step->decMark();
	}

	if (step->getMark() != 0) {
		MCE_INFO("Spam refuse user's footprint: " << footprint->visitor);
		return false;
	}

	if (_privacy=="true"){
		if (!canSend(footprint)) {
			MCE_INFO(footprint->visitor << " refuse leaving footprint for " << footprint->owner);
			ostringstream sql;
			ostringstream table;
			table << "privacy_footprint_send_" << (footprint->owner%_tableCount);
			sql << "INSERT IGNORE INTO " << table.str() << " (owner, guest, guest_name, guest_head, time) values ('" << footprint->owner << "','" << footprint->visitor << "','" << "" << "','" << "" << "','" << Date(footprint->timestamp/1000).str() << "')";
			ConnectionQuery conn("footprint", CDbWServer, table.str());
			conn.doInsert(sql.str());
			return false;
		}

		if (!canRecv(footprint)) {
			MCE_INFO(footprint->owner << " refuse the footprint of " << footprint->visitor);
			ostringstream sql;
			ostringstream table;
			table << "privacy_footprint_recv_" << (footprint->owner%_tableCount);
			sql << "INSERT IGNORE INTO " << table.str() << " (owner, guest, guest_name, guest_head, time) values ('" << footprint->owner << "','" << footprint->visitor << "','" << "" << "','" << "" << "','" << Date(footprint->timestamp/1000).str() << "')";
			ConnectionQuery conn("footprint", CDbWServer, table.str());
			conn.doInsert(sql.str());
			return false;
		}
		map<string, string>::const_iterator iterator=ctx.find("friendstatus");	
		if (iterator == ctx.end() || iterator->second=="no") {
			if (!canStranger(footprint)) {
				MCE_DEBUG(footprint->owner<< " refuse the footprint of stranger, " << footprint->visitor);
				return false;
			}
		}
	}
	return true;
}
*/
void FootprintFacadeManagerI::contral(const MyUtil::Str2IntMap& cmd,
		const Ice::Current& current) {
	ostringstream oss;
	oss << " ";
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	for (MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++) {
		/*if ("OVER_COUNT"== it->first)
			OVER_COUNT = it->second;
		else if ("MAX_MARK" == it->first)
			MAX_MARK = it->second;
		else if ("DEC_MARK_TIMESLOT" == it->first)
			DEC_MARK_TIMESLOT = it->second;
		else if ("MAX_DEC_MARK" == it->first)
			MAX_DEC_MARK = it->second;
		else if ("ALPHA" == it->first)
			ALPHA = it->second;
		else if ("BETA" == it->first)
			BETA = it->second;
		else */
		IceUtil::Mutex::Lock lock(list_mutex_);
		if(it->first == "ADD_BLACK_VISITOR")
			filter_list_[BLACK_VISITOR].insert(it->second);
		else if(it->first == "ADD_WHITE_VISITOR")
			filter_list_[WHITE_VISITOR].insert(it->second);
		else if(it->first == "REMOVE_BLACK_VISITOR")
			filter_list_[BLACK_VISITOR].erase(it->second);
		else if(it->first == "REMOVE_WHITE_VISITOR")
			filter_list_[WHITE_VISITOR].erase(it->second);
		else if(it->first == "ADD_BLACK_OWNER")
			filter_list_[BLACK_OWNER].insert(it->second);
		else if(it->first == "ADD_WHITE_OWNER")
			filter_list_[WHITE_OWNER].insert(it->second);
		else if(it->first == "REMOVE_BLACK_OWNER")
			filter_list_[BLACK_OWNER].erase(it->second);
		else if(it->first == "REMOVE_WHITE_OWNER")
			filter_list_[WHITE_OWNER].erase(it->second);

		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
		MCE_INFO("[LIST SIZE] :"<<"BLACK VISITOR:"<<filter_list_[BLACK_VISITOR].size());
	}
}

//this interface just for store interview.
void FootprintFacadeManagerI::interview(const FootprintInfoPtr& footprint, const Ice::Current& current) {
	ostringstream oss;
	oss << " visitor: " << footprint->owner << " interview: " << footprint->visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	IntervieweeSeqPtr I = getIntervieweeSeq(footprint->owner);
	int now = time(NULL);
	//只保留一天数据
	if (now - I->getTime() > 24*60*60){
		MCE_INFO("[FootprintFacadeManagerI::interview] visitor " << footprint->owner << " release the old data!");
		I->clear();
	}
	I->add(footprint->visitor);
}

void FootprintFacadeManagerI::removeAllByVisitor(Ice::Int visitor, const Ice::Current& current) {
	ostringstream oss;
	oss << " visitor: " << visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	TaskManager::instance().execute(new RemoveByVisitorTask(visitor, *this, current));
}

/*
StepPtr FootprintFacadeManagerI::getStep(Ice::Int userId) {
	return ServiceI::instance().locateObject<StepPtr>(STEP, userId);
}*/
VisitorMapPtr FootprintFacadeManagerI::getVisitorMap(Ice::Int userId) {
	return ServiceI::instance().locateObject<VisitorMapPtr>(VISITORMAP, userId);
}

IntervieweeSeqPtr FootprintFacadeManagerI::getIntervieweeSeq(Ice::Int userId) {
	return ServiceI::instance().locateObject<IntervieweeSeqPtr>(INTERVIEWEESEQ, userId);
}
/*
bool FootprintFacadeManagerI::canSend(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_send_config WHERE owner=" << fpt->owner << " AND guest=" << fpt->visitor;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintSendConfigHandler(flag));
	return flag;
}

bool FootprintFacadeManagerI::canRecv(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_recv_config WHERE owner=" << fpt->owner << " AND guest=" << fpt->visitor;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintRecvConfigHandler(flag));
	return flag;
}

bool FootprintFacadeManagerI::canStranger(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_stranger_config WHERE id=" << fpt->owner;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintStrangerConfigHandler(flag));
	return flag;
}*/
// --------------------------------------------------------------------------
void BatchVisitTask::handle() {

	// 依次踩脚印
	//int visited = 0; 
	for (FootprintInfoSeq::const_iterator it = _fpts.begin(); it != _fpts.end(); ++it ) {
		// 踩脚印
		 _filter.visit(*it, _crt);
	}
	MCE_INFO("[BatchVisitTask::handle]" << _fpts.size() << " footprint(s)");
}

void RemoveByVisitorTask::handle() {
	vector<int> interviewees = _filter.getIntervieweeSeq(_visitor)->getInterviewees();
	for (vector<int>::iterator ii = interviewees.begin(); ii != interviewees.end(); ++ii) {
		MCE_DEBUG("RemoveByVisitorTask::handle, visitor " << _visitor << " interviewee " << *ii);
		FootprintInfoPtr info = new FootprintInfo;
		info->owner = (*ii);
		info->visitor = _visitor;
		info->timestamp = time(NULL);
		Ice::Context ctx;
		HomeFootprintAdapter::instance().remove(info, ctx);
	}
}

/*
//---------------------------------------------------------------------------
FootprintSendConfigHandler::FootprintSendConfigHandler(bool &flag) : _flag(flag) {
}

FootprintSendConfigHandler::~FootprintSendConfigHandler() {
}

bool FootprintSendConfigHandler::handle(mysqlpp::Row& res) const {
	if ((int)res["type"]==0) {
		_flag=false;
	}
	return true;
}

//---------------------------------------------------------------------------
FootprintRecvConfigHandler::FootprintRecvConfigHandler(bool &flag) : _flag(flag) {
}

FootprintRecvConfigHandler::~FootprintRecvConfigHandler() {
}

bool FootprintRecvConfigHandler::handle(mysqlpp::Row& res) const {
	if ((int)res["type"]==0){
		_flag=false;
	}
	return true;
}

//---------------------------------------------------------------------------
FootprintStrangerConfigHandler::FootprintStrangerConfigHandler(bool &flag) : _flag(flag) {
}

FootprintStrangerConfigHandler::~FootprintStrangerConfigHandler() {
}

bool FootprintStrangerConfigHandler::handle(mysqlpp::Row& res) const {
	if ((int)res["type"]==0){
		_flag=false;
	}
	return true;
}
*/
bool FootprintFacadeManagerI::isValidVisitor(const FootprintInfoPtr& fpt){
	VisitorMapPtr v = getVisitorMap(fpt->owner);
	return v->add(fpt->visitor);
}
void sendActionLog(const string& type,const FootprintInfoPtr& fpt){
        ostringstream object;
       	object << fpt->visitor;
       	ActiveTrack::instance().action(fpt->owner, type, "ADD_FOOTPRINT", object.str());

}
void FootprintFacadeManagerI::dispatchProfile(const FootprintInfoPtr& fpt){
	map<string,string> m;
//	m["nowriteviewcount"]="true";
	MCE_INFO("[dispatch profile] owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" globalId: "<<fpt->gid);

/*
        xce::fptn::FootprintPtr footprint = new xce::fptn::Footprint;
        footprint->host = fpt->owner;
        footprint->tinyinfo = new xce::fptn::FootprintTiny;
        footprint->tinyinfo->guest = fpt->visitor;
        footprint->tinyinfo->timestamp = fpt->timestamp;

       // xce::fptn::adapter::FootprintNAdapter::instance().visitHomepageOneway(footprint, m);
*/
	sendActionLog("PROFILE", fpt);

	HomeFootprintAdapter::instance().visit(fpt,m);	
	VisitFootprintAdapter::instance().visit(fpt);
	//MCE_INFO("[dispatch profile] visitor: "<<fpt->visitor<<" visit owner: "<<fpt->owner<<" too fast");
}
void FootprintFacadeManagerI::dispatchAlbum(const FootprintInfoPtr& fpt){
	map<string,string> m;
//	m["nowriteviewcount"]="true";
	MCE_INFO("[dispatch album] owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" globalId: "<<fpt->gid);
	if(isValidVisitor(fpt)){
		sendActionLog("ALBUM", fpt);
		HomeFootprintAdapter::instance().visit(fpt,m);
		VisitFootprintAdapter::instance().visit(fpt);
	}else{
		MCE_INFO("[dispatch album] visitor: "<<fpt->visitor<<" visit owner: "<<fpt->owner<<" too fast");
	}
}

void FootprintFacadeManagerI::dispatchBlog(const FootprintInfoPtr& fpt){
	map<string,string> m;
//	m["nowriteviewcount"]="true";
	MCE_INFO("[dispatch blog] owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" globalId: "<<fpt->gid);
	if(isValidVisitor(fpt)){
		sendActionLog("BLOG", fpt);
		HomeFootprintAdapter::instance().visit(fpt,m);	
		VisitFootprintAdapter::instance().visit(fpt);
	}else{
		MCE_INFO("[dispatch blog] visitor: "<<fpt->visitor<<" visit owner: "<<fpt->owner<<" too fast");
	}
	//BlogFootprintAdapter::instance().visitor(fpt);
}
void FootprintFacadeManagerI::dispatchPhoto(const FootprintInfoPtr& fpt){
	map<string,string> m;
//	m["nowriteviewcount"]="true";	
	MCE_INFO("[dispatch photo] owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" globalId: "<<fpt->gid);
	if(isValidVisitor(fpt)){
		sendActionLog("PHOTO", fpt);
		HomeFootprintAdapter::instance().visit(fpt,m);	
		VisitFootprintAdapter::instance().visit(fpt);
	}else{
		MCE_INFO("[dispatch photo] visitor: "<<fpt->visitor<<" visit owner: "<<fpt->owner<<" too fast");
	}
}

void FootprintFacadeManagerI::dispatchLove(const FootprintInfoPtr& fpt){
	map<string,string> m;
//	m["nowriteviewcount"]="true";	
	MCE_INFO("[dispatch love] owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" globalId: "<<fpt->gid);
	if(isValidVisitor(fpt)){
		sendActionLog("LOVE", fpt);
		LoveFootprintByIntervieweeAdapter::instance().visit(fpt);	
		LoveFootprintByInterviewerAdapter::instance().visit(fpt);	
	}else{
		MCE_INFO("[dispatch love] visitor: "<<fpt->visitor<<" visit owner: "<<fpt->owner<<" too fast");
	}
}

void FootprintFacadeManagerI::dispatch(const FootprintInfoPtr& fpt){
	GlobalIdPtr p =	GlobalIdTransfer::get(fpt->gid);
	switch(p->type){
		case PROFILE:
			dispatchProfile(fpt);break; 
		case PHOTO:
			dispatchPhoto(fpt);break;
		case BLOG:
			dispatchBlog(fpt);break;
		case ALBUM:
			dispatchAlbum(fpt);break;
		case LOVE:
			dispatchLove(fpt);break;
	}; 
}
bool FootprintFacadeManagerI::canPassList(const FootprintInfoPtr& fpt){

	bool canPass = true;
	if(fpt->owner==fpt->visitor)
		return false;

	IceUtil::Mutex::Lock lock(list_mutex_);

	if(filter_list_[BLACK_VISITOR].size() && filter_list_[BLACK_VISITOR].find(fpt->visitor)!=filter_list_[BLACK_VISITOR].end()){
		canPass = filter_list_[WHITE_VISITOR].size() && filter_list_[WHITE_VISITOR].find(fpt->visitor)!=filter_list_[WHITE_VISITOR].end();
		MCE_INFO("[canPassList] find black visitor list: "<<fpt->visitor);
	}
	if(!canPass)
		return canPass;

	if(filter_list_[BLACK_OWNER].size() && filter_list_[BLACK_OWNER].find(fpt->visitor)!=filter_list_[BLACK_OWNER].end()){
		canPass = filter_list_[WHITE_OWNER].size() && filter_list_[WHITE_OWNER].find(fpt->visitor)!=filter_list_[WHITE_OWNER].end();
	}
	return canPass;
}
bool VisitorMap::add(int visitor){
        bool isValid = false;
	int now = time(NULL);
	IceUtil::Mutex::Lock lock(mutex_);

        map<int,int>::iterator it = id2Time_.find(visitor);
        if(it!=id2Time_.end()){
                if(now - it->second > 60*10){
                        it->second = now;
                        isValid = true;
                }else{
                        isValid = false;
                }
        }else {
                isValid = true;
                id2Time_.insert(make_pair<int,int>(visitor,now));
        }
	return isValid;
}

bool IntervieweeSeq::add(int interviewee){
	bool isValid = true;
	IceUtil::Mutex::Lock lock(mutex_);
	vector<int>::iterator it = lower_bound(interviewees_.begin(), interviewees_.end(), interviewee);

	if (it != interviewees_.end() && (*it) == interviewee) {
		MCE_DEBUG("IntervieweeSeq::add, interviewee " << interviewee << " is already in cache! ");
	} else {
		interviewees_.insert(it, interviewee);
		MCE_DEBUG("IntervieweeSeq::add, interviewee " << interviewee << " SeqSize " << interviewees_.size());
	}

	return isValid;
}

int IntervieweeSeq::getTime() {
	IceUtil::Mutex::Lock lock(mutex_);
	return time_;
}

void IntervieweeSeq::clear() {
	IceUtil::Mutex::Lock lock(mutex_);
	interviewees_.clear();
	time_ = time(NULL);
}

vector<int> IntervieweeSeq::getInterviewees() {
	IceUtil::Mutex::Lock lock(mutex_);
	vector<int> result;
	result.reserve(interviewees_.size());
	result.swap(interviewees_);
	return result;
}
