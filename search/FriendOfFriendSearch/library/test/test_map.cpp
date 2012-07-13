#include "../include/common.h"
#include "../include/userdata.h"
#include "../include/mapdb.h"
#include "../include/memcache.h"
#include "../include/lrucache.h"
#include "../include/querycache.h"
#include "../include/map.h"

using namespace com::renren::search2::friendoffriend;

static void ShowResult(const FriendOfFriendMap::Result &result)
{
  const UserData *items = result.GetItems();
  for (size_t i = 0; i < result.GetCount(); ++i) {
    LOGP("id: %u, type: %u, mutual %u", items[i].GetID(), items[i].GetType(), items[i].GetMutual());
  }
}

static const char *server_strings = "localhost:2012";

static void TestMap(const FriendOfFriendMap &map)
{
  /// try to find friends
  FriendOfFriendMap::Result result;
  if (!map.Find("二二", result)) {
    LOGP("Find 二二 failed.");
    exit(1);
  }
  LOGP("Found 二二");
  ShowResult(result);

  if (!map.Find("三三", result)) {
    LOGP("Find() failed.");
    exit(1);
  }
  LOGP("Found 三三");
  ShowResult(result);

  if (!map.Find("一一", result)) {
    LOGP("Find() failed.");
    exit(1);
  }
  LOGP("Found 一一");
  ShowResult(result);

  if (!map.Find("四四", result)) {
    LOGP("Find 四四 failed.");
  }
  LOGP("Found 四四?");
  ShowResult(result);

  /// show the size of the data
  const char *data_ptr = NULL;
  size_t data_size = 0;
  if (!map.GetData(data_ptr, data_size)) {
    LOGP("GetData() failed.");
    exit(1);
  }

  LOGP("size of map: %lu bytes.", data_size);
}

int main(int argc, char *argv[])
{
  FriendOfFriendMap map1;
  FriendOfFriendMap map2;
  FriendOfFriendMapDB db;
  FriendOfFriendMapCache cache(40);
  FriendOfFriendMemcache memcache(1800);

  if (!memcache.SetServers(server_strings)) {
    LOGP("SetServers() failed.");
    return 1;
  }

  memcache.Set("key1", "value1", 6);
  size_t value_len = 0;
  char *value = memcache.Get("key1", value_len);
  if (value == NULL) {
    LOGP("memcache.Get() failed");
    return 1;
  }
  LOGP("value: %s", value);
  free(value);

  if (!db.SetServers(server_strings)) {
    LOGP("SetServers() failed.");
    return 1;
  }

  LOGP("Size of FriendOfFriendMap is: %lu bytes", sizeof(map1));

  FriendInfoSeq infos;
  std::vector<std::string> names;

  /// fill data
  FriendInfo info1, info2, info3, info4;
  info1.id = 1234;
  info1.type = 2;
  info1.commonFriendsNum = 8;

  info2.id = 2345;
  info2.type = 2;
  info2.commonFriendsNum = 2;

  info3.id = 3456;
  info3.type = 2;
  info3.commonFriendsNum = 3;

  info4.id = 7123;
  info4.type = 1;
  info4.commonFriendsNum = 2;

  infos.push_back(info1);
  infos.push_back(info2);
  infos.push_back(info3);
  infos.push_back(info4);

  names.push_back("二二");
  names.push_back("三三");
  names.push_back("一一");
  names.push_back("二二");

  /// generate the data
  if (!map1.GenerateFromInfo(infos, names)) {
    LOGP("GenerateFromInfo() failed.");
    return 1;
  }

  TestMap(map1);

  /// store the data of the map in memcached
  if (!db.SetMap(282312, map1)) {
    LOGP("SetMap() failed.");
    return 1;
  }

  /// fetch the data to another map
  if (!db.GetMap(282312, map2)) {
    LOGP("GetMap() failed.");
    return 1;
  }

  TestMap(map2);

  // test override
  if (!db.SetMap(282312, map2)) {
    LOGP("SetMap() failed.");
    return 1;
  }

  if (!db.SetMap(282312, map2)) {
    LOGP("SetMap() failed.");
    return 1;
  }

  /// try to erase the key
  if (!db.EraseMap(282312)) {
    LOGP("EraseMap() failed.");
    return 1;
  }

  for (int i = 0; i < 65546; ++i) {
    if (!cache.SetMap(i + 1223, map1)) {
      LOGP("SetMap() failed.");
      return 1;
    }
    if (i > 65530) {
      LOGP("Set %d" , i);
    }
  }
  if (!cache.SetMap(761234, map1)) {
    LOGP("SetMap() failed.");
    return 1;
  }

  cache.Check();

  srand(time(NULL));

  timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < 1000000; ++i) {
    cache.GetMap(1223 + (rand() % 65545), map2);
  }
  gettimeofday(&end, NULL);

  LOGP("Cost %d ms.", (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

  cache.Check();

  LOGP("sleep for expiration...");
  sleep(40);
  LOGP("Now let's check how many entries are expired.");

  cache.Check();

  if (!cache.GetMap(1236, map2)) {
    LOGP("GetMap() failed.");
    return 1;
  }

  TestMap(map2);

  FriendOfFriendQueryCache querycache(3600);
  querycache.SetServers(server_strings);
  UserData arr_users[3];
  arr_users[0].id_ = 12345;
  arr_users[0].type_ = 1;
  arr_users[0].mutual_ = 10;

  arr_users[1].id_ = 721731;
  arr_users[1].type_ = 0;
  arr_users[1].mutual_ = 10;

  arr_users[2].id_ = 13821;
  arr_users[2].type_ = 4;
  arr_users[2].mutual_ = 90;

  if (!querycache.SetCache("test_12345", FriendOfFriendQueryCache::Cache(46123, arr_users, 3))) {
    LOGP("Set cache error !");
    return 1;
  }

  FriendOfFriendQueryCache::Cache queryresult;
  if (!querycache.GetCache("test_12345", queryresult)) {
    LOGP("Get cache error !");
    return 1;
  }

  const UserData* users;
  uint32_t result_hits;
  if (!queryresult.GetResult(users, result_hits)) {
    LOGP("Resolve cache error !");
    return 1;
  }

  LOGP("Total count: %u", queryresult.GetTotalHits());
  for (int i = 0; i < result_hits; ++i) {
    LOGP("userid: %u", users[i].GetID());
  }

  return 0;
}

