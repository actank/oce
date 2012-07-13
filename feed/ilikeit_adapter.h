#ifndef XCE_FEED_ILIKEIT_ADAPTER_H__
#define XCE_FEED_ILIKEIT_ADAPTER_H__

#include "feed/base_feed/basictypes.h"
#include "feed/base_feed/common.h"
#include "feed/base_feed/logging.h"
#include "feed/arch_feed/adapter2.h"
//#include "ILikeIt.h"
#include "ILikeItCacheAdapter.h"
#include "IceLogger.h"

namespace xce {

//  SO@endpoint,MyUtil::SubjectObserverPrx->订阅”ClusterState“通知，opera(ClusterStateInfo{ServerStateMap ssm;})
//  M@endpoint,clustercontroller::ControllerManagerPrx, GetServerState() 返回 ClusterStateInfo结构
//inline xce::ilikeit::ILikeItCachePrx CreateILikeItAdapter(int uid) {
//  return Adapter::oce::Create<xce::ilikeit::ILikeItCachePrx>("ControllerILikeItCacheM", uid);
//}

inline ::xce::ilikeit::ILikeItCachePrx CachedILikeItAdapter(int uid) {
  return Adapter::oce::Cached<xce::ilikeit::ILikeItCachePrx>("ControllerILikeItCacheM", uid);
}

enum GlobalType {
  kGlobalBlog = 1,
  kGlobalPhoto = 2,
  kGlobalAlbum = 3,
  kGlobalShare = 4,
  kGlobalState = 5,
  kGlobalEdm = 6,
};

inline bool BuildGlobalId(int64 source, int type, xce::ilikeit::GlobalIdSeq* seq) {
  // TODO: 极端猥琐的代码，优化之
  int stype = type & 0xffff;
  xce::ilikeit::GlobalIdPtr gid(new xce::ilikeit::GlobalId);
  gid->id = source;
  if (stype == 601 || stype == 2901 || stype == 2907 || stype == 2910 || stype == 2012)
    gid->type = kGlobalBlog;
  else if (stype == 701 || stype == 703 || stype == 708 || stype == 2902 || stype == 2905 
      || stype == 2908 || stype == 2911 || stype == 2038)
    gid->type = kGlobalPhoto;
  else if (stype == 709 || stype == 2906  || stype == 2909  || stype == 2912 || stype == 2013)
    gid->type = kGlobalAlbum;
  else if (stype / 100 == 80 && stype != 8005 ||
          stype == 121 ||
          stype == 122 ||
          stype == 123)
    gid->type = kGlobalEdm;
  else if (stype == 5030 || stype / 100 == 1 ||stype == 2005 || stype == 2006 || stype == 2007 
          || stype == 2032 || stype == 2035 || stype == 2036)
    gid->type = kGlobalShare;
  else
    return false;
  if (seq)
    seq->push_back(gid);
  return true;
}

inline bool BuildZan(int uid, const xce::ilikeit::GlobalIdSeq& seq, xce::ilikeit::LikeInfoSeq* ret) {
  ASSERT(ret && uid && !seq.empty());
  if (!ret) {
    MCE_WARN("BuildZan arg ret NULL");
    return false;
  }

  try {
    *ret = xce::ilikeit::ILikeItCacheAdapter::instance().getLikeDetailBatchG(seq, uid, 0, 0);

    // 按 zan 散一份获取 prx
    //xce::ilikeit::ILikeItCachePrx prx = CachedILikeItAdapter(uid);
    //if (!prx) {
    //  MCE_WARN("BuildZan prx NULL");
    //  return false;
    //}

    //*ret = prx->getLikeDetailBatch(seq, uid, 0, 0);
  } catch (Ice::Exception& e) {
    // LOG_F(WARNING) << e;
    MCE_WARN("BuildZan error : " << e);
    return false;
  } catch(std::exception& e) {
	  MCE_WARN("BuildZan error : " << e.what());
		return false;
	}

  return true;
}

}
#endif // XCE_FEED_ILIKEIT_ADAPTER_H__
