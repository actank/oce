#ifndef FEED_DB_ADAPTER_H_
#define FEED_DB_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedDB.h"

namespace xce{
namespace feed{



using namespace MyUtil;

class FeedDBDataAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBDataAdapter> {
public:
	FeedDBDataAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void            put( const FeedItem& feed, const BlockInfoPtr& blkInfo);
	void            putBatch(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq);
	void            putBatchBinary(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq);
	//void            putBatch(const FeedItem& feed, const Int2IntMap& userWeight);
	FeedItemSeq     get(const BlockInfoPtr& blkInfo, int begin, int count);

	//FileInfo		queryFile(int fileId);
	bool		moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo);
	void		removeFeed(const BlockInfoPtr& blkInfo, Ice::Long feedId);

	int				getCluster(){
		return (int)cluster();
	}

	BlockInfoPtr decodeData(const string& data);
	string	encodeData(const BlockInfoPtr& blkInfo);
	string	encodeDataSeq(const BlockInfoSeq& seq);
	BlockInfoSeq decodeDataSeq(const string& data);
protected:
	virtual string name() {
		return "FeedDBData";
	}
	virtual string endpoints() {
		return "@FeedDBData";
	}
	virtual size_t cluster() {
		return 5;
	}

	FeedDBDataPrx getManager(int id, int timeout=260);
	FeedDBDataPrx getManagerOneway(int id);

	vector<FeedDBDataPrx> _managersOneway;
	vector<FeedDBDataPrx> _managers;
};


class FeedDBMasterAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBMasterAdapter> {
public:
	FeedDBMasterAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	BlockInfoPtr	getBlock(int userid, bool write=false);
	BlockInfoPtr	newBlock(int userid);
	void		removeBlock(int userid);
	void			putBatch(const FeedItem& feed, const Int2IntMap& userWeight);
	BlockInfoSeq	getBlockBatch(const IntSeq& ids, bool write=false);
	BlockInfoSeq	newBlockBatch(const IntSeq& ids);
	void		decTotalCount(const UserFeedCountSeq& feedCounts);
protected:
	virtual string name() {
		return "FeedDBMaster";
	}
	virtual string endpoints() {
		return "@FeedDBMaster";
	}
	virtual size_t cluster() {
		return 0;
	}

	FeedDBMasterPrx getManager(int id, int timeout=260);
	FeedDBMasterPrx getManagerOneway(int id);

	vector<FeedDBMasterPrx> _managersOneway;
	vector<FeedDBMasterPrx> _managers;
};
class FeedDBAdapter : public Singleton<FeedDBAdapter>{
public:
	void			put(const FeedItem& feed, const Int2IntMap& userWeight);
	FeedItemSeq     get(int userId, int begin, int count);
	void		remove(int userId, Ice::Long feedId);
	void		removeAll(int userId); 
};

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
