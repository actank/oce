/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantFactory.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月28日 10时04分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDY_BYAPPLICANT_FACTORY_H__
#define __BUDDY_BYAPPLICANT_FACTORY_H__

#include <algorithm>
#include <BuddyByApplicantCache.h>
#include <ObjectCacheI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"

namespace xce {
namespace buddy {
using namespace MyUtil;
using namespace xce::buddy;
using namespace com::xiaonei::xce;
using namespace xce::generalpreloader;

class BuddyByApplicantCacheResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	BuddyByApplicantCacheResultHandler(const BuddyByApplicantDataPtr& result) : _result(result) {}
	virtual ~BuddyByApplicantCacheResultHandler() {}
protected:
	virtual bool handle(mysqlpp::Row& row) const {
		AccepterInfo data;
		data.accepter = (int) row["accepter"];
		data.time = (int) row["unix_time"];
		_result->accepters.push_back(data);
		return true;
	}
private:
	BuddyByApplicantDataPtr _result;
};

class BuddyByApplicantFactory : virtual public xce::generalpreloader::IdListFactory,
	virtual public MyUtil::Singleton<BuddyByApplicantFactory> {
public:
	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
		MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
		int emptyCount = 0;
		ostringstream emptyIds;
		for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
			BuddyByApplicantDataPtr obj = create(*idIt);
			if (!obj->accepters.empty()) {
				res->data[*idIt] = obj;
			} else {
				emptyIds << *idIt << " ";
				emptyCount ++;
			}
		}
		MCE_DEBUG("BuddyByApplicantFactory::create, emptyCount : " << emptyCount << " ids.size() : " << ids.size());
		return res;
	}

	BuddyByApplicantDataPtr create(Ice::Int id) {
		MCE_DEBUG("BuddyByApplicantCacheFactory::create(" << id << ")");
		BuddyByApplicantDataPtr result = new BuddyByApplicantData();
		Statement sql;
		sql
			<< "SELECT accepter,UNIX_TIMESTAMP(time) AS unix_time"
			<< " FROM buddyapply_applicant_" << abs(id % 100) << " WHERE applicant = '" << id << "'"
			<< " ORDER BY time DESC"
			<< " LIMIT 100";
		BuddyByApplicantCacheResultHandler handler(result);
		ostringstream pattern;
		pattern << "buddyapply_applicant_" << id % 100;
		QueryRunner("buddyapplycache_applicant", CDbRServer,pattern.str()).query(sql, handler);

		return result;
	}
};

}
}
#endif
