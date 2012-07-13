#ifndef __GENERALPRELOADERPRODUCER_H__
#define __GENERALPRELOADERPRODUCER_H__
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace generalpreloader {

using namespace com::xiaonei::xce;

/*******************************************************************************************/

//这个是GeneralPreloader中生产线程使用的生产器...虚基类。
//用户可自行设计，也可使用本文件最后处预留的BatchUsersIdProducer
//如自行设计，注意:
//produce返回的是一批id，这批id将被置入消费队列，最终产生对应的Ice::ObjectPtr;
//maxId是一个引用传递，每次produce调用时可以修改并且建议修改为本批读取的id的最大值;
//GeneralPreloader每次调用produce后，对修改后的maxId处理为：如果maxId==beginId，则认为本次生产线程已经到达末尾
class Producer : virtual public Ice::Object {
public:
        virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId, int& tableMod) = 0;

        //因为有一些数据库的字段是long型的，int型无法满足要求，所以增加了这个函数。默认情况下会使用produce的函数，产生int的vector
        virtual std::vector<long> produceLongIds(long beginId, int load_batch_size, long& maxId, int& tableMod) {
          int intMaxId = (int)maxId;
          vector<int> intIds = produce((int)beginId, load_batch_size, intMaxId, tableMod);
          maxId = intMaxId;
          vector<long> longIds(intIds.begin(), intIds.end());
          return longIds;
        }
};
typedef IceUtil::Handle<Producer> ProducerPtr;


const std::string DB_USERS = "user_passport";
const std::string DB_FANS = "fans_pages_a";

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

class BatchFansIdHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BatchFansIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) :
      _results(results), _maxId(maxId) {
      }
    virtual bool handle(mysqlpp::Row& row) const {
      int id = (int) row["user_id"];
      _maxId = id > _maxId ? id : _maxId;
      _results.push_back( id );
      return true;
    }
  private:
    std::vector<int>& _results;
    Ice::Int& _maxId;
};


class BatchFansIdProducer : public virtual Producer {
        virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId , int& tableMod) {
                
			std::vector<int> results;
			Statement sql;
            sql << "SELECT distinct user_id FROM  fans_pages_" << tableMod <<" WHERE user_id > " << beginId
                << " ORDER BY user_id LIMIT " << load_batch_size;
			BatchFansIdHandlerI handler( results, maxId );
			QueryRunner( DB_FANS, CDbRServer ).query( sql, handler );
			return results;
        }
};

class BatchUsersIdProducer : public virtual Producer {
        virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod) {

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
