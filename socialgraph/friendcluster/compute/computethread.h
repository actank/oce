#ifndef _XCE_SOCIALGRAPH_COMPUTE_THREAD_H__
#define _XCE_SOCIALGRAPH_COMPUTE_THREAD_H__

#include "base/messagequeue.h"
#include "base/hashmap.h"
#include "base/cache.h"

namespace xce {
namespace socialgraph {

struct CPerson;
struct CFriendCluster;

int CompareCPerson(const CPerson&, const CPerson&);
int CompareCFriendCluster(const CFriendCluster&, const xce::socialgraph::CFriendCluster&);


//------------------------------------------------------------------------------------------
class ClusterNameIndex {
public:
	ClusterNameIndex(std::string filePath = "./cluster_name_index");
  virtual ~ClusterNameIndex() {}
  bool GetClusterNameIndex(const std::string&, int&);
private:
  std::hash_map<std::string, int> clusterNameIndexMap_;
  std::string nameIndexFilePath_;
};

//------------------------------------------------------------------------------------------
class ClusterComputeMessage : public xce::Message {
public:
  ClusterComputeMessage(const std::string, boost::shared_ptr<ClusterNameIndex>& ptr);
	virtual void Run();
	virtual ~ClusterComputeMessage();
private:
	void SaveOneCluster(const std::string& line);
private:
	std::string line_;
  boost::shared_ptr<ClusterNameIndex> clusterNameIndexPtr_;
};



//------------------------------------------------------------------------------------------
class Uid2ClusterComputeMessage : public xce::Message {
public:
  Uid2ClusterComputeMessage(const std::string& , boost::shared_ptr<ClusterNameIndex>&);
	virtual void Run();
	virtual ~Uid2ClusterComputeMessage();
  
private:
	void Init();
  void SaveOneUserCluster(const std::string& line);

private:
  std::string line_;
  boost::shared_ptr<ClusterNameIndex> clusterNameIndexPtr_;
	
};


//------------------------------------------------------------------------------------------
// 首页推荐使用 业务类型为:FFUCR
class FriendClusterRecommendMessage : public xce::Message {
public:
  FriendClusterRecommendMessage(int id) : hid_(id) {

  }
  virtual ~FriendClusterRecommendMessage() {}

  virtual void Run();
  bool GetFriendClusterUser(int userId, std::vector<int>&);
private:
  int hid_;
private:
};

}}



#endif
