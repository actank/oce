#include "search/FriendOfFriendSearch/library/include/common.h"

#include <SearchModel.h>

#include "search/FriendOfFriendSearch/adapter/include/SearchManagerAdapter.h"

using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;
using namespace com::renren::search2::friendoffriend;

int main(int argc, char *argv[])
{
  if (argc != 2) {
    LOGP("Usage: %s <name>", argv[0]);
    return -1;
  }

  IndexCondition condition;
  condition.type = OpenSearch;
  condition.queryCondition["query"] = argv[1];

  IndexResultPtr result = SearchManagerAdapter::instance().SearchIndex(condition, 0, 20);
  LOGP("Totalcount: %u, timecost: %f", result->totalCount, result->timeCost);

  for (std::vector<IndexResultItem>::iterator item_it = result->contents.begin();
      item_it != result->contents.end(); ++item_it) {
    IndexResultItem &item = *item_it;
    const char *type = NULL;

    if (item.type == User) {
      type = "User";
    } else if (item.type == Page) {
      type = "Page";
    } else if (item.type == App) {
      type = "App";
    }

    LOGP("item: %s %u", type, item.id);
  }

  return 0;
}
