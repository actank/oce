

void LogicManager::put(const FeedItem& feed, const Int2IntMap& userWeight){
	FeedDBMasterHa::instance().getBlocks();
	static const unsigned int MAX_MSG_SIZE = 5000;
	map<int, vector<BlockInfoSeq> > blkInfoMap;

	vector<vector<int> > user_splits;
	Int2IntMap::const_iterator it = usersWeight.begin();
	for(; it!=usersWeight.end(); ++it){
		if(it->first == 0){
			continue;
		}
		if(user_splits.empty() || (user_splits.back().size()>MAX_MSG_SIZE)){
			user_splits.push_back(vector<int>());
		}
		user_splits.back().push(user->first);
	}

	for(size_t i=0; i<user_splits.size(); i++){
		BlockInfoSeq blkInfos = FeedDBMasterHA::instance().getBlocks(user_splits.at(i), true);
		map<int, vector<BlockInfoSeq> > blkInfoMap;
		for(size_t j=0; j<blkInfos.size(); j++){
			BlockInfoPtr blk = blkInfos.at(j);
			blk->second = userWeight[blk->userId];
			blkInfoMap[blk->serverId].push_back(blk);
		}
		map<int, vector<BlockInfoSeq> >::iterator tmpIt = blkInfoMap.begin();
		for(; tmpIt!=blkInfoMap.end(); ++tmpIt){
			
		}
		FeedDBDataHA::instance().putBatch(feed, );
	}


	map<int, vector<pair<bool, BlockInfoSeq> > >::iterator tmpIt = blkInfoMap.begin();	
	for(; tmpIt!=blkInfoMap.end(); ++tmpIt){
		//MCE_INFO("DataManager::putBatch --> " << it->second.size());
		for(size_t i=0; i<tmpIt->second.size(); i++){
			//MCE_INFO("DataNodeManager::putBatch --> serverid:" << tmpIt->first << " split:" << tmpIt->second.size() << " each size:" 
				//<< tmpIt->second.at(i).second.size() << " retry:" << tmpIt->second.at(i).first);	
			//最多重试10次
			//for(int j=0; j<10; j++){
				try{
					FeedDBDataAdapter::instance().putBatch(feed, tmpIt->second.at(i).second);
				}catch(Ice::Exception& e){
					MCE_WARN("DataNodeManager::putBatch --> invoke FeedDBData" << tmpIt->first << " err, " << e);
					RetryTimer::instance().push(feed, tmpIt->second.at(i).second);
					//string tmp = e.what();
					//if(tmp.find("TimeoutException") != string::npos){
					//	break;
					//}
					//if(!tmpIt->second.at(i).first){
					//	break;
					//}
				}
			//}
		}
	}
	MCE_INFO("batch put --> " << usersWeight.size() << " " << blkInfoMap.size());


}

FeedItemSeq LogicManager::get(int userId, int begin, int count){

}

void LogicManager::remove(int userId, Ice::Long feedId){

}

void LogicManager::removeAll(int userId){

}

v
