#ifndef RFEED_DB_DATA_H_
#define RFEED_DB_DATA_H_

#include <FeedDB.h>
//#include "FileArray.h"

namespace xce{
namespace feed{
namespace db{

//const char* DB_FILE_NAME = "rfeeddb";

class FeedDBDataI : public FeedDBData,
			public MyUtil::Singleton<FeedDBDataI>{
public:
	FeedDBDataI(){
		_inited = false;
	}
/*	void 			put(BlockInfo blkInfo, FeedItem feed);
		FeedItemSeq 	get(BlockInfo blkInfo, int begin, int count);
		bool			newFile(int fileid);
*/
	void			put(const FeedItem& feed, const BlockInfoPtr& blkInfo, const Ice::Current& = Ice::Current());
	void			modify(const FeedItem& feed, const BlockInfoPtr& blkInfo, const Ice::Current& = Ice::Current());
	void			putBatch(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq, const Ice::Current& = Ice::Current());	
	void			putBatchBinary(const FeedItem& feed, const string& data, const Ice::Current& = Ice::Current());	
	//void			putBatch(const Blk2FeedSeq& feeds, const Ice::Current& = Ice::Current());

	FeedItemSeq 	get(const BlockInfoPtr& blkInfo, int begin, int count, const Ice::Current& = Ice::Current());
	FileInfoPtr		queryFile(int fileId, const Ice::Current& = Ice::Current());
	ServiceInfoPtr		QueryServiceInfo(const Ice::Current& = Ice::Current());
	void			deleteFile(int fileId, const Ice::Current& = Ice::Current());
	bool			moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo, const Ice::Current& = Ice::Current());
	void			removeFeed(const BlockInfoPtr& blkInfo, Ice::Long feedId, const Ice::Current& = Ice::Current());

	void			updateConfig(const map<string, string>& cfgs, const Ice::Current& = Ice::Current());
	void			init();
	void			setIndex(int index){
		_index = index;
	}
private:
	//FileArray	_array;
	bool		_inited;
	string		_path;
	int		_index;
};

};
};
};
#endif
