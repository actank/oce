#ifndef __FASTPRELOADEALLUSERRPRODUCER_H__
#define __FASTPRELOADEALLUSERRPRODUCER_H__

#include <stdint.h>
#include "Producer.h"
#include "QueryRunner.h"
#include <mysql++/mysql++.h>

namespace xce{
namespace fastpreloader{

using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;

class UserIdResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        UserIdResultHandler(std::vector<int>& building,
                        const std::string& toField, int& max) :
                _building(building), _toField(toField) ,_max(max){
        }
        virtual ~UserIdResultHandler() {
        }
private:
        virtual bool handle(mysqlpp::Row& row) const {
                int to = (int) row[_toField.c_str()];
		_building.push_back(to);
		_max = to>_max ? to : _max;
                return true;
        }

        std::vector<int>& _building;
        std::string _toField;
	int& _max;
};

class AllUserProducer : virtual public FacadeProducer{
public:
	AllUserProducer(std::map<int,BlockingQueueIntPtr >& map, int cluster, int batchsize):
		FacadeProducer(map,cluster),_max(0),_batchsize(batchsize) {
        }

	virtual bool produce(std::vector<int>& data) {
		Statement sql;
                sql << "SELECT id FROM user_passport WHERE id > " << _max << " LIMIT " << _batchsize;
                UserIdResultHandler handler(data, "id", _max);
                QueryRunner("user_passport", CDbRServer).query(sql, handler);

                //std::cout<<"Produced " << data.size() << " userIds, start at " << _max <<std::endl;
                MCE_INFO("Produced " << data.size() << " userIds, start at " << _max );
		/*if(_max > 10000000){
			return false;
		}*/
		return !data.empty();
        }

private:
	int _max;
	int _batchsize;

};
typedef IceUtil::Handle<AllUserProducer> AllUserProducerPtr;

}
}
#endif
