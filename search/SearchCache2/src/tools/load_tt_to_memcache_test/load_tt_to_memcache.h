/*
 * =====================================================================================
 *
 *       Filename:  load_tt_to_mamcache.h
 *
 *    Description:  Test progrm for loading data from tt
 *
 *        Version:  1.0
 *        Created:  2010年09月30日 16时11分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "../../Util/DatabaseCacheFactory.h"
#include "../../Util/TokyoTyrantCache.h"
#include "../../Util/BinString.h"
#include "util/cpp/TimeCost.h"
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "UtilCxx/src/LogWrapper.h"


const string USER_STATUS_FIELD = " id, status ";
const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
const int IDS_BATCH_SIZE = 300;
//const string DSN_ARRAY[4] = {"tokyotryant://10.3.20.153:1981","tokyotryant://10.3.20.154:1981","tokyotryant://10.3.20.155:1981","tokyotryant://10.3.20.156:1981"};
const string DSN = "tokyotryant://10.3.20.153:1981";
const int CLUSTER = 4;
const int MOD = 0;
class BasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& valid_ids_set);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    Ice::Int& _beginId;
    std::vector<int>& valid_ids_set_;
};

