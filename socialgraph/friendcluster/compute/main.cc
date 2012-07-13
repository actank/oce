#include <stdlib.h>
#include <list>
#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>

#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/SocialGraphLogicAdapter.h"

#include "base/logging.h"
#include "base/ptime.h"
#include "base/threadpool.h"
#include "base/hash.h"

#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphproto/friendcluster.pb.h"
#include "socialgraph/socialgraphproto/item.pb.h"
#include "socialgraph/socialgraphproto/pukfriendrecommend.pb.h"
#include "socialgraph/friendcluster/compute/computethread.h"


void TestXceAdapter() {
  using namespace xce::socialgraph;
  SocialGraphLogicAdapter& sgAdapter = SocialGraphLogicAdapter::instance();
  try {
  	Message msg;
  	Info info;
		info.hostId = 27640369;
    info.operation = DELETE;
    info.friendIds.push_back(1005);
  	info.type = "FFUCR";
  	msg.content.insert(std::make_pair(info.type, info));
  	sgAdapter.Report(info.hostId, msg);
	} catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
	} catch (...) {
    LOG(LERROR) << "...exception  sgAdatper.Report()";
	}

}

void TestGetFFUC(int userId) {
  using namespace xce::socialgraph;
  std::string mckey = xce::Comb("FFUC_", userId);
  std::vector<unsigned char> data;
  xce::MemcachedClient::GetInstance().GetByDistCache(mckey, data);
  LOG(INFO) << "mckey:" << mckey << "  data.size():" << data.size();
  Person person;
  person.ParseFromArray(data.data(), data.size());
  LOG(INFO) << "userId:" << userId;
  for (int i = 0; i < person.clusterindexseq_size(); ++i) {
		LOG(INFO) << "\tcluster_index:" << person.clusterindexseq(i);
	} 

}

void TestGetClusterUserId(int clusterId) {
  using namespace xce::socialgraph;
  std::string mckey = xce::Comb("FFCU_", clusterId);
  std::vector<unsigned char> data;
  xce::MemcachedClient::GetInstance().GetByDistCache(mckey, data);
  LOG(INFO) << "mckey:" << mckey << "  data.size():" << data.size();
  FriendCluster fc;
  fc.ParseFromArray(data.data(), data.size());
  for (int i = 0; i < fc.useridseq_size(); ++i) {
		LOG(INFO) << "\tuid:" << fc.useridseq(i);
	} 

}

void TestGetFFUCR(int id) {
  using namespace xce::socialgraph;
  std::ostringstream oss;
  oss << "FFUCR_" << id;
  const std::string mckey = oss.str();
	std::vector<unsigned char> data;
  xce::MemcachedClient::GetInstance().GetByDistCache(mckey, data);
  Items items;
  items.ParseFromArray(data.data(), data.size());
  for (int i = 0; i < items.items_size(); i++) {
    const Item& item = items.items(i);
		LOG(INFO) << "\t" << item.id();
    for (int j = 0; j < item.fields_size(); ++j) {
      LOG(INFO) << "\t\t" << item.fields(j);
    }
  }

  LOG(INFO) << "mckey:" << mckey 
            << "\tdata.size():" << data.size()
            << "\titem.items_size():" << items.items_size();
  
}


void TestSetFFUCR(int id) {
  using namespace xce::socialgraph;
 	std::vector<unsigned char> dd;
  xce::MemcachedClient::GetInstance().GetByDistCache(xce::Comb("FFW_", id), dd); 
  xce::socialgraph::PukFriendRecommend pfr;
  pfr.ParseFromArray(dd.data(), dd.size());
  Items items;
  for (int i = 0; i < pfr.commonfriendseq_size(); ++i) {
		const xce::socialgraph::PukFriendRecommend_CommonFriend& cf = pfr.commonfriendseq(i);
    Item* item = items.add_items();
    item->set_id(cf.userid());
    item->set_field(cf.sharecount());
    for (int j = 0; j < cf.shares_size(); j++) {
      item->add_fields(cf.shares(j));
    }
  }
  int size = items.ByteSize();
  unsigned char* chs = new unsigned char[size];
  items.SerializeToArray(chs, size);
  std::vector<unsigned char> v;
  v.reserve(size);
  v.assign(chs, chs + size);
  const std::string mckey = xce::Comb("FFUCR_", id);
  bool flag = xce::MemcachedClient::GetInstance().SetByDistCache(mckey, v);
  LOG(INFO) << "set flag:" << flag<< "   mckey:" << mckey << "   data.size():" << v.size();
  delete []chs;
}

void TestDeleteFFUCR(int host, int guest) {
  using namespace xce::socialgraph;
  std::ostringstream oss;
  oss << "FFUCR_" << host;
  const std::string mckey = oss.str();
	std::vector<unsigned char> data;
  xce::MemcachedClient::GetInstance().GetByDistCache(mckey, data);
  Items items;
  items.ParseFromArray(data.data(), data.size());
  Items new_items;
  for (int i = 0; i < items.items_size(); i++) {
    const Item& item = items.items(i);
    if (item.id() != guest) {
			Item* new_item = new_items.add_items();
      new_item->set_id(item.id());
      if (item.has_field()) {
        new_item->set_field(item.field());
      }
      for (int j = 0; j < item.fields_size(); ++j) {
        new_item->add_fields(item.fields(j));
      }
    }
  }
  int size = new_items.ByteSize();
  unsigned char chs[size];
  new_items.SerializeToArray(chs, size);
  std::vector<unsigned char> v;
  v.reserve(size);
  v.assign(chs, chs + size);
  bool flag = xce::MemcachedClient::GetInstance().SetByDistCache(mckey, v);
  LOG(INFO) << "set flag:" << flag<< "   mckey:" << mckey << "   data.size():" << v.size();

}


int main(int argc, char **argv) {
//  TestXceAdapter();
//  TestSetFFUCR(269936619);
//  sleep(10);
//  TestGetFFUCR(269936619);
//  TestGetFFUCR(27640369);
//  TestDeleteFFUCR(27640369, 1003);
//  LOG(INFO) << "after delete 1003 ===========";
//  TestGetFFUCR(27640369);
//  TestGetFFUCR(222677625);
  using namespace xce::socialgraph;
  PTIME(pt, "main", true, 0);
	LOG(INFO) << "argc:" << argc;
	if (argc < 5) {
		LOG(INFO) << "uasge: ./friendclustercompte cluster_data uid2cluster_data uid_data cluster_name_index";
		exit(-1);
	}
	xce::ThreadPool threadPool(8, 20);
	boost::shared_ptr<ClusterNameIndex> cluster_name_index_ptr(new ClusterNameIndex(argv[4]));
  std::ifstream in(argv[1]);
  if (!in) {
    LOG(LERROR) << "cannot open file:" << argv[1];
    exit(1);
  }
  
  std::string line;
  std::list<std::string> line_seq;
  do {
    line.clear();
    std::getline(in, line);
    line_seq.push_back(line);
  } while(!line.empty());

  LOG(INFO) << "line_seq.size():" << line_seq.size();
  int line_count = 0;
  for (std::list<std::string>::const_iterator iter = line_seq.begin(); iter != line_seq.end(); ++iter) {
    threadPool.Post(new ClusterComputeMessage(*iter, cluster_name_index_ptr));
    ++line_count;
    if (line_count % 1000000 == 0) {
      LOG(INFO) << "line_count:" << line_count;
    }
  }

	sleep(3);
  threadPool.Stop();
  threadPool.Join();

	return 0;

}

/* uid2cluster_data compute
  std::ifstream in(argv[2]);
  if (!in) {
    LOG(LERROR) << "cannot open file:" << argv[2];
    exit(1);
  }
  
  std::string line;
  std::list<std::string> line_seq;
  do {
    line.clear();
    std::getline(in, line);
    line_seq.push_back(line);
  } while(!line.empty());

  LOG(INFO) << "line_seq.size():" << line_seq.size();
  int line_count = 0;
  for (std::list<std::string>::const_iterator iter = line_seq.begin(); iter != line_seq.end(); ++iter) {
    threadPool.Post(new Uid2ClusterComputeMessage(*iter, cluster_name_index_ptr));
    ++line_count;
    if (line_count % 1000000 == 0) {
      LOG(INFO) << "line_count:" << line_count;
    }
  }


*/

/* cluster_data 计算 
  std::ifstream in(argv[1]);
  if (!in) {
    LOG(LERROR) << "cannot open file:" << argv[1];
    exit(1);
  }
  
  std::string line;
  std::list<std::string> line_seq;
  do {
    line.clear();
    std::getline(in, line);
    line_seq.push_back(line);
  } while(!line.empty());

  LOG(INFO) << "line_seq.size():" << line_seq.size();
  int line_count = 0;
  for (std::list<std::string>::const_iterator iter = line_seq.begin(); iter != line_seq.end(); ++iter) {
    threadPool.Post(new ClusterComputeMessage(*iter, cluster_name_index_ptr));
    ++line_count;
    if (line_count % 1000000 == 0) {
      LOG(INFO) << "line_count:" << line_count;
    }
  }
*/

// ===============================================================  不要删除
/*
	LOG(INFO) << "argc:" << argc;
	if (argc < 5) {
		LOG(INFO) << "uasge: ./friendclustercompte cluster_data uid2cluster_data uid_data cluster_name_index";
		exit(-1);
	}
	
  using namespace xce::socialgraph;
	using namespace xce::buddy::adapter;
	PTIME(pt, "Compute Uid2ClusterId", true, 0);
	boost::shared_ptr<ClusterNameIndex> clusterNameIndexPtr(new ClusterNameIndex(argv[4]));
//	ClusterComputeMessage clusterComputeMessage(cluster_data, clusterNameIndexPtr);
//  clusterComputeMessage.Run();
	xce::ThreadPool threadPool(8, 20);
	std::ifstream fin2(argv[2]);
	if (!fin2) {
		LOG(LERROR) << "error! cannot open uid2cluster_data:" << argv[2];
	}
	std::string line;
  std::getline(fin2, line);
	while (!line.empty()) {
		xce::Message* userClusterMessage = new Uid2ClusterComputeMessage(line, clusterNameIndexPtr);
   	threadPool.Post(userClusterMessage);
  	std::getline(fin2, line);
  }
	sleep(10);
  threadPool.Stop();
  threadPool.Join();
	return 0;
*/
