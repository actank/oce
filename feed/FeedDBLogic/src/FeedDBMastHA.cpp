#include "FeedDBMasterHA.h"
#include "FeedDBAdapter.h"
/*
class Cluster{
public:
	int id;
	EClusterState state;	
	bool master;
};
*/

void FeedDBNameNodeHA::GetBlocks(){

}

bool FeedDBNameNodeHA::AddSlave(int id){
	if(slaves_.find(id)==slaves_.end()){
		MCE_WARN("FeedDBNameNodeHA::AddSlave --> the slave is exist " << id);
		return false;
	}
	slaves_[id] = new NameNode(id, RESTORING); 
}

bool FeedDBNameNodeHA::changeMaster(bool deleteOldMaster){
	
}

bool PromotSlaveToMaster(int slave_id, bool deleteOldMaster){
	IceUtil::RWRecMutex::WLock lock(_rwMutex);
	if(slave_id < 0){
		slave_id = SelectReadySlave();
	}
	if(slave_id < 0){
		MCE_WARN("PromotSlaveToMaster --> can not find a ready slave");
		return false;
	}
	map<int, NameNode>::iterator it = slaves_.find(slave_id);
	if(it==slaves_.end()){
		MCE_WARN("PromotSlaveToMaster --> slave id is not exist " << slaveId);
		return false;
	}
	if(it->second.state != READY_FOR_SERVICE){
		MCE_WARN("PromotSlaveToMaster --> the slave is not ready " << slaveId);
		return false;
	}

	if(!deleteOldMaster){
		slaves_[master_.id] = master_;
		slaves_[master_.id].state = READY_FOR_SERVICE;
	}
	master_ = it->second;
	master_.state = IN_SERVICE;
	slaves_.erase(it);
}

void FeedDBNameNodeHA::AllocBlocks(){

}

void FeedDBNameNodeHA::SyncToSlaves(){

}
//Cluster		master_;
//	vector<Cluster> slaves_;

