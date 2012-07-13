/*
  Date:   2011-10-24
  Author: Xiongjunwu
  Email:  Junwu.xiong
*/
#ifndef __RCD_PAGE_WORKER_H_
#define __RCD_PAGE_WORKER_H_

#include "socialgraph/recommendpage/util/pagecachehelper.h"
#include "socialgraph/recommendpage/util/pagedbhelper.h"
#include "UtilCxx/src/TaskManager.h"

namespace xce {
namespace socialgraph {

//different level types of thread pool size
const static int kLevel_Page_Infor = 1;
const static int kLevel_Page_Fans = 2;
const static int kLevel_User_Friend = 3;
const static int kLevel_Page_Common_Fans = 4;
const static int kLevel_Rcd_Page_Infor = 5;
const static int kLevel_Rcd_UserPage = 6;

//fetch page information task
class getPageInforTask : public MyUtil::Task {
 public:
  getPageInforTask(Page& page_item, int level = kLevel_Page_Infor) : Task(level), page_item_(page_item) {}
  ~getPageInforTask() {}
  virtual void handle();
 private:
  Page& page_item_;
};

//fetch page fans ids task
class getPageFansTask : public MyUtil::Task {
 public:
  getPageFansTask(Page& page_item, int level = kLevel_Page_Fans) : Task(level), page_item_(page_item) {}
  ~getPageFansTask() {}
  virtual void handle();
 private:
  Page& page_item_;
};

//fetch the user's friends task
class getUserFriendTask : public MyUtil::Task {
 public:
  getUserFriendTask(int user_id, IntVec& friend_ids, int level = kLevel_User_Friend) : 
                    Task(level), user_id_(user_id), friend_ids_(friend_ids)
  {}
  ~getUserFriendTask() {}
  virtual void handle();

 private:
  int user_id_;
  IntVec& friend_ids_; 
};

class setRcdUserPageTask : public MyUtil::Task {
 public:
  setRcdUserPageTask(const std::string& line, const PageMap& page_map, int level=kLevel_Rcd_UserPage) : 
                    Task(level), line_(line), page_map_(page_map)
  {}
  ~setRcdUserPageTask() {}
  virtual void handle();
 private:
  const std::string& line_;
  const PageMap& page_map_;
};

////build the user's recommended page information task
//class setRcdPageInforTask : public MyUtil::Task {
// public:
//  setRcdPageInforTask(UserPages userpages_item, int level = kLevel_Rcd_Page_Infor) : 
//                      Task(level), userpages_item_(userpages_item)
//  {}
//  ~setRcdPageInforTask() {}
//  virtual void handle();
// private:
//  int user_id_;
//  Page page_item_;
//  UserPages userpages_item_;
//};

};
};

#endif
