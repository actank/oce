/*
 * =====================================================================================
 *
 *       Filename:  BuddyByNameShare.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年05月26日 18时07分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#ifndef __BUDDYBYNAMESHARE_H__
#define __BUDDYBYNAMESHARE_H__

#include <Singleton.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "BuddyFriendsAdapter.h"
#include <IceUtil/IceUtil.h>
#include "BuddyFriendsCache.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "util/cpp/TimeCost.h"

namespace xce {
  namespace buddyfriends {

    using namespace xce::buddy;
    using namespace xce::buddy::adapter;
    using namespace MyUtil;
    using namespace com::xiaonei::xce;

    const size_t BUDDY_LIMIT = 300;

    /*****************************************************************************/
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
    /*****************************************************************************/

    //-----------------------------------BuddyFriendsFactory-----------------------------------------------
    class BuddyFriendsFactory : virtual public xce::generalpreloader::IdListFactory,
    virtual public MyUtil::Singleton<BuddyFriendsFactory> {
      public:
        BuddyFriendsFactory() {}
        virtual ~BuddyFriendsFactory() {}
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

    };
    //-----------------------------------BuddyFriendsFactory end-----------------------------------------------

  }
}
#endif
