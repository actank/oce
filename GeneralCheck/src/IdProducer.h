#ifndef __ID_PRODUCER_H__
#define __ID_PRODUCER_H__
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce 
{
namespace generalcheck
{

using namespace com::xiaonei::xce;

class IdProducer : virtual public Ice::Object {
public:
        virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ) = 0;
};
typedef IceUtil::Handle<IdProducer> IdProducerPtr;


const std::string DB_USERS = "user_passport";

class BatchUsersIdHandlerI: public com::xiaonei::xce::ResultHandler {

	public:
       
		BatchUsersIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) :
                _results(results), _maxId(maxId) {
        }
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

class BatchUsersIdProducer : public virtual IdProducer {
        virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ) {
                
			std::vector<int> results;
			Statement sql;
            sql << "SELECT id FROM user_passport WHERE id > " << beginId
                << " ORDER BY id LIMIT " << load_batch_size;
			BatchUsersIdHandlerI handler( results, maxId );
			QueryRunner( DB_USERS, CDbRServer ).query( sql, handler );
			return results;
        }
};

}
}

#endif
