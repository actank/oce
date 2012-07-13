#include "FootprintFilterI.h"
#include "util/cpp/String.h"
#include "Step.h"
#include "OceCxxAdapter/src/HomeFootprintAdapter.h"
#include "DbCxxPool/src/ConnectionQuery.h"
#include "util/cpp/TimeCost.h"
#include "UtilCxx/src/GlobalId.h"
#include "Util.h"

using namespace MyUtil;
using namespace xce::footprint;
//using xce::fptn::adapter::FootprintNAdapter;
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
	service.registerObjectCache(STEP, "S", new SyncServantFactory(new FootprintFilterFactoryI));
	service.registerObjectCache(VISITORMAP, "VL", new SyncServantFactory(new VisitorMapFactoryI));

	TaskManager::instance().config(TASK_LEVEL_BATCH, ThreadPoolConfig(0, 100));

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();	
	int tableCount = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
	string privacy= props->getPropertyWithDefault("Service." + service.getName() + ".Privacy", "false");
	string loggerTopCategory = props->getPropertyWithDefault("Service." + service.getName() + ".LoggerTopCategory", "ActiveTrack");

	XceLoggerPtr xce_logger = XceLoggerAdapter::instance().getLogger();
	xce_logger->initialize();
	service.getAdapter()->add(new FootprintFilterManagerI(tableCount, xce_logger, loggerTopCategory,privacy), service.createIdentity("FFM",	""));
	MCE_DEBUG("[MyUtil::initialize] tableCount:" << tableCount << " loggerTopCategory: " << loggerTopCategory
			<< " privacy: " << privacy);
}

//------------------------FootprintFilterFactoryI---------------------------------------
Ice::ObjectPtr FootprintFilterFactoryI::create(Ice::Int id) {
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr result = new Step;
	return result;
}
Ice::ObjectPtr VisitorMapFactoryI::create(Ice::Int id) {
	VisitorMapPtr result = new VisitorMap;
	return result; 
}
//---------------------------------------------------------------------------
FootprintFilterManagerI::FootprintFilterManagerI(int tableCount,const XceLoggerPtr& xce_logger, string loggerTopCategory,string privacy)
	: _privacy(privacy),_tableCount(tableCount), _xce_logger(xce_logger) {
		_loggerCategories.push_back(loggerTopCategory);
		_loggerCategories.push_back("action");
		_loggerCategories.push_back("Footprint");
		filter_list_.resize(4);
}

HomeInfoPtr FootprintFilterManagerI::visitAndGet(const FootprintInfoPtr& footprint, const Ice::Current& current) {

	HomeInfoPtr result = NULL;
	/* ostringstream inMsg;
	inMsg << "FootprintFilterManagerI::visitHomepage [owner=" << footprint->owner << " guest="<< footprint->visitor 
		<< " ip=" << current.con->toString() << "]";
	TimeCost tc = TimeCost::create(inMsg.str()); */

	if ( ! footprint ) {
		MCE_WARN("FootprintFilterManagerI::visitHomepage] footprint it null");
		result = new HomeInfo; 
		result->visitcount = 0;
		return result;
	}

	//  step计算
	if ( ! stepFootprint(footprint, current.ctx) ) {
		result = HomeFootprintAdapter::instance().getAll(footprint->owner, 0, 6, 0);
		return result;
	}

	// 踩脚印
	result = HomeFootprintAdapter::instance().visitAndGet(footprint, 0,6,0,current.ctx);
	return result;
}

void FootprintFilterManagerI::visitBatch(const FootprintInfoSeq& fpts, const Ice::Current& current) {

	MCE_DEBUG("[FootprintFilterManagerI::visitHomepageBatch] called" << fpts.size() << " footprint(s)");
	if ( fpts.size() > 0 ) {
		// 异步批量踩脚印
		TaskManager::instance().execute(new BatchVisitTask(fpts, *this, current));
	}
}

bool FootprintFilterManagerI::visit(const FootprintInfoPtr& footprint, const Ice::Current& current) {

	if ( ! footprint ) {
		MCE_WARN("FootprintFilterManagerI::visit] footprint it null");
		return false;
	}	

	if ( ! stepFootprint(footprint, current.ctx) ) {
		return false;
	}

	dispatch(footprint);
	//HomeFootprintAdapter::instance().visit(footprint, current.ctx);
	return true;
}

bool FootprintFilterManagerI::stepFootprint(const FootprintInfoPtr& footprint, const map<string, string>& ctx) {
	
	if(!canPass(footprint))
		return false;

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

void FootprintFilterManagerI::contral(const MyUtil::Str2IntMap& cmd,
		const Ice::Current&) {
	for (MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++) {
		if ("OVER_COUNT"== it->first)
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
		else if ("ADD_BLACK")
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

StepPtr FootprintFilterManagerI::getStep(Ice::Int userId) {
	return ServiceI::instance().locateObject<StepPtr>(STEP, userId);
}
VisitorMapPtr FootprintFilterManagerI::getVisitorMap(Ice::Int userId) {
	return ServiceI::instance().locateObject<VisitorMapPtr>(VISITORMAP, userId);
}

bool FootprintFilterManagerI::canSend(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_send_config WHERE owner=" << fpt->owner << " AND guest=" << fpt->visitor;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintSendConfigHandler(flag));
	return flag;
}

bool FootprintFilterManagerI::canRecv(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_recv_config WHERE owner=" << fpt->owner << " AND guest=" << fpt->visitor;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintRecvConfigHandler(flag));
	return flag;
}

bool FootprintFilterManagerI::canStranger(const FootprintInfoPtr & fpt) {
	Statement sql;
	sql << "SELECT type FROM footprint_stranger_config WHERE id=" << fpt->owner;
	bool flag=true;
	QueryRunner("footprint_config",CDbRServer).query(sql, FootprintStrangerConfigHandler(flag));
	return flag;
}
// --------------------------------------------------------------------------
void BatchVisitTask::handle() {

	// 依次踩脚印
	int visited = 0; 
	for (FootprintInfoSeq::const_iterator it = _fpts.begin(); it != _fpts.end(); ++it ) {
		// 踩脚印
		if ( _filter.visit(*it, _crt) ) {
			visited++;
		}
	}
	MCE_INFO("[BatchVisitTask::handle]" << _fpts.size() << " footprint(s)" << ", " << visited << " visited");
}

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
void FootprintFilterManagerI::dispatchProfile(const FootprintInfoPtr& fpt){
	map<string,string> m;
	HomeFootprintAdapter::instance().visit(fpt,m);	
}
void FootprintFilterManagerI::dispatchAlbum(const FootprintInfoPtr& fpt){
	map<string,string> m;
	VisitorMapPtr p = getVisitorMap(fpt->owner);
	map<int,int>::iterator it = p->id2Time_.find(fpt->visitor);
	bool isValid = false;
	if(it!=p->id2Time_.end()){
		if(fpt->timestamp - it->second > 10*60){
			it->second = fpt->timestamp;	
			isValid = true;
		}else{
			isValid = false;
		}
	}else {
		isValid = true;
		p->id2Time_.insert(make_pair<int,int>(fpt->owner,fpt->timestamp));	
	}
	if(isValid){
		HomeFootprintAdapter::instance().visit(fpt,m);
	}
}

void FootprintFilterManagerI::dispatchBlog(const FootprintInfoPtr& fpt){
	map<string,string> m;
	HomeFootprintAdapter::instance().visit(fpt,m);	
	//BlogFootprintAdapter::instance().visitor(fpt);
}
void FootprintFilterManagerI::dispatch(const FootprintInfoPtr& fpt){
	GlobalIdPtr p =	GlobalIdTransfer::get(fpt->gid);
	switch(p->type){
		case PROFILE:
			dispatchProfile(fpt); 
		case BLOG:
			dispatchBlog(fpt);
		case ALBUM:
			dispatchAlbum(fpt);
	}; 
}
bool FootprintFilterManagerI::canPass(const FootprintInfoPtr& fpt){

	bool canPass = true;
	if(filter_list_[BLACK_VISITOR].size() && filter_list_[BLACK_VISITOR].find(fpt->visitor)!=filter_list_[BLACK_VISITOR].end()){
		canPass = filter_list_[WHITE_VISITOR].size() && filter_list_[WHITE_VISITOR].find(fpt->visitor)!=filter_list_[WHITE_VISITOR].end();
	}
	if(!canPass)
		return canPass;

	if(filter_list_[BLACK_OWNER].size() && filter_list_[BLACK_OWNER].find(fpt->visitor)!=filter_list_[BLACK_OWNER].end()){
		canPass = filter_list_[WHITE_OWNER].size() && filter_list_[WHITE_OWNER].find(fpt->visitor)!=filter_list_[WHITE_OWNER].end();
	}
	return canPass;
}
