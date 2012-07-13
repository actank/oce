#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "UtilCxx/src/AtomicSTL.h"
#include "../include/Share.h"
#include "../include/AllUserProducer.h"
#include "../include/AllUserConsumer.h"
#include "BuddyFriendsCache.h"

using namespace std;
using namespace xce::fastpreloader;
using namespace xce::buddy;
/*
class FriendsAscHandler : virtual public com::xiaonei::xce::ResultHandler{
public:
        FriendsAscHandler(
                        MyUtil::ObjectResultPtr& objectResult,
                        const std::string& valueField ,
			const MyUtil::IntSeq& ids) :
                _objectResult(objectResult), _valueField(valueField) ,_ids(ids){
        }
protected:
        virtual bool handle(mysqlpp::Row& res) const{
                BuddyFriendsDataPtr::dynamicCast(_objectResult->data[_ids.front()])->friends.push_back((int)res[_valueField.c_str()]);
                return true;
        }
private:
        MyUtil::ObjectResultPtr _objectResult;
        std::string _valueField;
	MyUtil::IntSeq _ids;
};

class BuddyFriendsFactory : virtual public xce::fastpreloader::FastPreloaderFactory,
        virtual public MyUtil::Singleton<BuddyFriendsFactory> {
public:
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {

                MCE_INFO( "create begin: " << ids.front() << " end: " << ids.back());
                MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
                for(MyUtil::IntSeq::const_iterator it = ids.begin(); it!= ids.end();it++){
                        res->data[*it] = new BuddyFriendsData;
                }
                try {
                        Statement sql;
                        sql     << "SELECT guest"
                                << " FROM relation_" << (ids.front() % 100)
                                << " WHERE host >=" << ids.front()
                                << " and host <=" << ids.back();
                        ostringstream pattern;
                        pattern << "relation_" << (ids.front() % 100);
                        QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
                                        sql, FriendsAscHandler( res, "guest" ,ids));

                } catch (Ice::Exception& e) {
                        MCE_WARN("[create] Ice::Exception " << ids.front() << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[create] std::excetpion " << ids.front() << ": " << e.what());
                } catch (...) {
                        MCE_WARN("[create] exception" << ids.front() );
                }
                return res;
        }

private:
        BuddyFriendsFactory() {}
        friend class MyUtil::Singleton<BuddyFriendsFactory>;
};*/

class FriendsAscHandler : virtual public com::xiaonei::xce::ResultHandler{
public:
        FriendsAscHandler(
			MyUtil::ObjectResultPtr& objectResult,
			const std::string& keyField,
                        const std::string& valueField ) :
                _objectResult(objectResult), _keyField(keyField),_valueField(valueField) {
        }
protected:
        virtual bool handle(mysqlpp::Row& res) const{
		if(_objectResult->data.find((int)res[_keyField.c_str()]) != _objectResult->data.end()){
                        BuddyFriendsDataPtr::dynamicCast(_objectResult->data[(int)res[_keyField.c_str()]])->friends.push_back((int)res[_valueField.c_str()]);
                }
		return true;
        }
private:
	MyUtil::ObjectResultPtr _objectResult;
	std::string _keyField;
        std::string _valueField;
};


class BuddyFriendsFactory : virtual public xce::fastpreloader::FastPreloaderFactory,
        virtual public MyUtil::Singleton<BuddyFriendsFactory> {
public:
	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
		
		MCE_INFO( "create begin: " << ids.front() << " end: " << ids.back());
                MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
		for(MyUtil::IntSeq::const_iterator it = ids.begin(); it!= ids.end();it++){
			res->data[*it] = new BuddyFriendsData;
		}
		try {
			Statement sql;
			sql     << "SELECT host,guest"
				<< " FROM relation_" << (ids.front() % 100)
				<< " WHERE host >=" << ids.front() 
				<< " and host <=" << ids.back();
			ostringstream pattern;
			pattern << "relation_" << (ids.front() % 100);
			QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
					sql, FriendsAscHandler( res, "host","guest" ));
		
		} catch (Ice::Exception& e) {
			MCE_WARN("[create] Ice::Exception " << ids.front() << ": " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[create] std::excetpion " << ids.front() << ": " << e.what());
		} catch (...) {
			MCE_WARN("[create] exception" << ids.front() );
		}
                return res;
        }

private:
	BuddyFriendsFactory() {}
	friend class MyUtil::Singleton<BuddyFriendsFactory>;
};
/*
class OrderFriendsAscHandler : virtual public com::xiaonei::xce::ResultHandler{
public:
        OrderFriendsAscHandler(std::vector<int>& data,
                        const std::string& valueField ) :
                _data(data), _valueField(valueField) {
        }
protected:
        virtual bool handle(mysqlpp::Row& res) const{
                _data.push_back((int) res[_valueField.c_str()]);
                return true;
        }
private:
        std::vector<int>& _data;
        std::string _valueField;
};

class BuddyFriendsNewFactory : virtual public xce::fastpreloader::FastPreloaderFactory,
        virtual public MyUtil::Singleton<BuddyFriendsNewFactory> {
public:
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {

                MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
                for (size_t i = 0; i < ids.size(); ++i) {
                        try {
                                Ice::ObjectPtr obj = create( (Ice::Int)ids.at(i) );
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

        Ice::ObjectPtr create(Ice::Int id) {
                BuddyFriendsDataPtr obj = new BuddyFriendsData;
                std::vector<int> orderFriend;
                Statement sql;
                sql     << "SELECT guest"
                        << " FROM relation_" << (id % 100)
                        << " WHERE host = " << id;
                ostringstream pattern;
                pattern << "relation_" << (id % 100);
                QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
                                sql, OrderFriendsAscHandler( orderFriend, "guest" ));

                orderFriend.swap( obj->friends );
                return obj;
        }

private:
	BuddyFriendsNewFactory() {}
	friend class MyUtil::Singleton<BuddyFriendsNewFactory>;
};
*/
typedef IceUtil::Handle<AllUserConsumer<BuddyFriendsCacheManagerPrx> > AllUserConsumerPtr;
int main(){
	MCE_INIT("./log","INFO");
	ConnectionPoolManager::instance().initialize();
	
	std::map<int,BlockingQueueIntPtr > map;
	for(int i =0; i<100; i++){
		map[i] = new BlockingQueueInt();
		map[i]->setProps(2000000);
	}

	FacadeProducerPtr producer = new AllUserProducer(map,100,10000);
        producer->start(1024*1024).detach();	
	sleep(10);
	for(int i = 0; i<10; i++){
		vector<int> loadmod;
		for(int j=i*10;j<(i+1)*10;j++){
			loadmod.push_back(j);
		}
		string controller = "ControllerBuddyFriendsCache";
		FastPreloaderFactoryPtr bffp = &BuddyFriendsFactory::instance();
		//FastPreloaderFactoryPtr bffp = &BuddyFriendsNewFactory::instance();
		AllUserConsumerPtr consumer = new AllUserConsumer<BuddyFriendsCacheManagerPrx>(producer,map,loadmod,30, i,1,bffp, controller);
		MCE_INFO("consumer " << consumer);
		consumer->start(1024*1024).detach();
		MCE_INFO("consumer " << consumer <<"end");
	}
	sleep(1000000);
	return 0;
}
