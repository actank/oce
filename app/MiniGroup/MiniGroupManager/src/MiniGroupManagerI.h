/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupManagerI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 04:02:16 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __MINIGROUP_CACHE_I_H__
#define __MINIGROUP_CACHE_I_H__

#include "../../MiniGroupCommon/src/MiniGroupCommon.h"
#include "ServiceI.h"

namespace xce {
namespace mngp {
  using namespace MyUtil;

  class MiniGroupCacheI : public MiniGroupCache, public MyUtil::Singleton<MiniGroupCacheI> {
    public:
      MiniGroupCacheI() : _valid(false) {}
      virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
      virtual void setData(const MyUtil::ObjectResultPtr& miniGroupData, const Ice::Current& = Ice::Current());
      bool isValid();
    private:
      IceUtil::Mutex _mutex;
      bool _valid;
  };
  
  class User2MiniGroupCacheI : public User2MiniGroupCache, public MyUtil::Singleton<User2MiniGroupCacheI> {
    public:
      User2MiniGroupCacheI() : _valid(false) {}
      virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
      virtual void setData(const MyUtil::ObjectResultPtr& user2MiniGroupData, const Ice::Current& = Ice::Current());
      bool isValid();
    private:
      IceUtil::Mutex _mutex;
      bool _valid;
  };

  class MiniGroupManagerI : public MiniGroupManager, public MyUtil::Singleton<MiniGroupManagerI> {
    public:
      MiniGroupManagerI();
      //获取用户一共参加了那几个MiniGroup
      virtual MiniGroupSeq getMiniGroups(int user_id, const Ice::Current& = Ice::Current());

      virtual MiniGroupSeq getMiniGroupsWithIds(const MyUtil::LongSeq& miniGroupIds, const Ice::Current& = Ice::Current());

      //添加多个MiniGroup，如果系统中原来有相同id的MiniGroup，那么进行覆盖
      virtual void addMiniGroups(const MiniGroupSeq& minigroups, const Ice::Current& = Ice::Current());

      //设置用户加入了那些MiniGroups
      virtual void setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& = Ice::Current());

      //将用户加入minigroup_ids中，minigroup_ids会追加到用户已经加入的MiniGroup的后面
      virtual void addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& = Ice::Current());

      //删除一个用户加入的minigroup_ids, 删除后剩余的MiniGroup保持原来的顺序
      virtual void removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& = Ice::Current());

      virtual void removeAllJoinedMiniGroups(int userId, const Ice::Current& = Ice::Current());
      virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

      bool isValid();
    private:
      MiniGroupSeq getMiniGroups(const MyUtil::LongSeq& miniGroupIds);
      IceUtil::Mutex _mutex;
      bool _valid;
  };

  class MiniGroupInfoFactoryI : public MyUtil::ServantFactory {
    public:
      Ice::ObjectPtr create(Ice::Int id);
  };

  class User2MiniGroupInfoFactoryI : public MyUtil::ServantFactory {
    public:
      Ice::ObjectPtr create(Ice::Int id);
  };

}
}

#endif

