#ifndef __CARDDBADAPTER_H__
#define __CARDDBADAPTER_H__

#include <vector>
#include <string>
#include <IceUtil/IceUtil.h>
#include <cstdio>
#include <time.h>
#include "UtilCxx/src/TaskManager.h"
#include "IceExt/src/ServiceI.h"
#include "CardDBAdapter.h"
#include "XceCache.h"
#include "CardCache.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "sstream"
#include "OceCxxAdapter/src/UserContactAdapter.h"
#include "util/cpp/TimeCost.h"


using namespace xce::cardcache;
using namespace xce::adapter::usercontact;
using namespace com::xiaonei::xce;

// 数据源
static std::string DATA_SOURCE_CONTACT     = "user_contact";
static std::string DATA_SOURCE_ADDRESSBOOK = "addressbook";
// 各个表的域
static const std::string contactField_  = " (id, msn, mobilephone, qq) ";
static const std::string importField_     = " (user_id, import_way, import_detail, msn, email, name, phone, qq, address, extra, request_count) ";
static const std::string importFieldNew_     = " (user_id, import_way, import_detail, msn, email, name, phone, qq, address, extra, request_count, subscribe_flag) ";
static const std::string relationField_   = " (user_id, friend_id, request_flag, record_time) ";
static const std::string cardField_     = " ( user_id, address, email, extra) ";
// 数据库中各个表的名称和前缀
static const std::string CONTACT_TABLE_NAME = "user_contact";
static const std::string CARD_TABLE_NAME    = "address_vcard";
static const std::string IMPORT_TABLE_NAME = "address_book_import";
static const std::string relationPrefix_    = "personal_address_book_";

struct dbInfo {
  int userId;
  int friendId;
  int state;
  time_t recordTime;
};

class CardDBAdapter {
  public:
    // 散列的份数
    static const int mod = 100;

  public:
    static std::string GetTableName(int id, const std::string prefix) {
      int index = id % mod;
      // std::stringstream ss;
      // ss << index;
      // return prefix + ss.str();
      std::string result;
      char buffer[10];
      sprintf(buffer, "%d", index);
      result = buffer;
      return prefix + result;
    }

    // 构造函数
    CardDBAdapter() {}

    // 向数据库中插入名片信息和联系方式信息,如果这些信息已经存在，则更新；否则，插入
    static bool AddInfo(const CardInfoSeq& infos) {
      if (infos.empty()) {
        return false;
      }

      {
        Statement sql;
        // std::ostringstream sql;
        sql << "INSERT INTO " << CARD_TABLE_NAME << cardField_ << " VALUES";
        for (CardInfoSeq::const_iterator it = infos.begin(); it != infos.end(); ++it) {
          if ((*it)->userId <= 0) {
            continue;
          }
          if(it!=infos.begin()){
            sql<< ",";
          }
          sql << "(" <<
            (*it)->userId << ",'"<< 
            (*it)->address << "','"<< 
            (*it)->email << "','" << 
            (*it)->extra << "' )";
        }
        sql << " ON DUPLICATE KEY UPDATE " <<
          "address = VALUES(address)," <<
          "email = VALUES(email), " <<
          "extra = VALUES(extra) " ;
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
        // std::cout << sql.str() << std::endl;
      }

/*      {
        Statement sql;
        sql << "INSERT INTO " << CONTACT_TABLE_NAME << contactField_ << " VALUES";
        for (CardInfoSeq::const_iterator it = infos.begin(); it != infos.end(); ++it) {
          if ((*it)->userId <= 0) {
            continue;
          }
          if(it!=infos.begin())
            sql<< ",";
          sql << "(" <<
            (*it)->userId << ",'" <<
            (*it)->msn << "','" <<
            (*it)->mobile << "','" <<
            (*it)->qq << "')"; 
        }
        sql << " ON DUPLICATE KEY UPDATE " << 
          "msn = VALUES(msn), " <<
          "mobilephone = VALUES(mobilephone), " <<
          "qq = VALUES(qq)";

        QueryRunner("user_contact", CDbWServer).execute(sql);
      }

      for(CardInfoSeq::const_iterator it=infos.begin();it!=infos.end();++it){
        UserContactAdapter::instance().reload((*it)->userId);
      }
      */
      return true;
    }

    // 向数据库中添加导入信息, 如果该信息已经存在，则更新，否则，插入该信息
    static bool AddImport(ImportInfoSeq infos) {
      ImportInfoSeq::iterator it;

      if (infos.empty()) {
        return false;
      }
      Statement sql;
      sql << "INSERT INTO " << IMPORT_TABLE_NAME << importFieldNew_ << "VALUES";
      TimeCost tc = TimeCost::create("[AddImport:begin]",1);
      for (it = infos.begin(); it != infos.end(); ++it) {
        if ((*it)->userId <= 0) {
          continue;
        }
        if(it!=infos.begin()){
          sql<< ",";
        }
        sql << "(" <<
          (*it)->userId << "," <<
          (int)(*it)->importWay << ",'" <<
          (*it)->importDetail << "','" <<
          (*it)->msn << "','" <<
          (*it)->email << "','" <<
          (*it)->name << "','" <<
          (*it)->mobile << "','" <<
          (*it)->qq << "','" <<
          (*it)->address << "','" <<
          (*it)->extra << "'," <<
          (*it)->requestCount << ","<<
          "0)";
      }
      tc.step("[AddImport:done]");
      sql << " ON DUPLICATE KEY UPDATE " <<
        "msn = VALUES(msn)," <<
        "email = VALUES(email)," <<
        "name = VALUES(name)," <<
        "phone = VALUES(phone), " <<
        "qq = VALUES(qq), " <<
        "address = VALUES(address), " <<
        "extra = VALUES(extra), " <<
        "request_count = VALUES(request_count)," <<
        "subscribe_flag = VALUES(subscribe_flag); ";
      QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      tc.step("[AddImport:ExecuteDone]");
      return true;
    }
              

    static bool AddImportNew(ImportInfoNewSeq infos) {
      ImportInfoNewSeq::iterator it;

      if (infos.empty()) {
        return false;
      }
      Statement sql;
      sql << "INSERT INTO " << IMPORT_TABLE_NAME << importFieldNew_ << "VALUES";
      TimeCost tc = TimeCost::create("[AddImportNew:begin]",1);
      for (it = infos.begin(); it != infos.end(); ++it) {
        if ((*it)->userId <= 0) {
          continue;
        }
        if(it!=infos.begin()){
          sql<< ",";
        }
        sql << "(" <<
          (*it)->userId << "," <<
          (int)(*it)->importWay << ",'" <<
          (*it)->importDetail << "','" <<
          (*it)->msn << "','" <<
          (*it)->email << "','" <<
          (*it)->name << "','" <<
          (*it)->mobile << "','" <<
          (*it)->qq << "','" <<
          (*it)->address << "','" <<
          (*it)->extra << "'," <<
          (*it)->requestCount << ","<<
          (int)(*it)->subscribeFlag << ")";
      }
      tc.step("[AddImportNew:done]");
      sql << " ON DUPLICATE KEY UPDATE " <<
        "user_id = VALUES(user_id)," <<
        "import_way = VALUES(import_way)," <<
        "import_detail = VALUES(import_detail)," <<
        "msn = VALUES(msn)," <<
        "email = VALUES(email)," <<
        "name = VALUES(name)," <<
        "phone = VALUES(phone), " <<
        "qq = VALUES(qq), " <<
        "address = VALUES(address), " <<
        "extra = VALUES(extra), " <<
        "request_count = VALUES(request_count)," <<
        "subscribe_flag = VALUES(subscribe_flag); ";
      QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      tc.step("[AddImportNew:ExecuteDone]");
      return true;
    }

    static bool SetSubscribeFlag(ImportInfoNewSeq values) {
      ImportInfoNewSeq::iterator it;

      if (values.empty()) {
        return false;
      }
      TimeCost tc = TimeCost::create("[AddImportNew:begin]",1);
      for (it = values.begin(); it != values.end(); ++it) {
        Statement sql;
        sql << "UPDATE " << IMPORT_TABLE_NAME << " SET subscribe_flag=";
        sql <<  (int)(*it)->subscribeFlag << " WHERE id=" <<
          (*it)->id << ";";
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      }
      return true;
    }

    //向数据库中添加关系信息，如果该关系已经存在，则更新；否则，插入该关系
    static bool AddRelation(int userId, int friendId, int state1, int state2, int recordTime) {
      if (userId <= 0 || friendId <= 0) {
        return false;
      }

      {
        Statement sql;
        std::string table_name_1 = GetTableName(userId, relationPrefix_);

        sql << "INSERT INTO " << table_name_1 << relationField_
          << "VALUES(" << userId << "," << friendId << "," << state1 << ",from_unixtime(" << recordTime << "))"
          << " ON DUPLICATE KEY UPDATE request_flag = VALUES(request_flag)," <<
          "record_time = VALUES(record_time);";
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      }
      {
        Statement sql;
        std::string table_name_2 = GetTableName(friendId, relationPrefix_);
        sql << "INSERT INTO " << table_name_2 << relationField_
          << "VALUES(" << friendId << "," << userId << "," << state2 << ",from_unixtime(" << recordTime << "))"
          << " ON DUPLICATE KEY UPDATE request_flag = VALUES(request_flag)," <<
          "record_time = VALUES(record_time);";

        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      }
      return true;
    }

    static bool AddRelations(std::vector<dbInfo>& infos, int suffix) {
      if (infos.empty()) {
        return false;
      }
      MCE_INFO("[AddRelations] info.size:" << infos.size() << " suffix:" << suffix);
      std::vector<dbInfo>::iterator it;
      Statement sql,sqld;
      stringstream table_name;
      table_name << relationPrefix_ << suffix;
      std::string table_name_1 = table_name.str();
      sql << "INSERT INTO " << table_name_1 << relationField_ << "VALUES";
      sqld << "DELETE FROM " << table_name_1 << " WHERE ";

      int userId, friendId, state, recordTime, addCount = 0, removeCount = 0;
      bool removeFlag;
      for (it = infos.begin(); it != infos.end(); ++it) {
        userId = (*it).userId;
        friendId = (*it).friendId;
        state = (*it).state;
        recordTime = (*it).recordTime;

        removeFlag = false;
        if (state == REMOVE) {
          removeFlag = true;
        }
        if (removeFlag) {
          if (removeCount != 0) {
            sqld << " or ";
          }
          sqld << " ( user_id = " << userId << " and friend_id = " << friendId << " ) ";
          removeCount++;
        } else {
          if(addCount != 0){
            sql<< ",";
          }
          sql << "(" << userId << "," << friendId << "," << state << ",from_unixtime(" << recordTime << "))";
          addCount++;
        }
      }
      if (addCount > 0) {//至少有一条数据需要写入时才执行
        sql << " ON DUPLICATE KEY UPDATE request_flag = VALUES(request_flag)," <<
          "record_time = VALUES(record_time);";
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      }
      if (removeCount > 0) {//至少有一条数据需要删除时才执行
        sqld << ";";
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sqld);
      }
      return true;
    }

    // 删除导入的信息
    static void RemoveImport(ImportInfoSeq infos) {
      ImportInfoSeq::iterator it;
      for (it = infos.begin(); it != infos.end(); ++it) {
        Statement sql;
        sql << "DELETE FROM " << IMPORT_TABLE_NAME << " WHERE ";
        sql << "user_id =" << (*it)->userId << " and import_way = '"
          << (int)(*it)->importWay << "' and import_detail = '" << (*it)->importDetail << "';";
        QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
      }

      // QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWRServer).query(sql, NULL);
    }
    

    //　删除一对关系
    static void RemoveRelation(int userId, int friendId) {
      if (userId <= 0 || friendId <= 0) {
        return;
      }
      std::string table_name_1 = GetTableName(userId, relationPrefix_);
      std::string table_name_2 = GetTableName(friendId, relationPrefix_);
	  {
		  Statement sql;
		  sql << "DELETE FROM " << table_name_1 << " WHERE user_id = " << userId << " AND " << "friend_id = " << friendId << ";";
	  	  QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
	  }
	  {
		  Statement sql;
          sql<< "DELETE FROM " << table_name_2 << " WHERE user_id = " << friendId << " AND " << "friend_id = " << userId << ";";
     	   QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
	  }
    }

    // 删除名片信息
    static void RemoveInfo(std::vector<int> ids) {
      if (ids.empty()) {
        return;
      }

      std::vector<int>::iterator it;
      Statement sql;
      sql << "DELETE FROM " << CARD_TABLE_NAME << " WHERE user_id IN(";
      for (it = ids.begin(); it != ids.end(); ) {
        if ((*it) <= 0) {
          continue;
        }
        sql << *it;
        if ((++it) != ids.end()) {
          sql << ", ";
        } else {
          sql << ");";
          break;
        }
      }

      sql << "DELETE FROM " << CONTACT_TABLE_NAME << " WHERE id IN(";
      for (it = ids.begin(); it != ids.end(); ) {
        sql << *it;
        if ((++it) != ids.end()) {
          sql << ", ";
        } else {
          sql << ");";
          break;
        }
      }

      QueryRunner(DATA_SOURCE_ADDRESSBOOK, CDbWServer).execute(sql);
    }
    
};

#endif
