/*
  Date: 2011-10-27
  Author:   Junwu.xiong 
  Email:    junwu.xiong@renren-inc.com
*/
#ifndef __USER_PAGE_
#define __USER_PAGE_

#include "socialgraph/recommendpage/util/pageitem.h"

namespace xce {
namespace socialgraph {

// user's recommended page class 
class UserPages {
 public:

  const static unsigned int kRelationNum = 500;   //usual number of friends of each user
  const static unsigned int kRcdPageNum = 200;   //upbound of recommended pages number to each user 
  const static unsigned int kComDisplayNum = 10;  //upbound of the being displayed common friends number to each user of the page recommended

  UserPages();
  //UserPages(int user_id, PageVec page_items);
  ~UserPages() {}
  void setUserId(int user_id);
  int getUserId();

  void setPages(PageVec page_items);
  PageVec getPages();

 private:
  int user_id_;
  PageVec page_items_;
};

};
};

#endif 
