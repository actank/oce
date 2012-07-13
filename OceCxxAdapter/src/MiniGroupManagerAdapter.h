/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupManagerAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/26/2011 11:26:35 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __MINIGROUP_CACHE_ADAPTER_H__
#define __MINIGROUP_CACHE_ADAPTER_H__
#include <AdapterI.h>
#include <ServiceI.h>
#include <MiniGroup.h>
#include <IceUtil/IceUtil.h>

namespace minigroup {
  using namespace xce::mngp;
  class MiniGroupManagerAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::mngp::MiniGroupManagerPrx>,
      public MyUtil::Singleton<MiniGroupManagerAdapter> {
    public:
      MiniGroupManagerAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::mngp::MiniGroupManagerPrx> ("ControllerMiniGroupManager", 120,
          300, new MyUtil::OceChannel,true, "MNGPM") {}
      MiniGroupSeq getMiniGroups(int user_id);
      MiniGroupSeq getMiniGroupsWithIds(const MyUtil::LongSeq& miniGroupIds);
      
      void addMiniGroups(const MiniGroupSeq& minigroups);
      void setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void removeAllJoinedMiniGroups(int userId);
      void setValid(bool valid);
    private:
      MiniGroupManagerPrx getMiniGroupManagerPrx(int id = 0);
      vector<MiniGroupManagerPrx> getAllMiniGroupManagerPrx(int id = 0);
  };
}

#endif

