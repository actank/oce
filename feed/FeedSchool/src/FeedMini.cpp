/*
 * FeedItem.cpp
 *
 *  Created on: Sep 10, 2009
 *      Author: antonio
 */
#include <set>

#include "FeedMini.h"
#include "FeedDBAdapter.h"
#include "QueryRunner.h"
#include "IceLogger.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;

using namespace boost::multi_index;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  UserFeedSetFactory* factory = new UserFeedSetFactory();
  for (size_t i = 0; i < 10; ++i) {
    ostringstream id;
    id << "US" << i;
    service.registerObjectCache(i, id.str(), factory);
  }
  service.getAdapter()->add(&FeedItemManagerI::instance(),
      service.createIdentity("FIM", ""));
  MyUtil::TaskManager::instance().scheduleRepeated(new KickUserTimer());
}

//----------------------------------------------------------------------------------------------
template<typename T>
static string Con2Str(T & t) { //将容器中的元素输出到string
  ostringstream oss;
  oss << "(";
  for (typename T::const_iterator itr = t.begin(); itr != t.end(); ++itr) {
    oss << *itr << ",";
  }
  oss << ")";
  return oss.str();
}

//----------------------------------------------------------------------------------------------


inline bool FeedMergeGroup::updateWeight() {
  FeedPtr& feed = (*_feedList.begin());
  int w = feed->getWeight() + feed->friendrank() * 10;
  if (_weight != w) {
    _weight = w;
    return true;
  }
  return false;
}

inline bool FeedMergeGroup::addFeed(const FeedPtr& feed) {
  if (feed->miniMergeType() == FeedMergeReplace) {
    _feedList.push_front(feed);
    while (_feedList.front()->feed != _feedList.back()->feed) {
      _feedList.pop_back();
    }
    return updateWeight();
  }
  // for append --
  _feedList.push_front(feed);
  for (FeedList::iterator it = ++_feedList.begin(); it != _feedList.end();) {
    if ((*it)->isValid()) {
      ++it;
    } else {
      it = _feedList.erase(it);
    }
  }
  if (_feedList.size() > 5) {
    _feedList.pop_back();
  }
  return updateWeight();
}
inline int FeedMergeGroup::type() const {
  return (*_feedList.begin())->bigType();
}
inline int FeedMergeGroup::stype() const {
  return (*_feedList.begin())->smallType();
}
inline Ice::Long FeedMergeGroup::merge() const {
  return (*_feedList.begin())->merge;
}
inline void FeedMergeGroup::remove(Ice::Long feedid) {
  //    MCE_DEBUG("FeedMergeGroup --> remove");
  for (FeedList::iterator it = _feedList.begin(); it != _feedList.end();) {
    if ((*it)->feed == feedid) {
      if (_feedList.size() > 1) {
        it = _feedList.erase(it);
      } else {
        (*it)->feed = -1;
      }
      return;
    }
    ++it;
  }
  //    MCE_DEBUG("FeedMergeGroup --> remove end");
}
inline int FeedMergeGroup::timestamp() const {
  if (_feedList.empty()) {
    return 0;
  }
  return (*_feedList.begin())->time;
}

FeedSeq FeedMergeGroup::get() {
  FeedSeq seq;
  for (FeedList::iterator it = _feedList.begin(); it != _feedList.end();) {
    if ((*it)->isValid()) {
      seq.push_back(*it);
      ++it;
    } else {
      if (_feedList.size() > 1) {
        it = _feedList.erase(it);
      }
    }
  }
  return seq;
}
FeedPtr FeedMergeGroup::getNewestFeed() {
  for (FeedList::iterator it = _feedList.begin(); it != _feedList.end();) {
    if ((*it)->isValid()) {
      return (*it);

    } else {
      if (_feedList.size() > 1) {
        it = _feedList.erase(it);
      } else {
        return 0;
      }
    }
  }
  return 0;
}
MyUtil::LongSeq FeedMergeGroup::getFeedIdSeq() {

  MyUtil::LongSeq fids;
  for (FeedList::iterator it = _feedList.begin(); it != _feedList.end();) {
    if ((*it)->isValid()) {
      fids.push_back((*it)->feed);
      ++it;
    } else {
      if (_feedList.size() > 1) {
        it = _feedList.erase(it);
      }
    }
  }
  return fids;
}

bool FeedMergeGroup::empty() {

  for (FeedList::iterator it = _feedList.begin(); it != _feedList.end();) {
    if ((*it)->isValid()) {
      return false;
    } else {
      if (_feedList.size() > 1) {
        it = _feedList.erase(it);
        continue;
      }
    }
    ++it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------
void UserFeedSet::addFeed(const FeedPtr& feed, const FeedConfigPtr& config) {
  MCE_DEBUG("UserFeedSet::addFeed  "<<"feed="<<feed->feed<<" merge="<<feed->merge);
  IceUtil::Mutex::Lock lock(_mutex);
  _addFeed(feed, config);
}

void UserFeedSet::_addFeed(const FeedPtr& feed, const FeedConfigPtr& config) {
  //	MCE_DEBUG("UserFeedSet::_addFeed  "<<"feed="<<feed->feed<<" merge="<<feed->merge);
  StypeMergeIndex& index = _mergeGroupSet.get<0> ();
  StypeMergeIndex::iterator it = index.find(boost::make_tuple(
      feed->smallType(), feed->merge));
  if (it != index.end()) {
    MCE_DEBUG("UserFeedSet::addFeed --> find in map");
    FeedMergeGroupPtr g = (*it);

    if (config && config->updateTime == 0) {
      feed->time = (*it)->timestamp();
    }

    g->addFeed(feed);
    index.replace(it, g);
  } else {
    MCE_DEBUG("UserFeedSet::addFeed --> not in map");
    FeedMergeGroupPtr g = new FeedMergeGroup(feed);
    _mergeGroupSet.insert(g);
  }
  _miniTime = _miniTime < feed->time ? _miniTime : feed->time;

}

void UserFeedSet::setRead(int stype, Ice::Long merge) {
  MCE_DEBUG("call UserFeedSet::setRead");
  //	Statement sql;
  //	sql << "delete from feed_mini_" << _user % 100 << " where user=" << _user
  //			<< " and small_type=" << stype << " and mini_merge =" << merge;
  //	ostringstream expr;
  //	expr << "feed_mini_" << _user % 100;
  //	QueryRunner(FeedDBInstance, CDbWServer, expr.str()).store(sql);
  IceUtil::Mutex::Lock lock(_mutex);
  _lastAccessTime = time(NULL);
  StypeMergeIndex& index = _mergeGroupSet.get<0> ();
  StypeMergeIndex::iterator it = index.find(boost::make_tuple(stype, merge));
  if (it != index.end()) {
    //		MCE_DEBUG("delete feed merge group\n"<<(*it)->toString())
    index.erase(it);
    if (index.size() < 6 && _loadStat == 0) {
      load();
    }
  } else {
    //MCE_DEBUG("can not find this group stype="<<stype<<"   merge="<<merge);
    MCE_INFO("UserFeedSet::setRead --> use index 1, " << _user << " " << stype << " " << merge);
    //存在bug，0号索引有问题，用1号索引再删一次
    WeightIndex& index = _mergeGroupSet.get<1> ();
    WeightIndex::iterator it = index.begin();
    for (; it != index.end(); ++it) {
      if ((*it)->stype() == stype && (*it)->merge() == merge) {
        MCE_INFO("UserFeedSet::setRead --> remove use index 1, " << _user << " " << stype << " " << merge);
        index.erase(it);
        //test
        StypeMergeIndex& index0 = _mergeGroupSet.get<0> ();
        StypeMergeIndex::iterator it0 = index0.begin();
        for (; it0 != index0.end(); ++it0) {
          MCE_INFO("UserFeedSet::setRead --> for test " << _user << " " << (*it0)->stype() << " " << (*it0)->merge());
        }
        break;
      }
    }

  }
}
void UserFeedSet::setReadAll() {
  //	Statement sql;
  //	sql << "delete from feed_mini_" << _user % 100 << " where user = " << _user;
  //	ostringstream expr;
  //	expr << "feed_mini_" << _user % 100;
  //	QueryRunner(FeedDBInstance, CDbWServer, expr.str()).schedule(sql);
  FeedMergeGroupSet tmp;
  IceUtil::Mutex::Lock lock(_mutex);
  _lastAccessTime = time(NULL);
  _mergeGroupSet.swap(tmp);
  _miniTime = 0x7fffffff;
}

FeedDataSeq UserFeedSet::getFeedDataSeq(int begin, int limit) {
  IceUtil::Mutex::Lock lock(_mutex);

  time_t t = time(NULL);
  _lastAccessTime = t;
  //	if (t - _lastSortTime > 300) {
  //		_lastSortTime = t;
  //		TaskManager::instance().execute(new SortTask(this));
  //	}

  WeightIndex& index = _mergeGroupSet.get<1> ();

  FeedDataSeq seq;
  vector<WeightIndex::iterator> itSeq;

  if (index.size() < (unsigned int) (begin + limit) + 10) {
    MCE_DEBUG("UserFeedSet::getFeedContentSeq --> _loadStat:"<<_loadStat<<"  index.size(): "<<index.size()<<"  begin: "<<begin<<"   limit: "<<limit);
    if (_loadStat == 0) {
      load();
      FeedItemManagerI::instance().addkickUser(_user);
    }
  }

  WeightIndex::iterator iter = index.begin();
  for (int k = 0; iter != index.end() && k < begin; ++iter, ++k) {
    ;
  }
  for (int h = 0; iter != index.end() && h < limit; ++iter, ++h) {
    itSeq.push_back(iter);
  }

  //	MCE_DEBUG("itSeq.size:"<<itSeq.size());
  //	for (std::vector<WeightIndex::iterator>::iterator it = itSeq.begin(); it
  //			!= itSeq.end(); ++it) {
  //		MCE_DEBUG((**it)->toString());
  //	}

  _buildFeedContents(index, itSeq, seq);
  MCE_DEBUG("UserFeedSet::getFeedContentSeq --> content.size:"<<seq.size());
  return seq;
}

FeedDataSeq UserFeedSet::getFeedDataSeqByStype(const IntSeq& stypes, int begin,
    int limit) {
  IceUtil::Mutex::Lock lock(_mutex);

  time_t t = time(NULL);
  _lastAccessTime = t;
  //		if (t - _lastSortTime > 300) {
  //			_lastSortTime = t;
  //			TaskManager::instance().execute(new SortTask(this));
  //		}

  WeightIndex& index = _mergeGroupSet.get<1> ();
  FeedDataSeq seq;
  vector<WeightIndex::iterator> itSeq;

  if (index.size() < (unsigned int) (begin + limit) + 10) {
    //			MCE_DEBUG("UserFeedSet::getFeedContentSeq --> _loadStat:"<<_loadStat<<"  index.size(): "<<index.size()<<"  begin: "<<begin<<"   limit: "<<limit);
    if (_loadStat == 0) {
      load();
      FeedItemManagerI::instance().addkickUser(_user);
    }
  }
  set<int> typeSet;
  for (size_t i = 0; i < stypes.size(); ++i) {
    typeSet.insert(stypes.at(i));
  }
  WeightIndex::iterator iter = index.begin();
  for (int k = 0; iter != index.end() && k < begin; ++iter) {
    int stype = (*iter)->stype();
    if (typeSet.find(stype) != typeSet.end()) {
      ++k;
    }
  }
  for (int h = 0; iter != index.end() && h < limit; ++iter) {
    int stype = (*iter)->stype();
    if (typeSet.find(stype) != typeSet.end()) {
      itSeq.push_back(iter);
      ++h;
    }
  }

  _buildFeedContents(index, itSeq, seq);
  MCE_DEBUG("UserFeedSet::getFeedContentSeq --> content.size:"<<seq.size());
  return seq;
}
FeedDataSeq UserFeedSet::getFeedDataSeqByType(const IntSeq& types, int begin,
    int limit) {

  IceUtil::Mutex::Lock lock(_mutex);

  time_t t = time(NULL);
  _lastAccessTime = t;
  //	if (t - _lastSortTime > 300) {
  //		_lastSortTime = t;
  //		TaskManager::instance().execute(new SortTask(this));
  //	}

  WeightIndex& index = _mergeGroupSet.get<1> ();
  FeedDataSeq seq;
  vector<WeightIndex::iterator> itSeq;

  if (index.size() < (unsigned int) (begin + limit) + 10) {
    //		MCE_DEBUG("UserFeedSet::getFeedContentSeq --> _loadStat:"<<_loadStat<<"  index.size(): "<<index.size()<<"  begin: "<<begin<<"   limit: "<<limit);
    if (_loadStat == 0) {
      load();
      FeedItemManagerI::instance().addkickUser(_user);
    }
  }
  set<int> typeSet;
  for (size_t i = 0; i < types.size(); ++i) {
    typeSet.insert(types.at(i));
  }
  WeightIndex::iterator iter = index.begin();
  for (int k = 0; iter != index.end() && k < begin; ++iter) {
    int type = (*iter)->type();
    if (typeSet.find(type) != typeSet.end()) {
      ++k;
    }
  }
  for (int h = 0; iter != index.end() && h < limit; ++iter) {
    int type = (*iter)->type();
    if (typeSet.find(type) != typeSet.end()) {
      itSeq.push_back(iter);
      ++h;
    }
  }

  _buildFeedContents(index, itSeq, seq);
  MCE_DEBUG("UserFeedSet::getFeedContentSeq --> content.size:"<<seq.size());
  return seq;
}

void UserFeedSet::load() {
  //	MCE_DEBUG("UserFeedSet::load uid = "<<_user);
  {
    //		IceUtil::Mutex::Lock lock(_mutex);
    _loadStat = 1;
  }
  //	MCE_DEBUG(__LINE__<<"_loadStat = "<< _loadStat);
  Statement sql;
  sql << "select * from feed_school_" << _user % 10 << " where user=" << _user
      << " and time < FROM_UNIXTIME(" << _miniTime
      << ") order by time desc limit " << LoadFeedCount;
  ostringstream expr;
  expr << "feed_mini_" << _user % 10;
  if(_user%10!=0){
    expr << "0";
  }
  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      expr.str()).store(sql);
  //	MCE_DEBUG("UserFeedSet::load  load "<<res.num_rows()<<"  Feed");
  if (!res || res.num_rows() <= 0) {
    //		MCE_DEBUG(__LINE__<<"_loadStat = "<< _loadStat);
    //		IceUtil::Mutex::Lock lock(_mutex);
    _loadStat = 2;
    return;
  }
  for (size_t i = 0; i < res.num_rows(); ++i) {
    //		MCE_DEBUG(__LINE__<<"_loadStat = "<< _loadStat);
    mysqlpp::Row row = res.at(i);
    FeedItem item;
    item.feed = row["feed"];
    item.merge = row["mini_merge"];
    item.time = (time_t) mysqlpp::DateTime(row["time"]);
    item.type = row["type"];
    item.weight = 0;
    FeedPtr f = new Feed(item, 0);
    FeedConfigPtr config;
    _addFeed(f, config); //加入cache
  }

  //	IceUtil::Mutex::Lock lock(_mutex);
  if (res.num_rows() < (unsigned int) LoadFeedCount) {
    _loadStat = 2;
  } else {
    _loadStat = 0;
  }
}

template<class Index>
void UserFeedSet::_buildFeedContents(Index& index, std::vector<
    typename Index::iterator>& itSeq, FeedDataSeq& seq) {
  MCE_DEBUG("UserFeedSet::BuildFeedContents --> iterator.size:"<< itSeq.size()<< ", ContentSeq.size:"<<seq.size());
  MyUtil::LongSeq fids;
  for (typename std::vector<typename Index::iterator>::iterator it =
      itSeq.begin(); it != itSeq.end(); ++it) {
    MyUtil::LongSeq ids = (**it)->getFeedIdSeq();//查找这个MergeGroup所有的feedid
    fids.insert(fids.end(), ids.begin(), ids.end());
  }
  if (fids.empty()) {
    return;
  }
  FeedContentDict fdict = FeedContentAdapter::instance().getFeedDict(fids);//根据feedid查找feedcontent
  MCE_DEBUG("_buildFeedContents --> get FeedContent content.size:"<<fdict.size());
  for (typename std::vector<typename Index::iterator>::iterator wit =
      itSeq.begin(); wit != itSeq.end(); ++wit) {
    FeedSeq feedSeq = (**wit)->get();
    if (feedSeq.size() == 1) { //这个MergeGroup只有一个feed
      FeedContentDict::iterator it = fdict.find(feedSeq.at(0)->feed);
      if (it != fdict.end()) {
        if (it->second) {
          //因为feed_content表中的type为负时会为0，所以需要重新负一下值
          it->second->data->type = feedSeq.at(0)->type;
          //------

          ostringstream xml;
          xml << it->second->data->xml;
          if (it->second->reply) {
            xml << it->second->reply->oldReply << it->second->reply->newReply
                << "<reply_count>" << it->second->reply->count
                << "</reply_count>";
            it->second->reply = 0;
          }

          it->second->data->xml = xml.str();
          seq.push_back(it->second->data);
          //TODO do feedback
          //            MCE_DEBUG("_buildFeedContents --> weight:"<< it->second->data->weight << "  file:"<<__FILE__<<" line:"<<__LINE__);
          //            feedSeq.at(0)->setWeight(it->second->data->weight);
          //            MCE_DEBUG("_buildFeedContents  weight:"<<(it->second->data->weight & 0x0000ffff));
          //            if ((**wit)->updateWeight()) {
          //              index.replace(*wit, **wit);
          //            };
          //----
        } else {
          (**wit)->remove(it->first);
          if ((**wit)->empty()) {
            index.erase(*wit);
          }
        }
      }

    } else if (feedSeq.size() > 1) { //MergeGroup有多个feed
      FeedDataPtr data;
      FeedReplyPtr reply;
      ostringstream xml;
      for (size_t i = 0; i < feedSeq.size(); ++i) {
        Ice::Long fid = feedSeq.at(i)->feed;
        FeedContentDict::iterator it = fdict.find(fid);
        if (it == fdict.end()) {
          continue;
        }

        if (!it->second) { //FeedContentDict没有feedid对应的FeedData和FeedReply
          (**wit)->remove(it->first);
          if ((**wit)->empty()) {
            index.erase(*wit);
          }
          continue;
        }

        //          feedSeq.at(i)->setWeight(it->second->data->weight);
        if (!data) { //FeedDataPtr没有被赋值
          data = it->second->data;
          reply = it->second->reply;
          //TODO do feedback
          //            if ((**wit)->updateWeight()) {
          //              index.replace(*wit, **wit);
          //            };
          //---
        }

        //          MCE_DEBUG("xml:"<<it->second->data->xml);
        xml << it->second->data->xml; //拼接xml
      }
      if (data) {
        if (reply) {
          xml << reply->oldReply << reply->newReply << "<reply_count>"
              << reply->count << "</reply_count>";
        }
        data->xml = xml.str();
        seq.push_back(data);
      }

    }

  }

}

//返回user最近limit个feedgroup中所有的FeedItem
void UserFeedSet::GetFeedItemSeqSeq(int limit, FeedItemSeqSeq & itemseqseq) { //一个ItemSeq包含一个MergeGroup中所有的Feed
  IceUtil::Mutex::Lock lock(_mutex);
  if (_mergeGroupSet.empty()) {
    return;
  }
  WeightIndex& index = _mergeGroupSet.get<1> ();
  WeightIndex::iterator wit = index.begin();
  for (int k = 0; wit != index.end() && k < limit; ++wit, ++k) { //取出limit个MergeGroup
    vector<FeedPtr> tmp = (*wit)->get();
    /*    if (tmp.size() == 0) {
     continue;
     }*/
    FeedItemSeq itemseq;
    for (vector<FeedPtr>::iterator lit = tmp.begin(); lit != tmp.end(); ++lit) {
      /*      if (!(*lit)) { //可能这个FeedPtr是空指针
       continue;
       }*/
      FeedPtr fp = (*lit);
      FeedItem fi;
      fi.actor = _user;
      fi.feed = fp->feed;
      fi.merge = fp->merge;
      fi.time = fp->time;
      itemseq.push_back(fi);
    }
    itemseqseq.push_back(itemseq);
  }
  return;
}

//---------------------------------------------------------------------------

Ice::ObjectPtr UserFeedSetFactory::create(Ice::Long id) {
  MCE_DEBUG("UserFeedSetFactory::create --> uid = "<< id);

  UserFeedSetPtr us = new UserFeedSet(id);
  us->load();
  return us;
}

//---------------------------------------------------------------------------


FeedDataSeq FeedItemManagerI::getFeedDataByStype(int uid,
    const MyUtil::IntSeq& stypes, int begin, int limit, const Ice::Current&) {
  UserFeedSetPtr feedSet = locateUserFeedSet(uid);

  return feedSet->getFeedDataSeqByStype(stypes, begin, limit);
}

FeedDataSeq FeedItemManagerI::getFeedDataByType(int uid,
    const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current&) {
  UserFeedSetPtr feedSet = locateUserFeedSet(uid);

  return feedSet->getFeedDataSeqByType(types, begin, limit);
}

FeedDataSeq FeedItemManagerI::getFeedData(int uid, int begin, int limit,
    const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::getFeedNews --> uid:"<<uid<<", begin:"<<begin<<", limit:"<<limit);
  UserFeedSetPtr feedSet = locateUserFeedSet(uid);

  return feedSet->getFeedDataSeq(begin, limit);
}

void FeedItemManagerI::addFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
    const Ice::Current&) {
  MCE_DEBUG("FeedItemManagerI::addFeed --> feed="<<item.feed);
  //	MCE_DEBUG("FeedItemManagerI::addFeed --> feed="<<item.feed << " merge="<<item.merge << " type="<<item.type << " time="<<item.time<<"\n\ttarget-->");
  for (MyUtil::Int2IntMap::const_iterator it = id2weight.begin(); it
      != id2weight.end(); ++it) {
    UserFeedSetPtr feedSet = locateUserFeedSet(it->first);
    if (feedSet) {

      FeedPtr f = new Feed(item, it->second);
      feedSet->addFeed(f, config);
    }
  }
}

void FeedItemManagerI::setRead(int uid, int stype, Ice::Long merge,
    const Ice::Current&) {
  MCE_DEBUG("call FeedItemManagerI::setRead uid = "<<uid<<"   stype="<<stype<<"   merge="<<merge);
/*
  Statement sql;
  sql << "delete from " << _getFeedMiniTab(uid) << " where user=" << uid
      << " and small_type=" << stype << " and mini_merge =" << merge;
  ostringstream expr;
  expr << "feed_mini_" << uid % 100;
  QueryRunner(FeedDBInstance, CDbWServer, expr.str()).execute(sql);
  UserFeedSetPtr feedSet = findUserFeedSet(uid);
  feedSet->setRead(stype, merge);
*/
}
void FeedItemManagerI::setReadById(int uid, Ice::Long feedId,
    const Ice::Current&) {
/*  Statement sql;
  sql << "select * from " << _getFeedMiniTab(uid) << " where user=" << uid
      << " and feed=" << feedId;

  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      _getFeedMiniTab(uid)).store(sql);

  if (!res) {
    MCE_WARN("FeedItemManagerI::setReadById --> not find in feed_mini tab   user:"<<uid << ", feedid:"<<feedId << ", line:"<<__LINE__);
    _retrySetReadById(uid, feedId);
    return;
  }
  int stype = 0;
  Ice::Long merge = 0;
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row& row = res.at(i);
    stype = (int) row["small_type"];
    merge = (Ice::Long) row["mini_merge"];
  }
  if (stype == 0 || merge == 0) {
    MCE_WARN("FeedItemManagerI::setReadById --> not find in feed_mini tab   user:"<<uid << ", feedid:"<<feedId<< ", line:"<<__LINE__);
    _retrySetReadById(uid, feedId);
    return;
  }

  Statement delSql;
  delSql << "delete from " << _getFeedMiniTab(uid) << " where user=" << uid
      << " and small_type=" << stype << " and mini_merge=" << merge;
  QueryRunner(FeedDBInstance, CDbWServer, _getFeedMiniTab(uid)).store(delSql);

  UserFeedSetPtr feedSet = findUserFeedSet(uid);
  if (feedSet) {
    feedSet->setRead(stype, merge);
  }
*/
}

void FeedItemManagerI::_retrySetReadById(int uid, Ice::Long feedId) {
/*  Statement sql;
  sql << "select * from " << _getFeedIndexTab(feedId) << " where feed="
      << feedId;

  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      _getFeedIndexTab(feedId)).store(sql);

  if (!res) {
    MCE_WARN("FeedItemManagerI::_retrySetReadById --> not find in feed_mini tab   user:"<<uid << ", feedid:"<<feedId << ", line:"<<__LINE__);
    return;
  }
  int stype = 0;
  Ice::Long merge = 0;
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row& row = res.at(i);
    stype = (int) row["small_type"];
    merge = (Ice::Long) row["mini_merge"];
  }
  if (stype == 0 || merge == 0) {
    MCE_WARN("FeedItemManagerI::_retrySetReadById --> not find in feed_mini tab   user:"<<uid << ", feedid:"<<feedId<< ", line:"<<__LINE__);
    return;
  }
  UserFeedSetPtr feedSet = findUserFeedSet(uid);
  if (feedSet) {
    feedSet->setRead(stype, merge);
  }
*/
}

void FeedItemManagerI::setReadAll(int uid, const Ice::Current&) {
/*  Statement sql;
  sql << "delete from " << _getFeedMiniTab(uid) << " where user = " << uid;

  QueryRunner(FeedDBInstance, CDbWServer, _getFeedMiniTab(uid)).execute(sql);
  UserFeedSetPtr feedSet = findUserFeedSet(uid);
  feedSet->setReadAll();
*/
}

FeedDataSeq FeedItemManagerI::getFeedData2(int uid, int begin, int limit,
    const Ice::Current&) {
  return FeedDataSeq();
}

bool FeedItemManagerI::hasFeed(int uid, const Ice::Current&) {
/*  UserFeedSetPtr feedSet = findUserFeedSet(uid);
  if (feedSet) {
    return feedSet->size() > 0;
  }

  Statement sql;
  sql << "select * from feed_mini_" << uid % 100 << " where user=" << uid
      << " limit 1";
  ostringstream expr;
  expr << "feed_mini_" << uid % 100;
  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      expr.str()).store(sql);
  if (!res || res.num_rows() <= 0) {
    return false;
  }*/
  return true;

}

UserFeedSetPtr FeedItemManagerI::locateUserFeedSet(int uid) {
  return ServiceI::instance().locateObject<UserFeedSetPtr> ((uid / 100) % 10,
      uid);
}

UserFeedSetPtr FeedItemManagerI::findUserFeedSet(int uid) {
  return ServiceI::instance().findObject<UserFeedSetPtr> ((uid / 100) % 10, uid);
}

FeedItemSeqSeq FeedItemManagerI::GetFeedItemSeqSeqByUsers(
    const MyUtil::IntSeq& ids, const Ice::Current&) {
  MCE_DEBUG("FeedItemManagerI::GetFeedItemSeqSeqByUsers --> start. index = "<< ids.at(0) %20 << ",ids size = " <<ids.size() << ",ids = " << Con2Str(ids));

  FeedItemSeqSeq itemseqseq;
  for (vector<int>::const_iterator vit = ids.begin(); vit != ids.end(); ++vit) {
    UserFeedSetPtr feedSet;
    try {
      feedSet = locateUserFeedSet(*vit); //
      if (!feedSet) {
        MCE_DEBUG("FeedItemManagerI::GetFeedItemSeqSeqByUsers --> fail to get UserFeedSetPtr of user = " << *vit);
        continue;
      }
      feedSet->GetFeedItemSeqSeq(10, itemseqseq);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::GetFeedItemSeqSeqByUsers-->user = " << *vit << ", exception :" <<e);
    }
  }
  MCE_DEBUG("FeedItemManagerI::GetFeedItemSeqSeqByUsers --> ids size = " <<ids.size() << ",ids = " << Con2Str(ids) << ",itemseqseq size = " << itemseqseq.size());

  return itemseqseq;
}
