#ifndef FEED_DBT_ADAPTER_H_
#define FEED_DBT_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedDB.h"

namespace xce{
namespace feed{



using namespace MyUtil;

class FeedDBDataTAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBDataTAdapter> {
public:
	FeedDBDataTAdapter(){
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
		return "FeedDBDataT";
	}
	virtual string endpoints() {
		return "@FeedDBDataT";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedDBDataPrx getManager(int id, int timeout=260);
	FeedDBDataPrx getManagerOneway(int id);

	vector<FeedDBDataPrx> _managersOneway;
	vector<FeedDBDataPrx> _managers;
};


class FeedDBMasterTAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBMasterTAdapter> {
public:
	FeedDBMasterTAdapter(){
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
		return "FeedDBMasterT";
	}
	virtual string endpoints() {
		return "@FeedDBMasterT";
	}
	virtual size_t cluster() {
		return 0;
	}

	FeedDBMasterPrx getManager(int id, int timeout=260);
	FeedDBMasterPrx getManagerOneway(int id);

	vector<FeedDBMasterPrx> _managersOneway;
	vector<FeedDBMasterPrx> _managers;
};
class FeedDBTAdapter : public Singleton<FeedDBTAdapter>{
public:
	void			put(const FeedItem& feed, const Int2IntMap& userWeight);
	FeedItemSeq     get(int userId, int begin, int count);
	void		remove(int userId, Ice::Long feedId);
	void		removeAll(int userId); 
};

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
