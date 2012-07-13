#include <IceUtil/IceUtil.h>
#include "FriendInputAdapter.h"
#include <UserBase.h>
#include <UserBaseAdapter.h>
#include "BuddyByIdReplicaAdapter.h"
#include <QueryRunner.h>
#include <ServiceI.h>
#include "LogWrapper.h"
#include <ConnectionHolder.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <mysql/mysql.h>
using namespace std;
using namespace MyUtil;
using namespace xce;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::services::FriendInput;
using namespace xce::adapter::userbase;
using namespace xce::buddy::adapter;

const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";
ofstream pinyin_dict("pinyin_dictionary.txt");

void Usage()  {
  cout << "Usage<FriendInputTestTools> -t<test_case> -i<id> -q<query>" << endl;
}


int main(int argc, char ** argv)
{
   string query;
   vector<int> results;
   int id;
   cout<<"Please input your ID:";
   cin>>id;
   cout<<endl;
   while(1)  {
      cin>>query;
      if(query=="exit")
         break;
      results = FriendInputAdapter::instance().getFriendInput(id,query,100);
      for(size_t i=0; i<results.size() ;i++)
         cerr<<(UserBaseAdapter::instance().getUserBaseCommonView(results.at(i)))->name()<<" ";
      cerr<<endl;
      vector<int> friendsIds=BuddyByIdCacheAdapter::instance().getFriendListAsc(id,-1);
      cerr<<"The number of friends is:"<<friendsIds.size()<<endl;
      cerr<<"The Friends'list of this person is:"<<endl;
      for(size_t i=0; i < friendsIds.size(); i++)
        cerr<<(UserBaseAdapter::instance().getUserBaseCommonView(friendsIds.at(i)))->name()<<" ";
      cerr<<endl;
   }
   return 0;
}
