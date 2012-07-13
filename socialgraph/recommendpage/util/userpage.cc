#include "socialgraph/recommendpage/util/userpage.h"

using namespace xce::socialgraph;
//usual number of friends of the user
const unsigned int UserPages::kRelationNum; 
//upbound of recommended pages number to each user
const unsigned int UserPages::kRcdPageNum;
//upbound of the being displayed common friends number to each user of the page recommended 
const unsigned int UserPages::kComDisplayNum;

UserPages::UserPages()
{

  int user_id = 0;

  PageVec page_items;

  setUserId(user_id);
  setPages(page_items);
}

void UserPages::setUserId(int user_id)
{
  user_id_ = user_id;
}

int UserPages::getUserId()
{
  return user_id_;
}

void UserPages::setPages(PageVec page_items)
{
  page_items_ = page_items;
}

PageVec UserPages::getPages()
{
  return page_items_;
}
