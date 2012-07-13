#ifndef FEEDDBMASTER_HA_H
#define FEEDDBMASTER_HA_H

#include <map>
#include <IceUtil/RWRecMutex.h>
using namespace std;
enum EClusterState{IN_SERVICE, READY_FOR_SERVICE, RESTORING};

class NameNode{
public:
	int id;
	EClusterState state;	
	bool master;
};


class FeedDBNameNodeHA{
public:
	void GetBlocks();
	bool AddSlave(int id);
	bool changeMaster(bool deleteOldMaster);
	bool PromotSlaveToMaster(int slaveId = -1, bool deleteOldMaster = true);
protected:
	void AllocBlocks();
	void SyncToSlaves();
private:
	Cluster			master_;
	map<int, Cluster> 	slaves_;
	IceUtil::RWRecMutex	mutex_;
};

class SyncTask : public MyUtil::Task{
public:
	SyncTask(BlockInfoSeq& blocks){
		blocks_.swap(blocks);
	}
	void handle(){
		for(size_t i=0; i<slaves_.size(); i++){
		}
	}
private:
	BlockInfoSeq blocks_;
};
#endif
