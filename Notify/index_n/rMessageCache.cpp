#include "rMessageCache.h"
#include "NotifyI.h"
#include "Notify/util/schema_manager.h"
#include "Notify/util/rmessage_util.h"

using namespace xce::notify;
using namespace MyUtil;



//============
void UserMessage::AddMessage(const NotifyContentPtr& content){
	MessageBasePtr list = GetList(RMessageUtil::instance().Util_GetRMessageBigtype(content->type));
	if(list){
		list->AddMessage(content);
	}
}
//============



int MessageBase::GetUndealCountByBigtype(){
	return _undeal_count;	
}
int MessageBase::GetUndealCountByType(int type){
	int ans = 0;
	map<int, int>::iterator it = _type_undeal_count.find(type);
	if(it != _type_undeal_count.end()){
		ans = it->second;
	}
	return ans;
}

NotifyIndexSeq MessageBase::LoadSeqFromDBNowMerge(int wantbegin, int wantsize){

	if(_load_stat == LOAD_STAT_BUSY || !_isload){
		MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge--> load_stat is busy so do not load ~ uid: " << _userid << " wantbegin:" << wantbegin << " wantsize:" << wantsize << " load_stat:" << _load_stat << " bigtype:" << _bigtype << " _isload:" << _isload);
		return NotifyIndexSeq();
	} 
	_load_stat = LOAD_STAT_BUSY;


	MyUtil::IntSeq types = SchemaManager::instance().GetTypesByBigtype(_bigtype);
	if(types.empty()){
		MCE_WARN("MessageBase::LoadSeqFromDBNowMerge--> GetTypesByBigtype result empty !  bigtype:" << _bigtype);
	}


	ostringstream oss;
	for(MyUtil::IntSeq::iterator tit = types.begin(); tit != types.end(); ++tit){
		if(tit != types.begin()){
			oss << ",";
		}
		oss << boost::lexical_cast<string>(*tit);
	}


	typedef map<pair<int, Ice::Long> , NotifyIndexSeq> MergeMap;
	MergeMap merge;
	NotifyIndexSeq ans;//no merge
	NotifyIndexSeq nomerge;//no merge
	Ice::Long min_message_id = GetMinMessageId();
	MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge--> " << " wantbegin:" << wantbegin << " wantsize:" << wantsize);
	while(true){
		bool isbreak = false;
		Statement sql;
		list<NotifyIndex> seq;
		sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and bigtype = " << _bigtype << " and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count; 
		//sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and type in ( " << oss.str() << ") and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count;
		mysqlpp::StoreQueryResult res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
		if(!res){
			isbreak = true;
		}
		if (res && res.num_rows() > 0) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				NotifyIndex index;
				mysqlpp::Row row = res.at(i);
				index.id = (Ice::Long) row["id"];
				int bigtype = (int)row["bigtype"];
				int type = (int)row["type"];
				type |= (bigtype << 16);
				index.type = type;
				index.source = (Ice::Long) row["source"];
				index.from = (Ice::Int) row["from_id"];

				int stype = RMessageUtil::instance().Util_GetRMessageType(index.type);
				bool ismerge = SchemaManager::instance().MustMerge(stype, 16);
				if(ismerge){
					merge[make_pair<int, Ice::Long>(stype, index.source)].push_back(index);
				}
				else{
					nomerge.push_back(index);
				}
				int realbeginsize = nomerge.size() + merge.size();
				seq.push_back(index);
				if(realbeginsize > wantsize + wantbegin){
					MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge BREAK --> userid:" << _userid << " realbeginsize:" << realbeginsize << " wantsize+wantbegin:" << wantsize + wantbegin);
					isbreak = true;
				}else{
					int realtotalsize = nomerge.size() + merge.size();
					int endpos = wantbegin + wantsize;
					if(!wantbegin){
						endpos += 1;	
					}
					if(realtotalsize >= wantbegin && realtotalsize < endpos){
						ans.push_back(index);
						MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge --> insert into ans id:" << index.id);
					}
				}
			}
		}
		if((int)(seq.size()) < _first_load_count){
			//update load state
			MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge BREAK --> _userid:" << _userid << " seq.size()" << seq.size() << " first_load_count:" << _first_load_count);
			_isload = false;
			isbreak = true;
		}
		AppendMessages(seq, false);
		MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge-->seq.size():" << seq.size());
		min_message_id = GetMinMessageId();
		if(isbreak){
			break;
		}
	}
	_load_stat = LOAD_STAT_FREE;
	MCE_DEBUG("MessageBase::LoadSeqFromDBNowMerge-->userid:" << _userid << " ans.size now:" << ans.size() << " merge.size():" << merge.size() << " nomerge.size():" << nomerge.size() << " _load_stat:" << _load_stat << " _bigtype:" << _bigtype << " isload:" << _isload);
	return ans;
}





NotifyIndexSeq MessageBase::LoadSeqFromDBNow(const set<int>& types, int wantbegin, int wantsize){

	if(_load_stat == LOAD_STAT_BUSY || !_isload){
		MCE_DEBUG("MessageBase::LoadSeqFromDBNow--> load_stat is busy so do not load ~ uid: " << _userid << " wantbegin:" << wantbegin << " wantsize:" << wantsize << " load_stat:" << _load_stat << " bigtype:" << _bigtype << " _isload:" << _isload);
		return NotifyIndexSeq();
	} 
	_load_stat = LOAD_STAT_BUSY;


	MyUtil::IntSeq typess = SchemaManager::instance().GetTypesByBigtype(_bigtype);
	if(types.empty()){
		MCE_WARN("MessageBase::LoadSeqFromDBNow--> GetTypesByBigtype result empty !  bigtype:" << _bigtype);
	}

	ostringstream oss;
	for(MyUtil::IntSeq::iterator tit = typess.begin(); tit != typess.end(); ++tit){
		if(tit != typess.begin()){
			oss << ",";
		}
		oss << boost::lexical_cast<string>(*tit);
	}

	NotifyIndexSeq ans;
	Ice::Long min_message_id = GetMinMessageId();
	int s = 0;
	MCE_DEBUG("MessageBase::LoadSeqFromDBNow--> types.size():" << types.size() << " wantbegin:" << wantbegin << " wantsize:" << wantsize);
	while(true){
		bool isbreak = false;
		Statement sql;
		list<NotifyIndex> seq;
		sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and bigtype = " << _bigtype << " and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count; 
		//sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and type in (" << oss.str() << ") and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count;
		mysqlpp::StoreQueryResult res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
		if(!res){
			isbreak = true;
		}
		if (res && res.num_rows() > 0) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				NotifyIndex index;
				mysqlpp::Row row = res.at(i);
				index.id = (Ice::Long) row["id"];
				int bigtype = (int)row["bigtype"];
				int type = (int)row["type"];
				type |= (bigtype << 16);
				index.type = type;
				index.source = (Ice::Long) row["source"];
				index.from = (Ice::Int) row["from_id"];
				seq.push_back(index);
				if(types.find(index.type) != types.end() || types.find(-1) != types.end()){
					if((int)(ans.size()) > wantsize + wantbegin){
						isbreak = true;
					}else{
						if(s >= wantbegin){
							ans.push_back(index);
						}
					}
					++s;
				}
			}
		}
		if((int)(seq.size()) < _first_load_count){
			//update load state
			_isload = false;
			isbreak = true;
		}
		AppendMessages(seq, false);
		MCE_DEBUG("MessageBase::LoadSeqFromDBNow-->seq.size()" << seq.size());
		min_message_id = GetMinMessageId();
		if(isbreak){
			break;
		}
	}
	_load_stat = LOAD_STAT_FREE; 
	MCE_DEBUG("MessageBase::LoadSeqFromDBNow-->userid:" << _userid << " ans.size now:" << ans.size() << " load_stat:" << _load_stat << " bigtype:" << _bigtype << " isload:" << _isload);
	return ans;
}


void MessageBase::Evict(){
	bool e = false;
	while(true){
		if((int)(_messagelist.size()) <= _evict_threshold_value){
			break;
		}
		e = true;
		_messagelist.pop_back();
	}
	if(e){
		MCE_DEBUG("MessageBase::Evict-->userid:" << _userid << " _messagelist.size:" << _messagelist.size());
	}
	_isload = true;
}

void MessageBase::AppendMessages(const list<NotifyIndex>& seq, bool update_load_stat){
	MCE_DEBUG("MessageBase::AppendMessages--> seq.size:" << seq.size() << " userid:" << _userid);
	if(update_load_stat){
		_load_stat = LOAD_STAT_FREE;
		MCE_INFO("MessageBase::AppendMessages-->update_load_stat = true , load_stat = " << _load_stat);
	}
	if(seq.empty()){
		return;
	}
	for(list<NotifyIndex>::const_iterator it = seq.begin(); it != seq.end(); ++it){
		_messagelist.push_back(*it);
	}
	if((int)(_messagelist.size()) >= _evict_threshold_value){
		MCE_INFO("MessageBase::AppendMessages-->userid:" << _userid << " should Evict");
		NotifyIndexManagerI::instance().UserRMessageEvict(_userid);
	}
}

void MessageBase::UpdateUserBigtypeLoadStat(bool isload){
	_isload = isload;
	MCE_DEBUG("MessageBase::UpdateUserBigtypeLoadStat--> userid:" << _userid << " bigtye:" << _bigtype << " _load_stat:" << _load_stat << " isload:" << _isload);
}

int MessageBase::GetUnreadCountByBigtype(){
	MCE_DEBUG("MessageBase::GetUnreadCountByBigtype--> userid :" << _userid << " unread_count:" << _unread_count);
	return _unread_count;
	/*
	int ans = 0;
	if(GetMinMessageId() <= _min_unread_id){
		for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
			if((*it).id >= _min_unread_id){
				++ans;
			}else{
				break;
			}
		}
		return ans;
	}
	else{
		Statement sql;
		//sql << "SELECT COUNT(*) AS unread_count from " << NotifyDbHelper::getIndexTableName(_uid) << " WHERE to_id = " << _uid << " AND id >=" << _min_unread_nid;
	}
	return ans;
	*/
}

void MessageBase::CheckAndLoadFromDB(int leftvalue, const set<Ice::Long>& blacklist_nids){
	MCE_DEBUG("MessageBase::CheckAndLoadFromDB-->userid:" << _userid << " leftvalue:" << leftvalue << " bigtype:" << _bigtype << " _load_stat:" << _load_stat << " _isload:" << _isload);

	if(_load_stat == LOAD_STAT_FREE && _isload){
		int left_merge_count = 0;
		if(leftvalue){
			map< pair<int, long>, int > left_merge;
			int c =0;
			for(list<NotifyIndex>::reverse_iterator it = _messagelist.rbegin(); it != _messagelist.rend(); ++it, ++c){
				if(c >= leftvalue || left_merge_count > _preload_threshold_value){
					break;
				}
				int type = (*it).type;
				Ice::Long source = (*it).source;
				left_merge[make_pair<int, long>(type, source)] += 1;
			}
			left_merge_count = (int)left_merge.size();
		}
		if(left_merge_count <= _preload_threshold_value){
			MCE_DEBUG("MessageBase::CheckAndLoadFromDB-->userid" << _userid << " use preload start LoadMessageTask");
			_load_stat = LOAD_STAT_BUSY;
			TaskManager::instance().execute(new LoadMessageTask(_userid, _bigtype, _first_load_count, GetMinMessageId(), blacklist_nids));
		}
	}
	MCE_DEBUG("MessageBase::CheckAndLoadFromDB-->userid:" << _userid << " load_stat:" << _load_stat << " bigtype:" << _bigtype << " _isload:" << _isload);
}

NotifyIndexSeq MessageBase::GetSeqBeforLastNid(int view, Ice::Long nid, int limit){
	NotifyIndexSeq ans;
	int count = limit;
	if(limit < 0 || limit >= (int)(_messagelist.size())){
		count = _messagelist.size();
	}
	int s = 0, c = 0;
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end() && c < count; ++it, ++s){
		if((view == 1 || view == 0) && ((*it).id >= nid)){
			ans.push_back((*it));
			c++;
		}
	}

	int endpos = count;
	int messagesize = _messagelist.size();
	CheckAndLoadFromDB(messagesize - endpos, set<Ice::Long>());
	
	if(_min_unread_id != LLONG_MAX){
		_min_unread_id = LLONG_MAX;
		TaskManager::instance().execute(new UpdateMinUnreadIdTask(_userid, _min_unread_id));
	}
	_unread_count = 0;
	MCE_INFO("MessageBase::GetSeqBeforLastNid-->userid" << _userid << " _messagelist.size():" << _messagelist.size() << " _min_unread_id:" << _min_unread_id << " _unread_count" << _unread_count << " ans.size:" << ans.size());
	return ans;
}

NotifyIndexSeq MessageBase::GetSeqBeforMerge(int view, Ice::Long nid, int limit){
	
	typedef map<pair<int, Ice::Long> , NotifyIndexSeq> MergeMap;
	
	MergeMap merge;

	NotifyIndexSeq ans;
	if(nid < 0){
		nid = LLONG_MAX;
	}
	int count = 0;
	//int begin = 0;
	if(limit < 0){
		return ans;
	}

	if(limit >= (int)(_messagelist.size())){
		limit = _messagelist.size();
	}

	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
		if((int)(merge.size()) >= limit){
			break;
		}
		NotifyIndex index = *it;
		if(index.id <= nid){
			//merge[make_pair<int, Ice::Long>(index.type, index.source)].push_back(index);
			merge[make_pair<int, Ice::Long>(RMessageUtil::instance().Util_GetRMessageType(index.type), index.source)].push_back(index);
		}
		count++;
	}

	for(MergeMap::iterator it = merge.begin(); it != merge.end(); ++it){
		for(NotifyIndexSeq::iterator vit = it->second.begin(); vit != it->second.end(); ++vit){
			ans.push_back(*vit);
		}
	}
	
	int endpos =  count;
	int messagesize = _messagelist.size();
	CheckAndLoadFromDB(messagesize - endpos, set<Ice::Long>());
	
	if(LLONG_MAX != _min_unread_id ){
		_min_unread_id = LLONG_MAX;
		TaskManager::instance().execute(new UpdateMinUnreadIdTask(_userid, _min_unread_id));
	}
	_unread_count = 0;
	MCE_INFO("MessageBase::GetSeqBeforMerge-->userid" << _userid << " _messagelist.size():" << _messagelist.size() << " _min_unread_id:" << _min_unread_id << " _unread_count" << _unread_count << " ans.size:" << ans.size() << " count=" << count << " merge.size:" << merge.size());
	return ans;
}

NotifyIndexSeq MessageBase::GetSeqByBigtype(int begin, int limit){

	typedef map<pair<int, Ice::Long> , NotifyIndexSeq> MergeMap;
  MergeMap merge;//需要合并的结果集
	NotifyIndexSeq nomerge;//不需要合并的结果集
	NotifyIndexSeq nomergeans;//不需要合并的结果集
  MergeMap seqs;

	NotifyIndexSeq ans;
	int count = limit;
	if(begin < 0){
		return ans;
	}
	if(limit < 0 || limit >= (int)(_messagelist.size())){
		count = _messagelist.size();
	}
	MCE_DEBUG("MessageBase::GetSeqByBigtype--> begin:" << begin << " limit:" << limit << " count:" << count);
	int realbeginsize = 0;
	int realtotalsize = 0;
	if((int)(_messagelist.size()) >= begin){
		MCE_DEBUG("MessageBase::GetSeqByBigtype--> _messagelist.size():" << _messagelist.size() << " begin:" << begin);
		int s = 0;
		for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it, ++s){
			NotifyIndex index = *it;
			int stype = RMessageUtil::instance().Util_GetRMessageType(index.type);
			bool ismerge = SchemaManager::instance().MustMerge(stype, 16);
			bool isbreak = false;
			if(ismerge){
				seqs[make_pair<int, Ice::Long>(stype, index.source)].push_back(index);
				realbeginsize = seqs.size() + nomerge.size();//get the total size
				if(realbeginsize >= begin){ 
					MergeMap::iterator mit = merge.find(make_pair<int, Ice::Long>(stype, index.source));
					if(mit != merge.end()){
						mit->second.push_back(index);
					}else{
						realtotalsize = nomergeans.size() + merge.size();
						if(realtotalsize >= count){
							MCE_DEBUG("MessageBase::GetSeqByBigtype-->@@@@@@@@@@@@@@@@@@@ realtotalsize = " << realtotalsize << " count:" << count);
							break;
						}
						merge[make_pair<int, Ice::Long>(stype, index.source)].push_back(index);
					}
				}
			}
			else{
				nomerge.push_back(index);
				realbeginsize = seqs.size() + nomerge.size();
				if(realbeginsize >= begin){ 
					realtotalsize = nomergeans.size() + merge.size();
					if(realtotalsize >= count){
						MCE_DEBUG("MessageBase::GetSeqByBigtype-->@@@@@@@@@@@@@@@@@@@ realtotalsize = " << realtotalsize << " count:" << count);
						break;
					}
					nomergeans.push_back(index);
				}
			}
			//MCE_DEBUG("MessageBase::GetSeqByBigtype--> NOTIFY INFO: id:" << index.id << " type:" << index.type << " stype:" << RMessageUtil::instance().Util_GetRMessageType(index.type) << " bigtype:" << RMessageUtil::instance().Util_GetRMessageBigtype(index.type) << " realbeginsize:" << realbeginsize << " realtotalsize:" << realtotalsize << " nomergecount:" << nomerge.size() << " merge.size():" << merge.size() << " nomergeans.size():" << nomergeans.size() << " isbreak:" << isbreak);
			//if(isbreak){
			//	break;
			//}
		}
	}

	for(MergeMap::iterator it = merge.begin(); it != merge.end(); ++it){
		for(NotifyIndexSeq::iterator vit = it->second.begin(); vit != it->second.end(); ++vit){
			ans.push_back(*vit);
		}
	}
	ans.insert(ans.end(), nomergeans.begin(), nomergeans.end());

	if(LLONG_MAX != _min_unread_id ){
		_min_unread_id = LLONG_MAX;
		TaskManager::instance().execute(new UpdateMinUnreadIdTask(_userid, _min_unread_id));
	}
	_unread_count = 0;
	MCE_DEBUG("MessageBase::GetSeqByBigtype-->count:"<< count << " merge.size:" << merge.size() << " nomerge.size():" << nomerge.size() << " nomergeans.size():" << nomergeans.size() << " ans.size():" << ans.size());


	realtotalsize = merge.size() + nomergeans.size();
	if(realtotalsize > count || (realtotalsize == count && count)){
		int endpos = begin + count;
		int messagesize = _messagelist.size();
		CheckAndLoadFromDB(messagesize - endpos, set<Ice::Long>());
		MCE_INFO("MessageBase::GetSeqByBigtype-->userid" << _userid << " _messagelist.size():" << _messagelist.size() << " _min_unread_id:" << _min_unread_id << " _unread_count" << _unread_count << " ans.size:" << ans.size());
		return ans;
	}else{
		int wantsize = count - realtotalsize;
		int wantbegin = 0;
		if(realtotalsize){
			wantbegin = 0;
		}else{
			wantbegin = begin;
		}
		NotifyIndexSeq ans2 = LoadSeqFromDBNowMerge(wantbegin ,wantsize);
		if(!ans2.empty()){
			ans.insert(ans.end(), ans2.begin(), ans2.end());
		}
		MCE_DEBUG("MessageBase::GetSeqByBigtype-->AFTER LoadSeqFromDBNowMerge ans.size:" << ans.size() << " wantbegin:" << wantbegin << " wantsize:" << wantsize);
		CheckAndLoadFromDB(0, set<Ice::Long>());
		return ans;
	}
}



NotifyIndexSeq MessageBase::GetSeqByTypeAndBigtype(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype){
	NotifyIndexSeq ans;
	set<int> typeset;
	for(MyUtil::IntSeq::const_iterator tit = types.begin(); tit != types.end(); ++tit){
		typeset.insert(*tit);
	}
	int s = 0;
	if(limit <= 0){
		limit = _messagelist.size();
	}
	if(types.size() == 1){
		int firsttype = (*(types.begin()));
		if(_type_undeal_count.find(firsttype) != _type_undeal_count.end()){
			int undealcount = _type_undeal_count[firsttype];
			if(limit > undealcount){
				limit = undealcount;
			}
		}
	}
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
		if((int)(ans.size()) >= limit){
			break;
		}
		NotifyIndex index = *it;
		int type = RMessageUtil::instance().Util_GetRMessageType(index.type);
		if(typeset.find(type) != typeset.end()){
			if(s >= begin){
				ans.push_back((*it));
				MCE_DEBUG("MessageBase::GetSeqByTypeAndBigtype-->i:" << s << " type:" << type << " index.type:" << index.type);
			}
			++s;
		}
	}
	if(LLONG_MAX != _min_unread_id){
		_min_unread_id = LLONG_MAX;
		TaskManager::instance().execute(new UpdateMinUnreadIdTask(_userid, _min_unread_id));
	}
	_unread_count = 0;
	MCE_DEBUG("MessageBase::GetSeqByTypeAndBigtype--> load from , ans.size():" << ans.size() << " limit:" << limit);
	if((int)(ans.size()) >= limit){
		int endpos = begin + limit;
		int messagesize = _messagelist.size();
		CheckAndLoadFromDB(messagesize - endpos, set<Ice::Long>());
		MCE_INFO("MessageBase::GetSeqByTypeAndBigtype-->userid" << userid << " _messagelist.size():" << _messagelist.size() << " _min_unread_id:" << _min_unread_id << " _unread_count" << _unread_count << " ans.size:" << ans.size() << " begin:" << begin << " limit:" << limit << " endpos:" << endpos );
		return ans;
	}else{
		int wantsize = limit - ans.size();	
		int wantbegin = 0;
		if(ans.size()){
			wantbegin = 0;
		}else{
			wantbegin = begin;
		}
		NotifyIndexSeq ans2 = LoadSeqFromDBNow(typeset, wantbegin ,wantsize);
		if(!ans2.empty()){
			ans.insert(ans.end(), ans2.begin(), ans2.end());
		}
		return ans;
	}
}

NotifyIndexSeqMap MessageBase::GetSeqByTypeAndBigtypeAll(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype){
}


void MessageBase::RemoveBySourceAndSender(int type, Ice::Long source, int sender){
	set<Ice::Long> delnids;
	int delcount = 0;
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end();){
		int realtype = RMessageUtil::instance().Util_GetRMessageType((*it).type);
		if(realtype == type && (*it).source == source && (*it).from == sender){
			_messagelist.erase(it++);
			delnids.insert((*it).id);
			++delcount;
		}
		else{
			++it;
		}
	}
	UpdateUndealCount(type, delcount);
	Statement sql;
	GetRemoveBySourceAndSenderSql(sql, type, source, sender);
	//TaskManager::instance().execute(new RemoveDBTask(sql, _userid));
	//TaskPtr t = new RemoveDBTask(sql, _userid);
	//t->handle();
	CheckAndLoadFromDB(_messagelist.size(), delnids);
}



void MessageBase::RemoveBySource(int type, Ice::Long source){
	set<Ice::Long> delnids;
	ostringstream os;	
	os << "MessageBase::RemoveBySource--> userid:" << _userid << " bigtype:" << _bigtype << " type:" << type << " source" << source << " nids:";

/*
	ostringstream logos;
	logos << "MessageBase::RemoveByIds--> BEFOR REMOVE:";
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
		NotifyIndex ele = *it;
		logos << "id:" << ele.id << " type:" << ele.type << " source:" << ele.source << "\n";	
	}
	MCE_DEBUG(logos.str());
*/
	int delcount = 0;
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end();){
		int realtype = RMessageUtil::instance().Util_GetRMessageType((*it).type);
		if(realtype == type && (*it).source == source){
			_messagelist.erase(it++);
			delnids.insert((*it).id);
			++delcount;
			os << (*it).id << ", ";
		}else{
			++it;
		}
	}
	UpdateUndealCount(type, delcount);
	Statement sql;
	GetRemoveBySourceSql(sql);
	//TaskManager::instance().execute(new RemoveDBTask(sql, _userid));
	//TaskPtr t = new RemoveDBTask(sql, _userid);
	//t->handle();
	os << " after remove  _messagelis.size:" << _messagelist.size() << " blacklist.size():"<< delnids.size(); 
	MCE_DEBUG(os.str());
	CheckAndLoadFromDB(_messagelist.size(), delnids);
}

void MessageBase::RemoveByIds(const MyUtil::LongSeq& nids, int type){
	//MCE_INFO("MessageBase::RemoveByIds~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	ostringstream os;	
	int delcount = 0;
	os << "MessageBase::RemoveByIds--> userid:" << _userid << " bigtype:" << _bigtype << " nids:";


	for(MyUtil::LongSeq::const_iterator nit = nids.begin(); nit != nids.end(); ++nit){
		MCE_DEBUG("MessageBase::RemoveByIds--> WANT DELETE IDS:" << (*nit));
	}
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
		MCE_DEBUG("MessageBase::RemoveByIds--> NOW HAVE:" << (*it).id);
	}




	for(MyUtil::LongSeq::const_iterator nit = nids.begin(); nit != nids.end(); ++nit){
		Ice::Long nid = *nit;
		for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ){
			if((*it).id == nid){
				os << nid << ", ";
				_messagelist.erase(it++);
				++delcount;
				break;
			}
			else{
				++it;
			}
		}
	}
	UpdateUndealCount(type, delcount);
	MCE_INFO(os.str());
	Statement sql;
	GetRemoveByIdsSql(sql, nids);
	//TaskManager::instance().execute(new RemoveDBTask(sql, _userid));
	//TaskPtr t = new RemoveDBTask(sql, _userid);
	//t->handle();

	set<Ice::Long> blacklist_nids;
	
	blacklist_nids.insert(nids.begin(), nids.end());

	CheckAndLoadFromDB(_messagelist.size(), blacklist_nids);
}

void MessageBase::RemoveById(Ice::Long nid, int type){
	set<Ice::Long> delnids;
	for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
		MCE_DEBUG("MessageBase::RemoveById-->it.id:" << (*it).id << " nid:" << nid);
		if((*it).id == nid){
			_messagelist.erase(it);
			delnids.insert((*it).id);
			UpdateUndealCount(type, 1);
			MCE_DEBUG("MessageBase::RemoveById-->userid:" << _userid << " notifyid:" << nid << " bigtype:" << _bigtype);
			break;
		}
	}
	Statement sql;
	GetRemoveByIdSql(sql, nid);
	//TaskManager::instance().execute(new RemoveDBTask(sql, _userid));
	//TaskPtr t = new RemoveDBTask(sql, _userid);
	//t->handle();
	CheckAndLoadFromDB(_messagelist.size(), delnids);
}

void MessageBase::AddMessage(const NotifyContentPtr& content){
//添加notify index
	NotifyIndex index;
	index.id = content->id;
	index.source = content->source;
	index.type = content->type;
	index.isreply = content->isreply;
	index.from = content->from;
	_messagelist.push_front(index);
	for(list<NotifyIndex>::iterator logit = _messagelist.begin(); logit != _messagelist.end(); ++logit){
		NotifyIndex index = *logit;
		MCE_DEBUG("MessageBase::AddMessage-->id:" << index.id);
	}

//如果长度超长，列表向左增长，新来的id是大的，那么逐出后面一个小的
	if((int)(_messagelist.size()) >= _evict_threshold_value){
		_messagelist.pop_back();
		_isload = true;
		MCE_DEBUG("MessageBase::AddMessage-->_userid:" << _userid << " _load_stat:" << _load_stat << " _isload:" << _isload);
	}
	if(index.id < _min_unread_id){
		_min_unread_id = index.id;
		TaskManager::instance().execute(new UpdateMinUnreadIdTask(_userid, _min_unread_id));
	}
	//更新count
	++_unread_count;
	++_undeal_count;
	int stype = RMessageUtil::instance().Util_GetRMessageType(index.type);
	map<int, int>::iterator it = _type_undeal_count.find(stype);	
	if(it == _type_undeal_count.end()){
		_type_undeal_count.insert(make_pair<int,int>(stype, 0));
	}
	++_type_undeal_count[stype];
	//MCE_INFO("MessageBase::AddMessage-->userid:" << _userid << " _undeal_count:" << _undeal_count <<  " _unread_count:" << _unread_count << " _min_unread_id:" << _min_unread_id << " _messagelist.size():" << _messagelist.size() << " INFO----> id:" << index.id << " source:" << index.source << " type:" << index.type << " isreply:" << index.isreply << " from:" << index.from << " stype:" << RMessageUtil::instance().Util_GetRMessageType(index.type) << " bigtype:" << RMessageUtil::instance().Util_GetRMessageBigtype(index.type));
} 






void MessageBase::FirstLoad(){
	
	/*
	MyUtil::IntSeq types = SchemaManager::instance().GetTypesByBigtype(_bigtype);
	if(types.empty()){
		MCE_WARN("MessageBase::FirstLoad--> GetTypesByBigtype result empty !  bigtype:" << _bigtype);
	}


	ostringstream oss;
	for(MyUtil::IntSeq::iterator tit = types.begin(); tit != types.end(); ++tit){
		if(tit != types.begin()){
			oss << ",";
		}
		oss << boost::lexical_cast<string>(*tit);
	}
	*/
  Statement sql;

	
	sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << GetMinMessageId() <<" and bigtype = " << _bigtype << " and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count; 
	//sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << GetMinMessageId() <<" and type in ( " << oss.str() << ") and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count; 

	mysqlpp::StoreQueryResult res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
	if (res && res.num_rows() > 0) {
		if(res.num_rows() < _first_load_count){
			_isload = false;
		}
		for (size_t i = 0; i < res.num_rows(); ++i) {
			NotifyIndex index;
			mysqlpp::Row row = res.at(i);
			index.id = (Ice::Long) row["id"];
			int bigtype = (int)row["bigtype"];
			int type = (Ice::Int) row["type"];
			type |= (bigtype << 16);
			index.type = type;
			index.source = (Ice::Long) row["source"];
			index.from = (Ice::Int) row["from_id"];
			//index.isreply = (Ice::Int) row["is_reply"];
			//index.isreply = (Ice::Int) row["is_reply"];
			_messagelist.push_back(index);
		}
	}
	
	//获取最小未读ID
	sql.clear();
	sql << "select min_unread_nid from " << GetInfoTableName(_userid) << " where id = " << _userid;
	res = QueryRunner(GetDbInstance(), CDbRServer, GetInfoTableName(_userid)).store(sql);

	if(res && res.num_rows() > 0){
		mysqlpp::Row row = res.at(0);
		_min_unread_id = (Ice::Long)row["min_unread_nid"];
		MCE_DEBUG("MessageBase::FirstLoad--> has one _userid:" << _userid << " _min_unread_id:" << _min_unread_id << " row.size:" << res.num_rows() << " min_unread_id:" << row["min_unread_nid"]);
	}
	else{
		_min_unread_id = LLONG_MAX;
		MCE_DEBUG("MessageBase::FirstLoad--> no one _userid:" << _userid << " _min_unread_id:" << _min_unread_id);
	}
	/*	
	//更新最小未读数
	int c = 0;
	if(GetMinMessageId() <= _min_unread_id ){
		for(list<NotifyIndex>::iterator it = _messagelist.begin(); it != _messagelist.end(); ++it){
			if((*it).id >= _min_unread_id){
				c++;
			}
			else{
				break;
			}
		}
	}
	else{
		Statement sql;
		sql << "SELECT COUNT(*) AS unread_count from " << GetIndexTableName(_userid) << " WHERE to_id = " << _userid << " AND id >=" << _min_unread_id;
		res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
		if(res.num_rows() > 0){
			mysqlpp::Row row = res.at(0);
			c = (int)row["unread_count"];
		}
	}
	_unread_count = c;
	//获取用户全部未处理数
	*/	
	
	int querycount = 0;
	Ice::Long min_message_id = LLONG_MAX; 
	while(true){
		sql.clear();
		ostringstream os;
		//os << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and type in ( " << oss.str() << ") and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count;
		//MCE_DEBUG("MessageBase::FirstLoad--> SQLIS : " << os.str());
		//sql << os.str();
		sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << min_message_id <<" and bigtype = " << _bigtype << " and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _first_load_count; 
		res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
		if (res && res.num_rows() > 0) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				NotifyIndex index;
				mysqlpp::Row row = res.at(i);
				index.id = (Ice::Long) row["id"];
				int bigtype = (int)row["bigtype"];
				int type = (Ice::Int) row["type"];
				index.type = type | (bigtype << 16);
				index.source = (Ice::Long) row["source"];
				index.from = (Ice::Int) row["from_id"];
				//index.isreply = (Ice::Int) row["is_reply"];
				//index.isreply = (Ice::Int) row["is_reply"];
				//_messagelist.push_back(index);
				++_undeal_count;
				//MCE_DEBUG("MessageBase::FirstLoad-->_userid:" << _userid << " nid:" << index.id << " bigtype:" << bigtype << " type:" << type << " inex.type:" << index.type);
				int stype = RMessageUtil::instance().Util_GetRMessageType(type);
				if(_type_undeal_count.find(stype) == _type_undeal_count.end()){
					_type_undeal_count.insert(make_pair<int, int>(stype, 0));
				}
				++_type_undeal_count[stype];
				if(index.id < min_message_id){
					min_message_id = index.id;
					MCE_DEBUG("MessageBase::FirstLoad--> update min_message_id:" << min_message_id);
				}
				if(index.id > _min_unread_id){
					_unread_count++;
					MCE_DEBUG("MessageBase::FirstLoad--> update unread_count:" << _unread_count);
				}
			}
		}
		if((int)(res.num_rows()) < _first_load_count || ++querycount >= 500){
			//_isload = false;
			break;
		}
		if(!res){
			break;
		}
	}
	if((int)(_messagelist.size()) >= _evict_threshold_value){
		MCE_INFO("MessageBase::FirstLoad-->userid:" << _userid << " should Evict");
		NotifyIndexManagerI::instance().UserRMessageEvict(_userid);
	}
	ostringstream os;
	os << "MessageBase::FirstLoad-->_userid:" << _userid << " isload:" << _isload << " unread_count:" << _unread_count <<  " _undeal_count:" << _undeal_count << " ";
	for(map<int, int>::iterator it = _type_undeal_count.begin(); it != _type_undeal_count.end(); ++it){
		os << "type: " << it->first << " count:" << it->second << " ; ";
	}
	MCE_DEBUG(os.str());
}

void MessageBase::PreLoad(){
}

void UserMessagePool::AddUserMessage(const UserMessagePtr& usermessage){

	std::pair<UserMessageCache::iterator, bool> p = _cache.push_front(usermessage);
	if(p.second){
		MCE_DEBUG("UserMessagePool::AddUserMessage--> userid= " << usermessage->_userid<< " OK");
	}
	if(!p.second){
		MCE_DEBUG("UserMessagePool::AddUserMessage--> add err same userid exist and replace and reloate userid:" << usermessage->_userid);
		_cache.replace(p.first, usermessage);
		_cache.relocate(_cache.begin(), p.first);
	}else if((int)(_cache.size()) > CACHESIZE){
		MCE_DEBUG("RoomPool::AddRoom --> pop last");
		_cache.pop_back();
	}
}


UserMessagePtr UserMessagePool::FindUserMessage(int userid){
	UserMessagePtr ans;
	IceUtil::Mutex::Lock lock(_cachemutex);
	UserIdIndex& id_index = _cache.get<1>();
	UserIdIndex::iterator idit = id_index.find(userid);
	if(idit != id_index.end()){
		ans = (*idit);
		SequencedIndex& seq_index = _cache.get<0>();
		_cache.relocate(seq_index.begin(), _cache.project<0>(idit));
	}
	return ans;
}

UserMessagePtr UserMessagePool::LocateUserMessage(int userid){
	UserMessagePtr ans;
	{
		IceUtil::Mutex::Lock lock(_cachemutex);
		UserIdIndex& id_index = _cache.get<1>();
		UserIdIndex::iterator idit = id_index.find(userid);
		if(idit != id_index.end()){
			ans = (*idit);
			SequencedIndex& seq_index = _cache.get<0>();
			_cache.relocate(seq_index.begin(), _cache.project<0>(idit));
		}
	}
	if(!ans){
		ans = new UserMessage(userid);
		if(ans){
			ans->FirstLoad();
			IceUtil::Mutex::Lock lock(_cachemutex);
			UserIdIndex& id_index = _cache.get<1>();
			UserIdIndex::iterator idit = id_index.find(userid);
			if(idit == id_index.end()){
				AddUserMessage(ans);
			}
			else{
				ans = (*idit);
			}
		}
	}
	return ans;
}
//==================================================================================================================

void UserMessagePool::UpdateUserBigtypeLoadStat(int userid, int bigtype, bool isload){
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		usermessage->UpdateUserBigtypeLoadStat(bigtype, isload);
	}
}

void UserMessagePool::AppendMessages(const list<NotifyIndex>& seq, int userid, int bigtype, bool update_load_stat){
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		usermessage->AppendMessages(seq, bigtype, update_load_stat);
	}
}

void UserMessagePool::AddMessage(const NotifyContentPtr& content, const MyUtil::IntSeq& target){
	MCE_DEBUG("UserMessagePool::AddMessage--> content->id:" << content->id << " content.type:" << content->type);
	for(MyUtil::IntSeq::const_iterator it = target.begin(); it != target.end(); ++it){
		UserMessagePtr usermessage = LocateUserMessage(*it);
		if(usermessage){
			IceUtil::RWRecMutex::WLock lock(_rwmutex);
			usermessage->AddMessage(content);
		}
	}
}

void UserMessagePool::RemoveById(int userid, Ice::Long notifyid, int type, int b){
	int bigtype = SchemaManager::instance().GetBigtypeBytype(type);
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		MCE_INFO("UserMessagePool::RemoveById--> userid:" << userid << " notifyid:" << notifyid << " bigtype:" << bigtype << " type:" << type);
		usermessage->RemoveById(notifyid, type, bigtype);
	}
}

void UserMessagePool::RemoveByIds(int userid, const MyUtil::LongSeq& nids, int type, int b){
	int bigtype = SchemaManager::instance().GetBigtypeBytype(type);
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		MCE_DEBUG("UserMessagePool::RemoveByIds --> " << userid);
		usermessage->RemoveByIds(nids, type, bigtype);
		MCE_DEBUG("UserMessagePool::RemoveByIds --> !!!!" << userid);
	}
}


void UserMessagePool::RemoveBySource(Ice::Int userid, Ice::Int type, Ice::Long source, int b){
	int bigtype = SchemaManager::instance().GetBigtypeBytype(type);
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		usermessage->RemoveBySource(type, source, bigtype);
	}
}


void UserMessagePool::RemoveBySourceAndSender(Ice::Int userid, Ice::Int type, Ice::Long source, Ice::Int sender, int b){
	int bigtype = SchemaManager::instance().GetBigtypeBytype(type);
	UserMessagePtr usermessage = FindUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		usermessage->RemoveBySourceAndSender(type, source, sender, bigtype);
	}
}

NotifyIndexSeq UserMessagePool::GetSeqByBigtype(int userid, int begin, int limit, int bigtype){
	NotifyIndexSeq ans;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		ans = usermessage->GetSeqByBigtype(begin, limit, bigtype);
	}
	MCE_INFO("UserMessagePool::GetSeqByBigtype --> userid:" << userid << " begin:" << begin << " limit:" << limit << " bigtype:" << bigtype << " ans.size:" << ans.size());
	return ans;
}

NotifyIndexSeq UserMessagePool::GetSeqByTypeAndBigtype(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype){
	NotifyIndexSeq ans;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		ans = usermessage->GetSeqByTypeAndBigtype(userid, types, begin, limit, bigtype);
	}
	ostringstream os;
	os << " para types:";
	for(MyUtil::IntSeq::const_iterator tit = types.begin(); tit != types.end(); ++tit){
		os << (*tit) << ", ";
	}
	MCE_INFO("UserMessagePool::GetSeqByTypeAndBigtype --> userid:" << userid << " types.size:" << types.size()<< " bigtype:" << bigtype << " ans.size:" << ans.size() << os.str());
	return ans;
}

NotifyIndexSeqMap UserMessagePool::GetSeqByTypeAndBigtypeAll(int userid, const MyUtil::IntSeq& types,int begin, int limit, int bigtype){
	NotifyIndexSeqMap ans;
	return ans;
}
NotifyIndexSeq UserMessagePool::GetSeqBeforLastNid(int userid, int view, Ice::Long nid, int limit, int bigtype){
	NotifyIndexSeq ans;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		ans = usermessage->GetSeqBeforLastNid(view, nid, limit, bigtype);
	}
	MCE_INFO("UserMessagePool::GetSeqBeforLastNid--> userid:" << userid << " view:" << view << " limit:" << limit << " bigtype:" << bigtype << " ans.size:" << ans.size());
	return ans;
}
NotifyIndexSeq UserMessagePool::GetSeqBeforMerge(int userid, int view, Ice::Long nid, int limit, int bigtype){
	NotifyIndexSeq ans;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		MCE_INFO("UserMessagePool::GetSeqBeforMerge --> userid:" << userid << " view:" << view << " nid:" << nid << " limit:" << limit << " bigtype:" << bigtype );
		ans = usermessage->GetSeqBeforMerge(view, nid, limit, bigtype);
	}
	MCE_INFO("UserMessagePool::GetSeqBeforMerge--> userid:" << userid << " view:" << view << " limit:" << limit << " bigtype:" << bigtype << " ans.size:" << ans.size());
	return ans;
}


int UserMessagePool::GetUnreadCountByBigtype(int userid, int bigtype){
	int ans = 0;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		return usermessage->GetUnreadCountByBigtype(bigtype);
	}
	return ans;

}

int UserMessagePool::GetUndealCountByBigtype(int userid, int bigtype){
	int ans = 0;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		return usermessage->GetUndealCountByBigtype(bigtype);
	}
	return ans;
}
int UserMessagePool::GetUndealCountByType(int userid, int type, int bigtype){
	int ans = 0;
	UserMessagePtr usermessage = LocateUserMessage(userid);
	if(usermessage){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		return usermessage->GetUndealCountByType(type, bigtype);
	}
	return ans;
}

//=========================================================================================

void UserMessagePool::UserMessageEvict(int userid){
	IceUtil::Mutex::Lock lock(_cachemutex);
	map<int, int>::iterator it = _user_time.find(userid);
	if(it == _user_time.end()){
		_user_time.insert(make_pair<int, int>(userid, time(NULL)));	
	}else{
		it->second = time(NULL);
	}
	MCE_INFO("UserMessagePool::UserMessageEvict-->userid:" << userid);
}

void UserMessagePool::DoEvict(){
	int userssize = 0, evictcount = 0, leftcount = 0;
	map<int,int> user_time;
	{
		IceUtil::Mutex::Lock lock(_cachemutex);
		user_time.swap(_user_time);
	}
	//if(user_time.empty()){
	//	return;
	//}
	userssize = user_time.size();
	for(map<int, int>::iterator it = user_time.begin(); it != user_time.end();){
		//if(time(NULL) - it->second >= 10 * 60){
		if(time(NULL) - it->second >= 30 * 60){
			int userid = it->first;
			UserMessagePtr usermessage = FindUserMessage(userid);
			if(usermessage){
				IceUtil::RWRecMutex::RLock lock(_rwmutex);
				usermessage->Evict();
				evictcount++;
			}
			user_time.erase(it++);
		}else{
			++it;
		}
	}
	{
		IceUtil::Mutex::Lock lock(_cachemutex);
		_user_time.swap(user_time);
		leftcount = user_time.size();
	}
	//MCE_INFO("UserMessagePool::DoEvict-->evictusers.size:" << userssize << " evictcount:" << evictcount << " leftcount:" << leftcount);
}




void LoadMessageTask::handle(){
	Statement sql;
	list<NotifyIndex> seq;
	MCE_DEBUG("LoadMessageTask::handle--> userid:" << _userid << " bigtype:" << _bigtype << " min_message_id:" << _min_message_id << " loadcount" << _loadcount);
	ostringstream oss;
	map< pair<int, long>, int > merge;
	while(true){
		sql.clear();
		oss.str("");
		oss << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << _min_message_id <<" and bigtype = " << _bigtype << " and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _loadcount; 
		MCE_INFO("LoadMessageTask:-->" << oss.str());
		//sql << "select id, type, source, from_id ,bigtype from " << GetIndexTableName(_userid) << " where to_id =" << _userid << " and id < " << _min_message_id <<" and type in ( " << oss.str() << ") and ( id < 13903218553 or id > 13915001511 ) order by id desc limit " <<  _loadcount; 
		sql << oss.str();
		mysqlpp::StoreQueryResult res = QueryRunner(GetDbInstance(), CDbRServer, GetIndexTableName(_userid)).store(sql);
		if (res && res.num_rows() > 0) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				NotifyIndex index; 
				mysqlpp::Row row = res.at(i);
				index.id = (Ice::Long) row["id"];
				if(_min_message_id > index.id){
					_min_message_id = index.id;
				}
				if(_blacklist_nids.find(index.id) != _blacklist_nids.end()){
					MCE_DEBUG("LoadMessageTask::handle--> id:" << index.id << " IN blacklist so do not put in!");
					continue;
				}
				int bigtype = (int)row["bigtype"];
				int type = (int)row["type"];
				type |= (bigtype << 16);
				index.type = type;
				index.source = (Ice::Long) row["source"];
				index.from = (Ice::Int) row["from_id"];
				//index.isreply = (Ice::Int) row["is_reply"];
				//index.isreply = (Ice::Int) row["is_reply"];
				merge[make_pair<int, long>((int)row["type"], index.source)] += 1;
				seq.push_back(index);
			} 
		}
		if(!res){
			break;
		}
		if(res && res.num_rows() < _loadcount){
			NotifyIndexManagerI::instance().UpdateUserBigtypeLoadStat(_userid, _bigtype, false);
			break;
		}
		if((int)seq.size() >= _loadcount){
			break;
		}
		//if((int)merge.size() >= _loadcount){
		//	break;
		//}
	}
	//if(!seq.empty()){
	NotifyIndexManagerI::instance().AppendUserRMessages(_userid, _bigtype, seq, true);
	//}
	/*
	if(res && res.num_rows() < _loadcount){
		MCE_DEBUG("LoadMessageTask::handle--> userid:" << _userid << " _bigtype:" << _bigtype << " res.size:" << res.num_rows() << " _loadcount:" << _loadcount);
		NotifyIndexManagerI::instance().UpdateUserBigtypeLoadStat(_userid, _bigtype, false);
	}
	*/
}

void MessageEvictTimer::handle(){
	NotifyIndexManagerI::instance().UserRMessageDoEvict();
}

void UpdateMinUnreadIdTask::handle(){
	
	try {
		Statement sql;
		sql << "INSERT INTO " << GetInfoTableName(_userid) << "(id, min_unread_nid)  VALUES (" << _userid << ", " << _min_unread_id << ") ON DUPLICATE KEY UPDATE min_unread_nid=" << _min_unread_id;
		mysqlpp::StoreQueryResult res = QueryRunner(GetDbInstance(), CDbWServer, GetInfoTableName(_userid)).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("UpdateMinUnreadIdTask::handle-->uid=" << _userid << " " << e.what());
	} catch (...) {
		MCE_WARN("UpdateMinUnreadIdTask::handle-->" << _userid);
	}
	
}




