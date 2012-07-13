/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintPreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月17日 17时35分19秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __HOME_FOOTPRINT_PRELOADER_H__
#define __HOME_FOOTPRINT_PRELOADER_H__

#include "FootprintShare.h"
#include "QueryRunner.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include "ActiveTrackAdapter.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "app/Utils/src/DBUtils.h"
#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace xce {
namespace footprint {

using namespace xce::utils;
using namespace MyUtil;
using namespace com::xiaonei::xce; 
using namespace xce::footprint;
using namespace xce::log;

class HomeFootprintPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<HomeFootprintPreloader> {
public:
  HomeFootprintPreloader() {}
  virtual ~HomeFootprintPreloader() {}
};

class HomeFootprintPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
public:
  virtual MyUtil::ObjectResultPtr createWithLongIds(const MyUtil::LongSeq& ids);
  //下面这个函数不会被调用到的,但是必须实现，否则编译不过
  virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
    return 0;
  }
};

class BatchHomeFootprintIdProducer : public virtual xce::generalpreloader::Producer {
public:
  virtual std::vector<long> produceLongIds(long beginId, int load_batch_size, long& maxId, int& tableMod);
  //下面这个函数是不会被调到的,但是必须实现，否则编译不过
  virtual std::vector<int> produce(int, int, int&, int&) {
    vector<int> ids;
    return ids;
  }
};

MyUtil::ObjectResultPtr HomeFootprintPreloaderFactory::createWithLongIds(const MyUtil::LongSeq& ids) {
  MCE_INFO("[HomeFootprintPreloaderFactory::createWithLongIds] ids.size = " << ids.size());
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  if (ids.empty()) {
    return result;
  }
  vector<HomeFootprintSetPtr> finalPtrs;
  //此处一定要注意，ids并不一定属于同一个尾号
  map<long, vector<long> > seperatedIds = DBUtils::seperateIdsByMod<long>(ids, TABLE_HOME_FOOTPRINT_HEAD_MOD);
  for (map<long, vector<long> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    vector<HomeFootprintSetPtr> ptrs;
    HomeFootprintSetSeqHandler handler(ptrs, CACHE_SIZE);
    Statement sql;
    sql << "select " << COLUMN_VISITOR << ", " << COLUMN_OWNER << " from " << TABLE_HOME_FOOTPRINT_HEAD << "_"
      << itv->first << " where " << COLUMN_OWNER << " in " << DBUtils::getIdsStr<long>(itv->second);
    com::xiaonei::xce::QueryRunner(DESC_HOME_FOOTPRINT_HEAD, com::xiaonei::xce::CDbRServer).query(sql, handler);
    finalPtrs.insert(finalPtrs.end(), ptrs.begin(), ptrs.end());
  }
  MyUtil::ObjectResultPtr resultPtr = HomeFootprintSet::changeToHomeVisitHistoryMap(finalPtrs);
  MCE_INFO("[HomeFootprintPreloaderFactory::createWithLongIds] ids.size = " << ids.size() << " finished");
  return resultPtr;
}

std::vector<long> BatchHomeFootprintIdProducer::produceLongIds(long beginId, int load_batch_size, long& maxId, int& tableMod) {
  vector<long> ids;
  MCE_INFO("[BatchHomeFootprintIdProducer::produce] beginId = " << beginId << ", maxId = " << maxId << ", tableMod = " << tableMod);
  com::xiaonei::xce::Statement sql;
  sql << "SELECT distinct(" << COLUMN_OWNER << ") FROM " << TABLE_HOME_FOOTPRINT_HEAD << "_" << tableMod
    << " WHERE " << COLUMN_OWNER << " >= " << beginId << " order by " << COLUMN_OWNER << " limit " << load_batch_size;
  BatchIdResultHandler<long> handler(COLUMN_OWNER, ids);
  com::xiaonei::xce::QueryRunner(DESC_HOME_FOOTPRINT_HEAD, com::xiaonei::xce::CDbRServer).query(sql, handler);
  if (!ids.empty()) {
    maxId = *(ids.end() - 1);
  }
  return ids;
}

}
}
#endif
