#include "tripod-new/src/cpp/include/TripodClient.h"
#include "DbCxxPool/src/ConnectionPoolManager.h"
#include "search/tripodsearchcache/TripodCacheData.h"
#include <vector>
#include <map>
#include "TripodSearchCache.pb.h"

using namespace xce::search::tripodcachedata;
using namespace com::renren::tripod;
using namespace oce::tripodsearchcache;

void show(std::string userIdStr, TripodCacheDataPtr ptr) {
  std::cout << "ID : " << userIdStr << std::endl;
  std::cout << "name : " << ptr->name() << std::endl;
  std::cout << "gender : " << ptr->gender() << std::endl;
  std::cout << "signature : " << ptr->signature() << std::endl;
  std::cout << "status : " << ptr->status() << std::endl;
  std::cout << "safestatus : " << ptr->safestatus() << std::endl;
  std::cout << "stage : " << ptr->stage() << std::endl;
  std::cout << "state : " << ptr->state() << std::endl;
  std::cout << "birthyear : " << ptr->birthyear() << std::endl;
  std::cout << "birthmonth : " << ptr->birthmonth() << std::endl;
  std::cout << "birthday : " << ptr->birthday() << std::endl;
  std::cout << "astrology : " << ptr->astrology() << std::endl;
  std::cout << "headurl : " << ptr->headurl() << std::endl;
  std::cout << "tinyurl : " << ptr->tinyurl() << std::endl;
  std::cout << "statusconfig : " << ptr->statusconfig() << std::endl;
  std::cout << "browseconfig : " << ptr->browseconfig() << std::endl;
  std::cout << "basicconfig : " << ptr->basicconfig() << std::endl;
  std::cout << "profileprivacy : " << ptr->profileprivacy() << std::endl;
  std::cout << "homeprovince : " << ptr->homeprovince() << std::endl;
  std::cout << "homecity : " << ptr->homecity() << std::endl;
  std::cout << "interest : " << ptr->interest() << std::endl;
  std::cout << "music : " << ptr->music() << std::endl;
  std::cout << "movie : " << ptr->movie() << std::endl;
  std::cout << "game : " << ptr->game() << std::endl;
  std::cout << "comic : " << ptr->comic() << std::endl;
  std::cout << "sport : " << ptr->sport() << std::endl;
  std::cout << "book : " << ptr->book() << std::endl;
  std::cout << "society : " << ptr->society() << std::endl;
  std::cout << "Network" << std::endl;
  for (int i = 0; i<ptr->networks_size(); ++i) {
    std::cout << "\t" << ptr->networks(i).id()<< std::endl;
    std::cout << "\t" << ptr->networks(i).name() << std::endl;
    std::cout << "\t" <<  ptr->networks(i).stage()<< std::endl;
  }

  std::cout << "Elementary School" << std::endl;
  for (int i = 0; i<ptr->elementaryschools_size(); ++i) {
    std::cout << "\t" << ptr->elementaryschools(i).id() << std::endl;
    std::cout << "\t" <<  ptr->elementaryschools(i).year()<< std::endl;
    std::cout << "\t" << ptr->elementaryschools(i).name() << std::endl;
  }

  std::cout << "Junior School" << std::endl;
  for (int i = 0; i<ptr->juniorschools_size(); ++i) {
    std::cout << "\t" << ptr->juniorschools(i).id() << std::endl;
    std::cout << "\t" << ptr->juniorschools(i).year() << std::endl;
    std::cout << "\t" << ptr->juniorschools(i).name() << std::endl;
  }

  std::cout << "High School" << std::endl;
  for (int i = 0; i<ptr->highschools_size(); ++i) {
    std::cout << "\t" << ptr->highschools(i).id() << std::endl;
    std::cout << "\t" << ptr->highschools(i).name() << std::endl;
    std::cout << "\t" << ptr->highschools(i).year() << std::endl;
    std::cout << "\t" << ptr->highschools(i).class1() << std::endl;
    std::cout << "\t" << ptr->highschools(i).class2() << std::endl;
    std::cout << "\t" << ptr->highschools(i).class3() << std::endl;
  }

  std::cout << "Colege" << std::endl;
  for (int i = 0; i<ptr->collegeschools_size(); ++i) {
    std::cout << "\t" << ptr->collegeschools(i).id() << std::endl;
    std::cout << "\t" << ptr->collegeschools(i).name() << std::endl;
    std::cout << "\t" << ptr->collegeschools(i).year() << std::endl;
    std::cout << "\t" << ptr->collegeschools(i).department() << std::endl;
  }

  std::cout << "University" << std::endl;
  for (int i = 0; i<ptr->universityschools_size(); ++i) {
    std::cout << "\t" << ptr->universityschools(i).id() << std::endl;
    std::cout << "\t" << ptr->universityschools(i).year() << std::endl;
    std::cout << "\t" << ptr->universityschools(i).name() << std::endl;
    std::cout << "\t" << ptr->universityschools(i).department() << std::endl;
    std::cout << "\t" << ptr->universityschools(i).dorm() << std::endl;
  }

  std::cout << "Workplace" << std::endl;
  for (int i = 0; i<ptr->workplaces_size(); ++i) {
    std::cout << "\t" << ptr->workplaces(i).id() << std::endl;
    std::cout << "\t" << ptr->workplaces(i).name() << std::endl;
  }

  std::cout << "Region" << std::endl;
  for (int i = 0; i<ptr->regions_size(); ++i) {
    std::cout << "\t" << ptr->regions(i).id() << std::endl;
    std::cout << "\t" << ptr->regions(i).city() << std::endl;
    std::cout << "\t" <<  ptr->regions(i).province()<< std::endl;
  }

}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: TestTripodSearchGet logPath userId1 [userId2, ...]" << std::endl;
    return 1;
  }
  ostringstream oss;
  com::xiaonei::xce::ConnectionPoolManager::instance();
  oss << argv[1] << "/TripodSearchGet.log";
  std::vector<std::string> keys;
  std::vector<std::string> missedKeys;
  std::string logPath = oss.str();
  MyUtil::init_logger("Mce", logPath, "DEBUG");
  for (int i = 2; i < argc; i++) {
    std::string userId = boost::lexical_cast<std::string>(argv[i]);
    MCE_WARN("logPath:" << logPath << " userId:" << userId);
    keys.push_back(userId);
  }
  TripodClient* testClient = new TripodClient("UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/Tripod", "ne0", "SearchCache"); 
  DataMap result;
  try {
    result = testClient->get(keys, missedKeys);
  } catch (Ice::Exception& e) {
    MCE_ERROR("get Error :" << e.what());
  }
  MCE_WARN("result.size:" << result.size());
  for (std::map<std::string, Data>::iterator it = result.begin(); it != result.end(); it++) {
    MCE_WARN("result key:" << it->first);
    TripodCacheDataPtr tmp = new TripodCacheData;
    string dataStr((it->second).begin(), (it->second).end());
    tmp->ParseFromString(dataStr);
    show(it->first, tmp);
    std::cout << "---------------------------------------" <<  std::endl;
  }
  std::cout << "result.size:" << result.size() << std::endl;
  delete testClient;
  return 0;

}
