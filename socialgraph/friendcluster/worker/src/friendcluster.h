#ifndef _XCE_SOCIALGRAPH_FRIENDCLUSTER_H__
#define _XCE_SOCIALGRAPH_FRIENDCLUSTER_H__
 
#include "socialgraph/socialgraphutil/socialgraphworkerI.h"
#include "friendcluster.pb.h"
#include "base/hashmap.h"
#include "base/threadpool.h"

#include <boost/thread/mutex.hpp>
#include "ServiceI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>


namespace xce {
namespace socialgraph {

struct CPerson;
struct CFriendCluster;

int CompareCPerson(const CPerson&, const CPerson&);
int CompareCFriendCluster(const CFriendCluster&, const xce::socialgraph::CFriendCluster&);

const static std::string kBizName = "FFUCR";

class DeleteMessage : public xce::Message {
public:
  DeleteMessage(int hid, int gid) : hid_(hid), gid_(gid) {
		std::ostringstream oss;
    oss << kBizName << "_" << hid;
		mckey_ = oss.str();
	}
	virtual void Run();

private:
  int hid_;
  int gid_;
	std::string mckey_;
};

class FriendClusterWorker : virtual public SocialGraphWorkerI, 
                            virtual public MyUtil::Singleton<FriendClusterWorker> {
public:
	virtual ~FriendClusterWorker() {}
	virtual void Report(const Info&, const Ice::Current&);
  virtual void PreLoad(int, bool, const Ice::Current&);
  virtual void Rebuild(int, const Ice::Current&);

private:
  FriendClusterWorker();
	friend class MyUtil::Singleton<FriendClusterWorker>;

	void Init();
  void InitClusterData();
	void SaveOneCluster(std::string&, std::hash_map<std::string, std::string>&);
private:
	boost::mutex mutex_;
	std::hash_map<std::string, std::string> clusterDataMap_;
	xce::ThreadPool threadPool_;
};


}} // end xce::socialgraph

#endif
