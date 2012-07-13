#include "NotifyIndexAdapter.h"
#include "NotifyContentAdapter.h"
#include "quicklz.h"
#include <boost/lexical_cast.hpp>

using namespace xce::notify;

// TODO : code review
NotifyBody::NotifyBody(NotifyContentPtr notify) :
  id(notify->id), source(notify->source), owner(notify->owner), from(
      notify->from), type(notify->type & 0x7FFFFFFF), time(notify->time), bigtype(GetRMessageBigtypeN(notify->type)) {

	BIGTYPE_LEFTMOVEN = 16;
	BIGTYPE_ANDVALUEN = 16711680;
	TYPE_ANDVALUEN = 65535;

  //important!! props 中key-value的分隔符是 \t
  //解压缩
  string propsStr;
  //MCE_DEBUG("NotifyBody::NotifyBody --> type="<<notify->type << " nid:" << id);
  if ((notify->type & 0x80000000) == 0x80000000) {
  //if (false) {
    //MCE_DEBUG("NotifyBody::NotifyBody --> decompress");
    char* scratch = new char[QLZ_SCRATCH_DECOMPRESS];
    size_t dataLen = qlz_size_decompressed(notify->props.data());
    MCE_DEBUG("dataLen = " << dataLen);
    char* decompressed = new char[dataLen + 2];
    qlz_decompress(notify->props.data(), decompressed, scratch);
    decompressed[dataLen] = '\t';
    decompressed[dataLen + 1] = '\0';
    propsStr = decompressed;
    delete scratch;
    delete decompressed;
  } else {
    MCE_DEBUG("NotifyBody::NotifyBody --> NO decompress");
    propsStr.swap(notify->props);
  }
  
	//MCE_DEBUG("NotifyBody::NotifyBody --> props="<<propsStr);
	
	
	//get real type for rmessage
	type = GetRMessageTypeN(type);

  size_t pos = 0;
  size_t tail = 0;
  string key;
  string val;

  while (pos < propsStr.length()) {
    tail = propsStr.find('\t', pos);
    if (tail == string::npos) {
      break;
    }
    key = propsStr.substr(pos, tail - pos);
    pos = tail + 1;

    tail = propsStr.find('\t', pos);
    if (tail == string::npos) {
      tail = propsStr.length();
    }
    val = propsStr.substr(pos, tail - pos);
    pos = tail + 1;
    props[key] = val;
  }
	props["bigtype"] = boost::lexical_cast<string>(bigtype);
}


// TODO : 并行调用service
void NotifyIndexAdapter::addNotify(const NotifyContentPtr& r,
    const MyUtil::IntSeq& uids) {
  map<int, MyUtil::IntSeq> uidMap;
  for (size_t i = 0; i < uids.size(); ++i) {
    uidMap[uids.at(i) % cluster()].push_back(uids.at(i));
  }
  for (map<int, MyUtil::IntSeq>::iterator it = uidMap.begin(); it
      != uidMap.end(); ++it) {
    getManagerOneway(it->first)->addNotify(r, it->second);
  }
}

Ice::Int NotifyIndexAdapter::getUnreadCount(Ice::Int uid) {
  return getManager(uid)->getUnreadCount(uid);
}

Ice::Int NotifyIndexAdapter::getUnreadCountByView(Ice::Int uid,int view) {
  return getManager(uid)->getUnreadCountByView(uid,view);
}

void NotifyIndexAdapter::removeById(Ice::Int uid, Ice::Long notifyId) {
  getManagerOneway(uid)->removeById(uid, notifyId);
}

void NotifyIndexAdapter::removeByIds(Ice::Int uid, const MyUtil::LongSeq& nids) {
  if (!nids.empty()) {
    getManagerOneway(uid)->removeByIds(uid, nids);
  }
}

void NotifyIndexAdapter::removeBySource(Ice::Int uid, Ice::Int type,
    Ice::Long source) {
  getManagerOneway(uid)->removeBySource(uid, type, source);
}

void NotifyIndexAdapter::removeBySourceAndSender(Ice::Int uid, Ice::Int type,
    Ice::Long source,Ice::Int sender) {
  getManagerOneway(uid)->removeBySourceAndSender(uid, type, source,sender);
}


void NotifyIndexAdapter::conentSeq2BodySeq(const NotifyContentSeq & contentSeq,
    NotifyBodySeq& bodySeq) {
  for (size_t h = 0; h < contentSeq.size(); ++h) {
    bodySeq.push_back(new NotifyBody(contentSeq.at(h)));
  }
}

NotifyBodySeq NotifyIndexAdapter::getNotifySeq(Ice::Int uid, Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getNotifySeq(uid, nid, limit);
  NotifyBodySeq bodySeq;

  conentSeq2BodySeq(contentSeq, bodySeq);

  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getReplySeq(Ice::Int uid, Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getReplySeq(uid, nid, limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getByType(Ice::Int uid,
    const MyUtil::IntSeq& type, Ice::Long nid, Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getByType(uid, type, nid,
      limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getNotifySeq2(Ice::Int uid, Ice::Int begin,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getNotifySeq2(uid, begin,
      limit);
  NotifyBodySeq bodySeq;

  conentSeq2BodySeq(contentSeq, bodySeq);

  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getReplySeq2(Ice::Int uid, Ice::Int begin,
    Ice::Int limit) {
  NotifyContentSeq contentSeq =
      getManager(uid)->getReplySeq2(uid, begin, limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}

/*NotifyBodySeq NotifyIndexAdapter::getNotifySeqMerge(Ice::Int uid, Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getNotifySeqMerge(uid, nid, limit);
  NotifyBodySeq bodySeq;

  conentSeq2BodySeq(contentSeq, bodySeq);

  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getReplySeqMerge(Ice::Int uid, Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getReplySeqMerge(uid, nid, limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}*/

NotifyBodySeq NotifyIndexAdapter::getNotifySeqX(Ice::Int uid, Ice::Int view,Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getNotifySeqX(uid,  view,nid, limit);
  NotifyBodySeq bodySeq;

  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}

NotifyBodySeq NotifyIndexAdapter::getReplySeqX(Ice::Int uid, Ice::Int view,Ice::Long nid,
    Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getReplySeqX(uid,  view,nid, limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}

bool NotifyIndexAdapter::reloadSchema(){
  MCE_DEBUG("NotifyIndexAdapter::reloadSchema. begin");
  for(size_t i = 0; i < cluster(); ++i)
  {
    MCE_DEBUG("NotifyIndexAdapter::reloadSchema. reload service:" << i);
    getManager(i)->reloadSchema();
  }
  return true;
}

NotifyBodySeq NotifyIndexAdapter::getByType2(Ice::Int uid,
    const MyUtil::IntSeq& type, Ice::Int begin, Ice::Int limit) {
  NotifyContentSeq contentSeq = getManager(uid)->getByType2(uid, type, begin,
      limit);
  NotifyBodySeq bodySeq;
  conentSeq2BodySeq(contentSeq, bodySeq);
  return bodySeq;
}
//====================================================RMESSAGE================
/*
NotifyBodySeq NotifyIndexAdapter::GetNotifyBodyByNotifyIndex(const NotifyIndexSeq& indexseq, vector<Ice::Long>& not_in_db_nids){
	
	MyUtil::LongSeq indexids;
	NotifyBodySeq bodyseq;
	NotifyContentSeq contents;
	NotifyContentDict contentDict;

	for(NotifyIndexSeq::const_iterator logit = indexseq.begin(); logit != indexseq.end(); ++logit){
		NotifyIndex index = *logit;
		indexids.push_back(index.id);
		//MCE_DEBUG("GetRMessageRequest::response-->id:" << index.id << " type:" << index.type << " index.source:" << index.source << " index.from:" << index.from << " _login_uid:" << _login_uid);
	}

	//GetContent
	try {
		contentDict = NotifyContentAdapter::instance().getNotifyDict(indexids);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyIndexAdapter::GetNotifyBodyByNotifyIndex--> NotifyContentAdapter::getNotifyDict() error:" << e);
	}
	NotifyContentPtr reply;
	NotifyContentDict::iterator iter;
	//BOOST_FOREACH(Ice::Long id, indexids){
	for(vector<Ice::Long>::iterator idit = indexids.begin(); idit != indexids.end(); ++idit){
		Ice::Long id = *idit;
		iter = contentDict.find(id);
		if (iter != contentDict.end()) {
			if(!iter->second){
				not_in_db_nids.push_back(id);
				continue;
			}
			contents.push_back(iter->second);
		}
	}
	for(NotifyContentSeq::iterator logit = contents.begin(); logit != contents.end(); ++logit){
		NotifyContentPtr content = *logit;
		NotifyBodyPtr body = new NotifyBody(*logit);
		bodyseq.push_back(body);

		ostringstream os;
		for(map<string, string>::iterator it = body->props.begin(); it != body->props.end(); ++it){
			os << " key:" << it->first << " value:" << it->second << ", ";
		}
		MCE_DEBUG("NotifyIndexAdapter::GetNotifyBodyByNotifyIndex-->id:" << body->id << " source:" << body->source << " owner:" << body->owner << " from :" << body->from << " type:" << body->type << " time:" << body->time <<  " props:" << os.str());
	}
	return bodyseq;
}


NotifyBodySeq NotifyIndexAdapter::GetRMessageByBigtype(int uid, int begin, int limit, int bigtype){
	NotifyIndexSeq indexids = getManager(uid)->GetRMessageByBigtype(uid, begin, limit, bigtype);
	vector<Ice::Long> should_remove_indexs;
	NotifyBodySeq ans = GetNotifyBodyByNotifyIndex(indexids, should_remove_indexs);
	//RemoveRMessageByIds(uid, should_remove_indexs, bigtype);
	return ans;
}
NotifyBodySeq NotifyIndexAdapter::GetRMessageBeforLastNid(int userid, int view, Ice::Long nid, int limit, int bigtype){
	NotifyIndexSeq indexids = getManager(userid)->GetRMessageBeforLastNid(userid, view, nid, limit,bigtype);	
	vector<Ice::Long> should_remove_indexs;
	NotifyBodySeq ans = GetNotifyBodyByNotifyIndex(indexids, should_remove_indexs);
	//RemoveRMessageByIds(userid, should_remove_indexs, bigtype);
	return ans;
}
NotifyBodySeq NotifyIndexAdapter::GetRMessageMerge(int userid, int view, Ice::Long nid, int limit, int bigtype){
	NotifyIndexSeq indexids = getManager(userid)->GetRMessageMerge(userid, view, nid, limit, bigtype);	
	vector<Ice::Long> should_remove_indexs;
	NotifyBodySeq ans = GetNotifyBodyByNotifyIndex(indexids, should_remove_indexs);
	//RemoveRMessageByIds(userid, should_remove_indexs, bigtype);
	return ans;
}
NotifyBodySeq NotifyIndexAdapter::GetRMessageByType(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype){
	NotifyIndexSeq indexids = getManager(userid)->GetRMessageByType(userid, types, begin, limit, bigtype);
	vector<Ice::Long> should_remove_indexs;
	NotifyBodySeq ans = GetNotifyBodyByNotifyIndex(indexids, should_remove_indexs);
	//RemoveRMessageByIds(userid, should_remove_indexs, bigtype);
	return ans;
}
NotifyBodySeqMap NotifyIndexAdapter::GetRMessageByTypeAll(int userid, const MyUtil::IntSeq& types, int begin, int limit){
	NotifyBodySeqMap ans;

	NotifyIndexSeqMap indexids = getManager(userid)->GetRMessageByTypeAll(userid, types, begin, limit);
	vector<Ice::Long> should_remove_indexs;
	for(NotifyIndexSeqMap::iterator it = indexids.begin(); it != indexids.end(); ++it){
		NotifyBodySeq oneans = GetNotifyBodyByNotifyIndex(it->second, should_remove_indexs);
		ans[it->first] = oneans;
	}
	//RemoveRMessageByIds(userid, should_remove_indexs, bigtype);
	return ans;
}
int NotifyIndexAdapter::GetUndealCountByBigtype(int uid, int bigtype){
	return getManager(uid)->GetUndealCountByBigtype(uid, bigtype);	
}
int NotifyIndexAdapter::GetUndealCountByType(int uid, int type){
	return getManager(uid)->GetUndealCountByType(uid, type);	
}
MyUtil::Int2IntMap NotifyIndexAdapter::GetUndealCountByTypes(int uid, const MyUtil::IntSeq& types){
	return getManager(uid)->GetUndealCountByTypes(uid, types);
}

void NotifyIndexAdapter::TestGet1(int userid, int begin, int limit, int type, int view, int bigtype){
	if(userid % 10 == 1){
		bigtype = rand() % 3 + 1;	
		begin = rand() % 20 + 1;
		limit = rand() % 5 + 1;
		//if(userid % 100 == 1){
			MCE_INFO("NotifyIndexAdapter::TestGet1-->userid:" << userid << " begin:" << begin << " limit:" << limit << " bigtype:" << bigtype);
			for(int i = 0; i < 2; ++i){
				getManager(userid)->GetRMessageByBigtype(userid, begin, limit, bigtype);
				MyUtil::IntSeq types;
				types.push_back(14);
				types.push_back(15);
				getManager(userid)->GetRMessageByType(userid, types, begin, limit, bigtype);
				getManager(userid)->GetRMessageByTypeAll(userid, types, begin, limit);
				getManager(userid)->GetUnreadCountByBigtype(userid, bigtype);
				MyUtil::IntSeq bigtypes;
				bigtypes.push_back(1);
				bigtypes.push_back(2);
				bigtypes.push_back(3);
				getManager(userid)->GetUnreadCountByBigtypes(userid, bigtypes);
				getManager(userid)->GetUndealCountByBigtype(userid, bigtype);
				getManager(userid)->GetUndealCountByType(userid, bigtype);
				getManager(userid)->GetUndealCountByTypes(userid, types);
			}
		//}
	}
}

int NotifyIndexAdapter::GetUnreadCountByBigtype(int userid, int bigtype){
	return getManager(userid)->GetUnreadCountByBigtype(userid, bigtype);	
}

MyUtil::Int2IntMap NotifyIndexAdapter::GetUnreadCountByBigtypes(int userid, const MyUtil::IntSeq& types){
	return getManager(userid)->GetUnreadCountByBigtypes(userid, types);
}
void NotifyIndexAdapter::RemoveRMessageById(int uid, Ice::Long notifyId, int type, int bigtype){
	return getManager(uid)->RemoveRMessageById(uid, notifyId, type, bigtype);
}
void NotifyIndexAdapter::RemoveRMessageByIds(int uid, const MyUtil::LongSeq& notifyIds, int type, int bigtype){
	if(notifyIds.empty()){
		return;
	}
	return getManager(uid)->RemoveRMessageByIds(uid, notifyIds, type, bigtype);
}
void NotifyIndexAdapter::RemoveRMessageBySource(int uid, int type, Ice::Long source, int bigtype){
	return getManager(uid)->RemoveRMessageBySource(uid, type, source, bigtype);
}
void NotifyIndexAdapter::RemoveRMessageBySourceAndSender(int uid, int type, Ice::Long source, int sender, int bigtype){
	return getManager(uid)->RemoveRMessageBySourceAndSender(uid, type, source, sender,bigtype);
}
*/
//====================================================RMESSAGE================
