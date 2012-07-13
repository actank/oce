//从数据库读取应用的粉丝数据，放到FansObject中。
//现在仅MutualFriendsLoaderI中用到，
//放到share目录中是为了以后preloader用到的时候可以共用。
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include "MutualFriendsCache.h"
#include "util/cpp/TimeCost.h"

namespace xce {
namespace mutualfriends {

using namespace xce::buddy;
using namespace MyUtil;
using namespace com::xiaonei::xce;

//-------------------------------OrderByIdAscHandler----------------------------
class OrderByIdAscHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	OrderByIdAscHandler(std::vector<int>& data, const std::string& valueField) :
		_data(data), _valueField(valueField) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		_data.push_back((int) res[_valueField.c_str()]);
		return true;
	}
private:
	std::vector<int>& _data;
	std::string _valueField;
};

//-----------------------------------FansFactory--------------------------------
class FansFactory : virtual public MyUtil::ServantFactory, virtual public MyUtil::Singleton<FansFactory> {
public:
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids) {
		MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
		for (size_t i = 0; i < ids.size(); ++i) {
			try {
				Ice::ObjectPtr obj = create(ids.at(i));
				if (obj) {
					res->data[ids.at(i)] = obj;
				}
			} catch (Ice::Exception& e) {
				MCE_WARN("[create] Ice::Exception " << ids.at(i) << ": " << e.what());
			} catch (std::exception& e) {
				MCE_WARN("[create] std::excetpion " << ids.at(i) << ": " << e.what());
			} catch (...) {
				MCE_WARN("[create] exception" << ids.at(i) );
			}
		}
		return res;
	}

	Ice::ObjectPtr create(Ice::Long id) {
		FansObjectPtr fansObj = new FansObject;
		std::vector<int> orderFans;

		//互斥块1::begin
		//发布使用：从数据库中读取用户数据
		Statement sql;
		sql << "SELECT userid" << " FROM member_" << (id % 100) << " WHERE object_id = " << id;
		std::ostringstream pattern;
		pattern << "member_" << (id % 100);
		com::xiaonei::xce::QueryRunner("relation_center", com::xiaonei::xce::CDbRServer, pattern.str()).query(sql, OrderByIdAscHandler(orderFans, "userid"));
		//互斥块1::end

//		//互斥块2::begin
//		//调试使用：没用户数据库环境的时候调试用
//		for(int i = 9; i < 100000; i += 9){
//			orderFans.push_back(i);
//		}
//		//互斥块2::end

		MCE_INFO("FansFactory::create id" << id << " size:" << orderFans.size());
		orderFans.swap(fansObj->fans);
		return fansObj;
	}
};

}//end of namespace mutualfriends
}//end of namespace xce
#endif
