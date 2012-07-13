/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupPreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/13/2011 09:36:42 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __MINIGROUP_PRELOADER_H__
#define __MINIGROUP_PRELOADER_H__

#include "MiniGroup.h"
#include "../../MiniGroupCommon/src/MiniGroupCommon.h"

#include "ServiceI.h"
#include "QueryRunner.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"

#include <string>
#include <vector>

namespace xce {
namespace mngp {
  using namespace std;

  class MiniGroupPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<MiniGroupPreloader> {
    public:
      MiniGroupPreloader() {}
      virtual ~MiniGroupPreloader() {}
  };

  class MiniGroupPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
    public:
      virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
  };
  
  class BatchMiniGroupIdProducer : public virtual xce::generalpreloader::Producer {
    public:
      virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId, int& tableMod);
  };

}
}

#endif
