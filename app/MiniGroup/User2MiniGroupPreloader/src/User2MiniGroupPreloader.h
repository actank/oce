/*
 * =====================================================================================
 *
 *       Filename:  User2MiniGroupPreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 04:38:34 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __USER_2_MINIGROUP_PRELOADER_I__
#define __USER_2_MINIGROUP_PRELOADER_I__

#include "MiniGroup.h"
#include "../../MiniGroupCommon/src/MiniGroupCommon.h"

#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"

namespace xce {
namespace mngp {
  class User2MiniGroupPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<User2MiniGroupPreloader> {
    public:
      User2MiniGroupPreloader() {}
      virtual ~User2MiniGroupPreloader() {}
  };

  class User2MiniGroupInfoPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
    public:
      virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
  };

  class BatchUser2MiniGroupIdProducer : public virtual xce::generalpreloader::Producer {
    public:
      virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId, int& tableMod);
  };

}
}

#endif
