#include "FeedMemcContentI.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "FeedMemcProxy/client/feed_memc_client.h"
//#include "FeedMemcProxy/client/talk_cache_client.h"
#include "FeedMemcProxy/client/user_profile_client.h"

#include <boost/foreach.hpp>
#include <FeedMemcLoaderAdapter.h>

#include "client/linux/handler/exception_handler.h"


using namespace xce::feed;
//using namespace xce::talk;
using namespace xce::ad;

using namespace com::xiaonei::xce;
using namespace MyUtil;

class SetTask1 : public Task{
public:
  SetTask1():Task(1){}
  void handle(){
    MCE_INFO("@@@@@@@@ Task1 begin");
     vector<Ice::Long> ids;
    ids.push_back(7157158703);
    FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(ids);
    FeedContentPtr content;
    if(!dict.empty()){
      content = dict.begin()->second;
      content->data->feed = 7157171548;
    }
    while(true){
      MCE_INFO("@@@@@@@@ set 7156174859");
      FeedMemcClient::instance().SetFeed(content);
    }
    MCE_INFO("@@@@@@@@ Task1 end");
  }
};
class SetTask2 : public Task{
public:
  SetTask2():Task(2){}
  void handle(){
    MCE_INFO("@@@@@@@@ Task2 begin");
    vector<Ice::Long> ids;
    ids.push_back(7157165386);
    FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(ids);
    FeedContentPtr content;
    if(!dict.empty()){
      content = dict.begin()->second;
      content->data->feed = 7157171548;
    }
    while(true){
      MCE_INFO("@@@@@@@@ set 7156176089");
      FeedMemcClient::instance().SetFeed(content);
    }
    MCE_INFO("@@@@@@@@ Task2 end");
  }
};
class GetTask : public Task{
public:
  GetTask(vector<Ice::Long> ids):ids_(ids){}
  void handle(){
    //MCE_INFO("@@@@@@@@ GetTask begin");
    //vector<Ice::Long> ids;
    //ids.push_back(7157171548);
    //while(true){
    while(true){
      FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(ids_);
    }
    //}
    //MCE_INFO("@@@@@@@@ GetTask end");
  }
private:
  vector<Ice::Long> ids_;
};

class ExecTimer : public Timer{
public:
  ExecTimer(vector<Ice::Long> ids):Timer(100), ids_(ids){}
  void handle(){
    TaskManager::instance().execute(new GetTask(ids_));
  }
private:
  vector<Ice::Long> ids_;
};

bool MyCallback(const char *dump_path,
 const char *minidump_id,
 void *context,
 bool succeeded) {
 printf("got crash %s %s\n", dump_path, minidump_id);
 return true;
 }

void MyUtil::initialize() {
static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
                                     , 0, true);  


  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedMemcContentI::instance(), service.createIdentity("M", ""));
  //TaskManager::instance().execute(new SetTask2());
  //TaskManager::instance().execute(new SetTask1());
  //TaskManager::instance().execute(new GetTask());
}

FeedContentDict FeedMemcContentI::GetFeedDict(const MyUtil::LongSeq & ids, const Ice::Current&) {
  //for(int i=0; i<ids.size(); i++){
  //  MCE_INFO("@@@@@ --> " << ids.at(i));
  //}
  //TaskManager::instance().scheduleRepeated(new ExecTimer(ids));
  //for(int i=0; i<2; i++){
  //  TaskManager::instance().execute(new GetTask(ids));
  //}
  FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(ids);
  MCE_INFO("FeedMemcContentI::GetFeedDict --> ids:" << ids.size() << " res:" << dict.size());
  return dict;
}

FeedContentPtr FeedMemcContentI::GetFeed(Ice::Long id, const Ice::Current&) {
  // for UserProfileClient test only
  vector<Ice::Long> feedids;
  feedids.push_back(8373921129);
  FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(feedids);
  FeedContentPtr content;
  content = dict[8373921129];
  content->data->xml = "<f v=\"1.0\" type=\"blogPublish\"><time>1300185966866</time><type>601</type><from><id>257876975</id><name>远非</name><tinyimg>0/0/men_tiny.gif</tinyimg></from><blog><id>716684593</id><title>ooxxxwwoo</title><digest>&amp;#23002;&amp;#23452;&amp;#19996;&amp;#20998;&amp;#20139;&amp;#26446;&amp;#22025;&amp;#27427;&amp;#30340;&amp;#26085;&amp;#24535;&amp;#36716;&amp;#65306;&amp;#20854;&amp;#23454;&amp;#22823;&amp;#23478;&amp;#37117;&amp;#34987;&amp;#26085;&amp;#26412;&amp;#22320;&amp;#38663;&amp;#21560;&amp;#24341;&amp;#20102;&amp;#30524;&amp;#29699; &amp;#30495;&amp;#27491;&amp;#26368;&amp;#21361;&amp;#38505;&amp;#30340;&amp;#26159;&amp;#21152;&amp;#24030; &amp;#65288;&amp;#20197;&amp;#19979;&amp;#20026;&amp;#36716;&amp;#36733;&amp;#65289;&amp;#26085;&amp;#26412;&amp;#36825;&amp;#27425;&amp;#29378;&amp;#26292;&amp;#30340;&amp;#22320;&amp;#38663;&amp;#30495;&amp;#30340;&amp;#21560;&amp;#24341;&amp;#20102;&amp;#22823;&amp;#23478;&amp;#30340;&amp;#30524;&amp;#29699;&amp;#65292;&amp;#25105;&amp;#22269;&amp;#30005;&amp;#35270;&amp;#21488;&amp;#26356;&amp;#26159;&amp;#36830;&amp;#31687;&amp;#32047;&amp;#29261;&amp;#30340;&amp;#25253;&amp;#36947;&amp;#20197;&amp;#26399;&amp;#36716;&amp;#31227;&amp;#22823;&amp;#23478;&amp;#23545;&amp;#22269;&amp;#20869;&amp;#29616;&amp;#29366;&amp;#30340;&amp;#19981;&amp;#28385;&amp;#12290;&amp;#20854;&amp;#23454;&amp;#65292;&amp;#22823;&amp;#23478;&amp;#38590;&amp;#36947;&amp;#24536;&amp;#35760;&amp;#20102;&amp;#32654;&amp;#22269;&amp;#21152;&amp;#24030;&amp;#37027;&amp;#22855;&amp;#24618;&amp;#27515;&amp;#20129;&amp;#30340;500&amp;#19975;&amp;#26465;&amp;#40060;&amp;#20102;&amp;#21527;&amp;#65311;&amp;#25105;&amp;#35748;&amp;#20026;&amp;#19981;&amp;#36229;&amp;#36807;&amp;#19968;&amp;#20010;&amp;#26376;&amp;#65292;&amp;#21152;&amp;#24030;&amp;#23558;&amp;#21457;&amp;#29983;&amp;#24046;&amp;#19981;&amp;#22810;9&amp;#32423;&amp;#30340;&amp;#22320;&amp;#38663;&amp;#12290;&amp;#20026;&amp;#37027;&amp;#20123;&amp;#21363;&amp;#23558;&amp;#32633;&amp;#38590;&amp;#30340;&amp;#20154;&amp;#31048;&amp;#31095;&amp;#21543;&amp;#65281;&amp;#65281;&amp;#65281;&amp;#65281; &amp;#27425;&amp;#22768;&amp;#27874;&amp;#12290;&amp;#12290;&amp;#12290;&amp;#36807;&amp;#24378;&amp;#30340;&amp;#27425;&amp;#22768;&amp;#27874;...</digest><url>http://blog.renren.com/GetEntry.do?id=716684593&amp;owner=257876975</url></blog></f>"; 
  content->reply->oldReply = "<f><reply><id>1692426042</id><type>0</type><time>2011-03-15 18:51</time><body>&quot;test1&quot;</body><im><body>test1</body></im><from><id>238489851</id><name>袁飞</name><tinyimg>http://hdn.xnimg.cn/photos/hdn221/20110220/1735/tiny_2kcu_195010p019116.jpg</tinyimg><icon></icon></from></reply></f>";
  content->reply->newReply = "<f><reply><id>1692426042</id><type>0</type><time>2011-03-15 18:51</time><body>&quot;test2&quot;</body><im><body>test2</body></im><from><id>238489851</id><name>袁飞</name><tinyimg>http://hdn.xnimg.cn/photos/hdn221/20110220/1735/tiny_2kcu_195010p019116.jpg</tinyimg><icon></icon></from></reply></f>";
  FeedMemcClient::instance().SetFeed(content);
  return NULL; 
  UserProfile profile;
  MCE_INFO("get profile : " << id );
  bool b = UserProfileClient::instance().Get(id, &profile);
  if (b) {
  MCE_INFO("profile fields : " 
      << " id " << profile.id()
      << " stage " << profile.stage()
      << " gender " << profile.gender()
      << " age " << profile.age()
      << " school " << profile.school()
      << " major " << profile.major()
      << " grade " << profile.grade()
      << " home_area " << profile.home_area() << "@" << profile.home_area().size()
      << " current_area " << profile.current_area() << "@" << profile.current_area().size()
      << " ip " << profile.ip()
      << " ip_area " << profile.ip_area() << "@" << profile.ip_area().size() );
  } else {
    MCE_INFO("profile not found. id=" << id); 
  }

  return FeedMemcClient::instance().GetFeed(id);
};

FeedDataSeq FeedMemcContentI::GetFeedDataByIds(int uid,const MyUtil::LongSeq & fids, const Ice::Current&) {
	if(fids.empty())
		return FeedDataSeq();

  FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(fids);
  CheckLoad(uid,fids,dict);

  FeedDataSeq seq;
  for (FeedContentDict::iterator it = dict.begin(); it != dict.end(); ++it) {
    if (it->second) {
      if (it->second->reply) {
        ostringstream xml;
        xml << it->second->data->xml;
        xml << it->second->reply->oldReply << it->second->reply->newReply
            << "<reply_count>" << it->second->reply->count << "</reply_count>";
        it->second->data->xml = xml.str();
      }
      seq.push_back(it->second->data);
    }
  }

  MCE_INFO("FeedMemcContentI::GetFeedDataByIds --> fids:" << fids.size() << " uid:" << uid << " res:" << seq.size());
  return seq;
}

void FeedMemcContentI::CheckLoad(long uid, const vector<Ice::Long> & fids, FeedContentDict & dict) {
  vector<Ice::Long> miss_fids;
  BOOST_FOREACH(long fid, fids){
  	if(!dict.count(fid)) miss_fids.push_back(fid);
  }
  if(miss_fids.empty()) return;

  FeedContentDict dict_loader;
  try {
  	dict_loader= FeedMemcLoaderAdapter::instance().GetFeedDict(miss_fids, 100);
  } catch (Ice::Exception& e) {
  	MCE_WARN("FeedItemManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_fids.size() << " err, " << e);
  	return;
  }
  MCE_INFO("FeedItemManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_fids.size() << " res:" << dict_loader.size());
  dict.insert(dict_loader.begin(),dict_loader.end());
}

void FeedMemcContentI::Test(const MyUtil::LongSeq & ids, const Ice::Current&) {
  return;
  MCE_INFO("FeedMemcContentI::Test --> ids:");
  if(ids.size() == 0){
    return;
  }
  //if(ids.size()%100 > 10 && ids.size()%100 < 20){
  if(ids.size()%10 > 0){
    FeedContentDict dict = FeedMemcClient::instance().GetFeedDict(ids);
    //MCE_INFO("FeedMemcContentI::Test --> ids:" << ids.size() << " res:" << dict.size());
  }
}

void FeedMemcContentI::Test2(const MyUtil::IntSeq & ids, const Ice::Current&) {
  if(ids.size() == 0){
    return;
  }
  return;
  if(ids.size()%10 < 10){
    //TalkCacheClient::instance().GetUserBySeqWithLoad(0, ids);
  }
}


