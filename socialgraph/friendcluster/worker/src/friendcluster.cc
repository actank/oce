#include "friendcluster.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "socialgraph/socialgraphutil/mcclient.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "item.pb.h"

void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
	using namespace xce::socialgraph;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FriendClusterWorker::instance(), service.createIdentity("FFUCR", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendClusterWorker.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendClusterWorker.Interval", 5);

	static const std::string kControllerName = "ControllerFriendClusterWorker";
  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &FriendClusterWorker::instance(), mod, interval);

	ServerStateSubscriber::instance().setStatus(1);

  //注册zookeeper
  FriendClusterWorker::instance().Register("FFUCR", kControllerName);
}


namespace xce {
namespace socialgraph {

struct CPerson {
  CPerson(int i, int w) :
    id_(i), weight_(w) {
  }
  int id_;
  int weight_;
};

struct CFriendCluster {
  CFriendCluster(const std::string& n, int w) :
    name_(n), weight_(w) {
  }
  std::string name_;
  int weight_;
};



int CompareCPerson(const CPerson& cp1, const CPerson& cp2) {
  return cp1.weight_ > cp2.weight_;
}

int CompareCFriendCluster(const CFriendCluster& cf1, const CFriendCluster& cf2) {
  return cf1.weight_ > cf2.weight_;
}


void DeleteMessage::Run() {
	using namespace xce::socialgraph;
  std::vector<unsigned char> data;
	bool fetch = xce::MemcachedClient::GetInstance().GetByDistCache(mckey_, data);
 	if (fetch) {
		Items items;
    if (!items.ParseFromArray(data.data(), data.size())) {
      return;
    }
    bool flag = false;
		Items new_items;
    for (int i = 0; i < items.items_size(); ++i) {
			const Item& item = items.items(i);
      if (gid_ != item.id()) {
				Item* new_item = new_items.add_items();
        new_item->set_id(item.id());
        if (item.has_field()) {
          new_item->set_field(item.field());
        }
        for (int j = 0; j < item.fields_size(); ++j) {
          new_item->add_fields(item.fields(j));
        }
			}
      else {
        flag = true;
      }
		}
    if (!flag) {
      return;
    }
    int byte_size = new_items.ByteSize();
    unsigned char* chs = new unsigned char[byte_size];
    new_items.SerializeToArray(chs, byte_size);
    std::vector<unsigned char> v;
    v.reserve(byte_size);
    v.assign(chs, chs + byte_size);
		xce::MemcachedClient::GetInstance().SetByDistCache(mckey_, v);
    delete []chs;
		MCE_INFO("DeleteMessage::Run() DELETE host:" << hid_ << " guest:" << gid_ << " mckey:" << mckey_);
	}
}


//启动后首先加载cluster全部数据到本地内存
FriendClusterWorker::FriendClusterWorker() : threadPool_(8, 20) {
	Init();
}


void FriendClusterWorker::Init() {
//	InitClusterData();
}

void FriendClusterWorker::InitClusterData() {
	std::ifstream fin("/data/xce/friendcluster/cluster_data");
	if (!fin) {
		MCE_WARN("cluster_data file error, cannot open!");
		exit(-1);
	}
	std::hash_map<std::string, std::string> dataMap;
  std::string line;
  std::getline(fin, line);
  while (!line.empty()) {
    SaveOneCluster(line, dataMap);
    line.clear();
    std::getline(fin, line);
  }	

	if (!dataMap.empty()) {
		boost::lock_guard<boost::mutex> guard(mutex_);
		clusterDataMap_.swap(dataMap);
	}

}

void FriendClusterWorker::SaveOneCluster(std::string& line, std::hash_map<std::string, std::string>& dataMap) {
  std::vector<std::string> units;
  boost::split(units, line, boost::is_any_of("\t"));

  if (units.size() == 3) {
    std::string clusterName = units[0];
    int clusterWeight = 0;
    try {
      clusterWeight = (int) boost::lexical_cast<double>(units[1]);
    } catch (...) {
      MCE_WARN("cast error clusterName:" << clusterName);
    }
    std::string personsInfo = units[2];
    std::vector<std::string> personUnits;
    std::vector<CPerson> cPersons;
    boost::split(personUnits, personsInfo, boost::is_any_of(";"));	
    for (size_t i = 0; i < personUnits.size(); ++i) {
      std::string& personInfo = personUnits[i];
      if (!personInfo.empty()) {
        std::vector<std::string> infos;
        boost::split(infos, personInfo, boost::is_any_of(","));
        if (infos.size() == 2) {
          int id = 0;
          int weight = 0;
          try {
            id = boost::lexical_cast<int>(infos[0]);
            weight = boost::lexical_cast<int>(infos[1]);
            CPerson p(id, weight);
            cPersons.push_back(p);
          } catch (...) {
            continue;
          }
        }
      }
    }
    if (!cPersons.empty()) {
      std::sort(cPersons.begin(), cPersons.end(), CompareCPerson);
    }
    //开始持久化
    xce::socialgraph::FriendCluster friendCluster;
    friendCluster.set_weight(clusterWeight);
    std::vector<CPerson>::const_iterator iter = cPersons.begin();
    for (; iter != cPersons.end(); ++iter) {
      friendCluster.add_useridseq(iter->id_);
    }

    std::string data;
    bool flag = friendCluster.SerializeToString(&data);
    if (flag) {
      //保存
      dataMap.insert(std::make_pair(clusterName, data));
    }
  }



}


void FriendClusterWorker::Report(const Info& info, const Ice::Current& current) {
	switch (info.operation) 
	{
		case DELETE: 
			{
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:DELETE");
		  	int hostId = info.hostId;
        int guestId = 0;
        if (!info.friendIds.empty() && info.friendIds.size() == 1) {
			  	guestId = info.friendIds[0];
		  	}
		  	xce::Message* message = new DeleteMessage(hostId, guestId);
        threadPool_.Post(message);
			}
			break;
		case GET:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:GET");
			}
			break;
		case UNDO: 
			{
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:UNDO");

			}
			break;
		case CREATE:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:CREATE");

			}
			break;
		case APPLY: //接受好友申请
			{
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:APPLY");
		  	int hostId = info.hostId;
        int guestId = 0;
        if (!info.friendIds.empty() && info.friendIds.size() == 1) {
			  	guestId = info.friendIds[0];
		  	}
		  	xce::Message* message = new DeleteMessage(hostId, guestId);
        threadPool_.Post(message);
        xce::Message* message2 = new DeleteMessage(guestId, hostId);
        threadPool_.Post(message2);
			}
			break;
		case ACCEPT:
			{
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:ACCEPT");

			}
			break;
		case IGNORE:
     	{
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:IGNORE");

			}
			break;
		case DENY:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:DENY");

			}
			break;
		case UPDATE:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:UPDATE");

      }
			break;
		case ADD:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:ADD");

      }
			break;
    default:
      {
  			MCE_INFO("bizType:" << info.type << " hostId:" << info.hostId << " operation:default");

      }
      break;
	}

}
void FriendClusterWorker::PreLoad(int uid, bool signal, const Ice::Current& current) {

}

void FriendClusterWorker::Rebuild(int uid, const Ice::Current& current) {

}


}} // end xce::socialgraph 

