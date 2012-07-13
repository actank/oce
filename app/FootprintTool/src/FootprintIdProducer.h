#ifndef __FOOTPRINT_ID_PRODUCER_H__
#define __FOOTPRINT_ID_PRODUCER_H__
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace footprint {

const std::string DB_USERS = "user_passport";
const std::string DB_FANS = "fans_pages_a";
const int BATCH_SIZE= 3000;

class Producer : virtual public Ice::Object {
public:
        virtual std::vector<int> produce(const int beginId, int& maxId) = 0;
};
typedef IceUtil::Handle<Producer> ProducerPtr;


class FootprintIdHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	FootprintIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) :
		_results(results), _maxId(maxId) {}

	virtual bool handle(mysqlpp::Row& row) const {
		int id = (int) row["id"];
		_maxId = id > _maxId ? id : _maxId;
		_results.push_back( id );
		return true;
	}
private:
	std::vector<int>& _results;
	Ice::Int& _maxId;
};

class FootprintIdProducer : public virtual Producer {
public:
	virtual std::vector<int> produce(const int beginId, int& maxId) {
		std::vector<int> results;
		com::xiaonei::xce::Statement sql;
		sql << "SELECT id FROM user_passport WHERE id > " << beginId
			<< " ORDER BY id LIMIT " << BATCH_SIZE;
		FootprintIdHandlerI handler( results, maxId );
		com::xiaonei::xce::QueryRunner( DB_USERS, com::xiaonei::xce::CDbRServer ).query( sql, handler );
		return results;
	}
};

}
}

#endif
