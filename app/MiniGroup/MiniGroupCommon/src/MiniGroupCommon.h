/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupCommon.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 04:05:50 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __MINIGROUP_COMMON_H__
#define __MINIGROUP_COMMON_H__

#include "MiniGroup.h"

#include <map>
#include <Ice/Ice.h>

#include "QueryRunner.h"
#include "Singleton.h"

namespace xce {
namespace mngp {
  using namespace std;
  using namespace com::xiaonei::xce;

  const string DB_SOURCE = "minigroup";
  const Ice::Int MINIGROUP_INFO = 0;
  const Ice::Int USER_2_MINIGROUP_INFO = 1;
  const string TABLE_MINIGROUP = "minigroup";
  const string TABLE_MEMBER_MINIGROUP = "member_minigroup";
  const string TABLE_MINIGROUP_ID = "id";
  const string TABLE_MINIGROUP_NAME = "name";
  const string TABLE_MINIGROUP_ICON = "icon";
  const string TABLE_MEMBER_MINIGROUP_MNGP_ID = "minigroup_id";
  const string TABLE_MEMBER_MINIGROUP_MEMBER_ID = "member_id";
  const string TABLE_MEMBER_MINIGROUP_RESTRICT = "access_id >= 1 and access_id <= 3 and (is_left = 1 or is_left = 0)";
  const string TABLE_MEMBER_MINIGROUP_SORT = " order by member_id, is_left desc, update_time desc";
  const int TABLE_MOD = 100;

  const static int TASK_RELOAD_BY_USERID = 1;
  const static int TASK_RELOAD_BY_MINIGROUPID = 2;

  const int PRODUCE_BATCH_SIZE = 500;
  const int CONSUME_BATCH_SIZE = 500;
  const int CONSUME_BATCH_THREAD_SIZE = 5;
  const int WRITE_BATCH_SIZE = 1000;
  const int PRX_DATAQUEUE_MAX_SIZE = 50000;

  class MiniGroupInfo : virtual public MiniGroup, virtual public IceUtil::Mutex {
    public:
      MiniGroupInfo(long id, const string& name, const string& icon);
      MiniGroupInfo(long id, const string& name, const string& icon, const MyUtil::Str2StrMap& extraInfo);
      MiniGroupInfo(const MiniGroupPtr& ptr);
      void setId(long id);
      void setName(const string& name);
      void setIcon(const string& icon);
      long getId();
      string getName();
      string getIcon();
      MyUtil::Str2StrMap getExtraInfo();
      void addToExtraInfo(const string& key, const string& value);
  };

  typedef IceUtil::Handle<MiniGroupInfo> MiniGroupInfoPtr;

  class User2MiniGroupInfo : public User2MiniGroup, virtual public IceUtil::Mutex {
    public:
      User2MiniGroupInfo(int user_id, const MyUtil::LongSeq& minigroup_ids);
      User2MiniGroupInfo(const User2MiniGroupPtr& ptr);
      void setJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids);
      void addJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids, bool addDirectly = false);
      void removeJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids);
      void clearJoinedMiniGroups();
      MyUtil::LongSeq getJoinedMiniGroupIds();
  };

  typedef IceUtil::Handle<User2MiniGroupInfo> User2MiniGroupInfoPtr;

  template<typename T, typename TPtr, typename SPtr>
  MyUtil::ObjectResultPtr parse(const MyUtil::ObjectResultPtr& sourceData) {
    MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
    for (map<long, Ice::ObjectPtr>::const_iterator it = sourceData->data.begin(); it != sourceData->data.end(); ++it) {
      result->data.insert(make_pair<long, TPtr>((*it).first, new T(SPtr::dynamicCast((*it).second))));
    }
    return result;
  }
  
  MyUtil::ObjectResultPtr parse(const MiniGroupSeq& miniGroups) {
    MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
    for (vector<MiniGroupPtr>::const_iterator it = miniGroups.begin(); it != miniGroups.end(); ++it) {
      result->data.insert(make_pair<long, MiniGroupInfoPtr>((*it)->id, new MiniGroupInfo(*it)));
    }
    return result;
  }

  class MiniGroupInfoResultHandlerI : public com::xiaonei::xce::ResultHandler {
    public: 
      MiniGroupInfoResultHandlerI(vector<MiniGroupInfoPtr>& minigroupinfo_ptrs);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      vector<MiniGroupInfoPtr>& minigroupinfo_ptrs_;
  };

  class User2MiniGroupInfoResultHandlerI : public com::xiaonei::xce::ResultHandler {
    public: 
      User2MiniGroupInfoResultHandlerI(vector<User2MiniGroupInfoPtr>& user2minigroup_ptrs);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      vector<User2MiniGroupInfoPtr>& user2minigroupinfo_ptrs_;
  };  

  class UserIdsProducerHandlerI : public com::xiaonei::xce::ResultHandler {
    public:
      UserIdsProducerHandlerI(vector<int>& ids, int& max_id);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      vector<int>& result_ids_;
      int& max_id_;
  };

  template<typename V>
  class DBUtil {
    public:
      template<typename T>
      static string getTableName(T id, T mod, const string& prefix);
      //将一个vector中的元素拼接成str, 例如vector中含有[0, -1, 3], 结果为"0, -1, 3"
      template<typename T>
      static string getIdsStr(const vector<T>& ids);
      //将一个数据(int 或者 long型的，按照mod划分成不同的数据，例如输入为[1, 3, 2, 8, 22, 10, 4, 12, 3, 4], mod = 10, 结果为
      //{0->[10], 1->[1], 2->[2, 22, 12], 3->[3, 3], 4->[4, 4], 8->[8]}
      template<typename T>
      static map<T, vector<T> > seperateIdsByMod(int mod, const vector<T>& ids);
    private:
      V value;
  };

  template<typename T>
  class BatchIdResultHandler : public com::xiaonei::xce::ResultHandler {
    public:
      BatchIdResultHandler(const string& idColumnName, vector<T>& ids);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      string _idColumnName;
      vector<T>& _ids;
  };

  MiniGroupInfo::MiniGroupInfo(long pid, const string& pname, const string& picon) {
    this->id = pid;
    this->name = pname;
    this->icon = picon;
  }

  MyUtil::Str2StrMap MiniGroupInfo::getExtraInfo() {
    IceUtil::Mutex::Lock lock(*this);
    return this->extraInfo;
  }
  
  MiniGroupInfo::MiniGroupInfo(const MiniGroupPtr& ptr) : MiniGroup(ptr->id, ptr->name, ptr->icon, ptr->extraInfo) {
  }

  void MiniGroupInfo::addToExtraInfo(const string& key, const string& value) {
    IceUtil::Mutex::Lock lock(*this);
    this->extraInfo.insert(make_pair(key, value));
  }

  MiniGroupInfo::MiniGroupInfo(long id, const string& name, const string& icon, 
    const MyUtil::Str2StrMap& extraInfo) : MiniGroup(id, name, icon, extraInfo) {
  }

  void MiniGroupInfo::setId (long id) {
    IceUtil::Mutex::Lock lock(*this);
    this->id = id;
  }

  void MiniGroupInfo::setName(const string& name) {
    IceUtil::Mutex::Lock lock(*this);
    this->name = name;
  }

  void MiniGroupInfo::setIcon(const string& icon) {
    IceUtil::Mutex::Lock lock(*this);
    this->icon = icon;
  }

  long MiniGroupInfo::getId() {
    IceUtil::Mutex::Lock lock(*this);
    return this->id;
  }

  string MiniGroupInfo::getName() {
    IceUtil::Mutex::Lock lock(*this);
    return this->name;
  }

  string MiniGroupInfo::getIcon() {
    IceUtil::Mutex::Lock lock(*this);
    return this->icon;
  }
      
  User2MiniGroupInfo::User2MiniGroupInfo(int user_id, const MyUtil::LongSeq& minigroup_ids) : User2MiniGroup(user_id, minigroup_ids) {
  }
      
  User2MiniGroupInfo::User2MiniGroupInfo(const User2MiniGroupPtr& ptr) : User2MiniGroup(ptr->userId, ptr->miniGroupIds) {
  }

  void User2MiniGroupInfo::User2MiniGroupInfo::setJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids) {
    IceUtil::Mutex::Lock lock(*this);
    this->miniGroupIds = minigroup_ids;
  }

  void User2MiniGroupInfo::addJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids, bool addDirectly) {
    IceUtil::Mutex::Lock lock(*this);
    if (addDirectly) {
      this->miniGroupIds.insert(this->miniGroupIds.end(), minigroup_ids.begin(), minigroup_ids.end());
      return;
    }
    for (MyUtil::LongSeq::const_iterator it = minigroup_ids.begin(); it != minigroup_ids.end(); ++it) {
      MyUtil::LongSeq::iterator found_it = find(this->miniGroupIds.begin(), this->miniGroupIds.end(), *it);
      if (found_it == this->miniGroupIds.end()) {
        this->miniGroupIds.push_back(*it);
      }
    }
  }

  void User2MiniGroupInfo::removeJoinedMiniGroups(const MyUtil::LongSeq& minigroup_ids) {
    IceUtil::Mutex::Lock lock(*this);
    for (MyUtil::LongSeq::const_iterator it = minigroup_ids.begin(); it != minigroup_ids.end(); ++it) {
      MyUtil::LongSeq::iterator found_it = find(this->miniGroupIds.begin(), this->miniGroupIds.end(), *it);
      if (found_it != this->miniGroupIds.end()) {
        this->miniGroupIds.erase(found_it);
      }
    }
  }

  void User2MiniGroupInfo::clearJoinedMiniGroups() {
    IceUtil::Mutex::Lock lock(*this);
    this->miniGroupIds.clear();
  }

  MyUtil::LongSeq User2MiniGroupInfo::getJoinedMiniGroupIds() {
    IceUtil::Mutex::Lock lock(*this);
    return this->miniGroupIds;
  }

  UserIdsProducerHandlerI::UserIdsProducerHandlerI(vector<int>& ids, int& id) : result_ids_(ids), max_id_(id) {
  }

  bool UserIdsProducerHandlerI::handle(mysqlpp::Row& row) const {
    int id = (int) row[TABLE_MEMBER_MINIGROUP_MEMBER_ID.c_str()];
    result_ids_.push_back(id);
    if (id > max_id_) {
      max_id_ = id;
    }
    return true;
  }

  MiniGroupInfoResultHandlerI::MiniGroupInfoResultHandlerI(vector<MiniGroupInfoPtr>& mnGroupInfoPtrs) : minigroupinfo_ptrs_(mnGroupInfoPtrs) {
  }

  bool MiniGroupInfoResultHandlerI::handle(mysqlpp::Row& row) const {
    long id = (long)row[TABLE_MINIGROUP_ID.c_str()];
    string name = (string)row[TABLE_MINIGROUP_NAME.c_str()];
    string icon = (string)row[TABLE_MINIGROUP_ICON.c_str()];
    MiniGroupInfoPtr ptr = new MiniGroupInfo(id, name, icon);
    //MCE_INFO("[MiniGroupInfoResultHandlerI::handle] id = " << id << ", name = " << name << ", icon = " << icon);
    minigroupinfo_ptrs_.push_back(ptr);
    return true;
  }

  User2MiniGroupInfoResultHandlerI::User2MiniGroupInfoResultHandlerI(vector<User2MiniGroupInfoPtr>& user2ptrs) : user2minigroupinfo_ptrs_(user2ptrs) {
  }

  bool User2MiniGroupInfoResultHandlerI::handle(mysqlpp::Row& row) const {
    long minigroup_id = (long)row[TABLE_MEMBER_MINIGROUP_MNGP_ID.c_str()];
    int memberId = (int)row[TABLE_MEMBER_MINIGROUP_MEMBER_ID.c_str()];
    //MCE_INFO("[User2MiniGroupResultHandlerI::handle] minigroup_id = " << minigroup_id << " memberId = " << memberId);
    MyUtil::LongSeq ids;
    ids.push_back(minigroup_id);
    //MCE_INFO("[User2MiniGroupResultHandlerI::handle] minigroupId = " << minigroup_id << " memberId = " << memberId);
    //要这样做必须保证查询memeber_group表的sql语句中一定要包括group by member_id, 否则下面的逻辑就是不对的
    if (user2minigroupinfo_ptrs_.empty() || (*(user2minigroupinfo_ptrs_.end() - 1))->userId != memberId) {
      User2MiniGroupInfoPtr cur_ptr = new User2MiniGroupInfo(memberId, ids);
      user2minigroupinfo_ptrs_.push_back(cur_ptr);
    } else {
      (*(user2minigroupinfo_ptrs_.end() - 1))->addJoinedMiniGroups(ids, true);
    }
    //MCE_INFO("[User2MiniGroupResultHandlerI::handle] user_id = " << (*(user2minigroupinfo_ptrs_.end() - 1))->userId 
    //    << " joined size = " << (*(user2minigroupinfo_ptrs_.end() - 1))->miniGroupIds.size());
    //MCE_INFO("[User2MiniGroupResultHandlerI::handle] user2MiniGroupPtrs.size = " << user2minigroupinfo_ptrs_.size());
    return true;
  }

  template<typename V>
  template<typename T>
  string DBUtil<V>::getTableName(T id, T mod, const string& prefix) {
    return prefix + "_" + boost::lexical_cast<string>(id % mod);
  }

  template<typename V>
  template<typename T>
  string DBUtil<V>::getIdsStr(const vector<T>& ids) {
    ostringstream result;
    result << "(";
    if(ids.size() > 0) {
      result << boost::lexical_cast<string>(ids[0]);
      for(size_t i = 1; i < ids.size(); ++i) {
        result << ", " << boost::lexical_cast<string>(ids[i]);
      }
    }
    result << ")";
    return result.str();
  }

  template<typename V>
  template<typename T>
  map<T, vector<T> > DBUtil<V>::seperateIdsByMod(int mod, const vector<T>& ids) {
    map<T, vector<T> > result;
    for(size_t i = 0; i < ids.size(); ++i) {
      typename map<T, vector<T> >::iterator foundIt = result.find(ids[i] % mod);
      if(foundIt == result.end()) {
        vector<T> modVector;
        modVector.push_back(ids[i]);
        result.insert(make_pair<T, vector<T> >(ids[i] % mod, modVector));
      } else {
        foundIt->second.push_back(ids[i]);
      }
    }
    return result;
  }

  template<typename T>
  BatchIdResultHandler<T>::BatchIdResultHandler(const string& idColumnName, vector<T>& ids) : _idColumnName(idColumnName), _ids(ids) {
  } 

  template<typename T>
  bool BatchIdResultHandler<T>::handle(mysqlpp::Row& row) const {
    T id = (T)row[_idColumnName.c_str()];
    _ids.push_back(id);
    return true;
  }

}
}

#endif
