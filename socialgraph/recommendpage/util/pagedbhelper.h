/*
  Date:   2011-10-11
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: page information structure, user infomation classes and database handler classes in recommend page project  
*/

#ifndef _COMMON_DB_HELP_H_
#define _COMMON_DB_HELP_H_

#include "socialgraph/recommendpage/util/pageitem.h"
#include "socialgraph/recommendpage/util/userpage.h"
#include <iostream>
#include <vector>
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace socialgraph {

class PageDbHelper {
 public:
  static void getPageInforFromDb(Page& page_item);
  static void getPageFansFromDb(Page& page_item);
  static void getUserFriendFromDb(int user_id, IntVec& friend_ids); 
  static void getUserPageFromDb(int user_id, IntSet& page_ids);
  static void setRcdPageInforToDb(int user_id, PageVec page_items);
  static void getRcdPageInforFromDb(int user_id, PageVec& page_item_vector);
};

// Page fans' ids, query from page_fans 
class PageFansIdsHandler : public com::xiaonei::xce::ResultHandler {
 public:
  PageFansIdsHandler(IntVec& result) : result_(result) {}
  virtual ~PageFansIdsHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  IntVec& result_;
};

// friend ids handler, query from relation_friendrank
class FriendIdsHandler : public com::xiaonei::xce::ResultHandler {
 public:
  FriendIdsHandler(IntVec& result) : result_(result) {}
  virtual ~FriendIdsHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  IntVec& result_;
};

//user's already focused common page ids handler, query from fans_pages
class PageIdsHandler : public com::xiaonei::xce::ResultHandler {
 public:
  PageIdsHandler(IntSet& result) : result_(result) {}  
  virtual ~PageIdsHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  IntSet& result_;
};

// recommend page item check result handler, query from recommend_page 
class RcdPagesHandler : public com::xiaonei::xce::ResultHandler {
 public:
  RcdPagesHandler(PageVec& result) : result_(result) {}
  virtual ~RcdPagesHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;

 private:
  PageVec& result_;
};

};
};
#endif
