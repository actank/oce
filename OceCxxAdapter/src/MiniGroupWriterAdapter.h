/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupWriterAdapter.h
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
#include <ext/hash_set>

namespace minigroup {
  using namespace xce::mngp;
  class MiniGroupWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::mngp::MiniGroupWriterPrx>,
      public MyUtil::Singleton<MiniGroupWriterAdapter>, IceUtil::Thread {
    public:
      MiniGroupWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::mngp::MiniGroupWriterPrx> ("ControllerMiniGroupWriter", 120, 300) {
        start();
      }
      void setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids);
      void reloadUsers(const MyUtil::IntSeq& userIds);
      void reloadMiniGroups(const MyUtil::LongSeq& miniGroupIds);
    protected:
      void run();
    private:
      MiniGroupWriterPrx getMiniGroupWriterPrx(int id);
      IceUtil::Monitor<IceUtil::Mutex> _mutex;
      hash_set<int> _userIdSet;
      hash_set<int> _miniGroupIdSet;
  };
}

#endif

