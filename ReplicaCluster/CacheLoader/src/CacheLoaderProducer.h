#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace cacheloader {

using namespace com::xiaonei::xce;

/*******************************************************************************************/

//这个是CacheLoader中生产线程使用的生产器...虚基类。
//用户可自行设计，也可使用本文件最后处预留的BatchUsersIdProducer
//如自行设计，注意:
//produce返回的是一批id，这批id将被置入消费队列，最终产生对应的Ice::ObjectPtr;
//maxId是一个引用传递，每次produce调用时可以修改并且建议修改为本批读取的id的最大值;
//CacheLoader每次调用produce后，对修改后的maxId处理为：如果maxId==beginId，则认为本次生产线程已经到达末尾
class Producer : virtual public Ice::Object {
public:
        virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId ) = 0;
};
typedef IceUtil::Handle<Producer> ProducerPtr;


const std::string DB_USERS = "user_passport_status";

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

class BatchUsersIdProducer : public virtual Producer {
        virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId ) {
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
