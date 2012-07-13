/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDSORTI_H_
#define FEEDSORTI_H_

#include <queue>
#include "Singleton.h"
#include "TaskManager.h"
#include "NewsUtil.h"
//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
using xce::feed::MergeUtil::MergeConfig;
using xce::feed::MergeUtil::FeedItemHandler;
using xce::feed::SortParam;
namespace xce {
namespace feed {

class GetRequest;


typedef map<pair<int, long> , FeedItem1D> StypeMerge_FW1D_Map;


class SortHelper: public MyUtil::Singleton<SortHelper> {
public:
	SortResultPtr MergeAndSort(int uid, const ::xce::feed::SortParamPtr & param,
			);
private:
	void MergeFeed(GetRequest & req);
	void MergeByStypeMergeAndCalScore(GetRequest & req);
	void MakeScoreIteratorMap(GetRequest & req);
	void GatherFeed(int uid,GetRequest & request,Long3D & fid3d);
	void GatherFeedSimple(int uid,GetRequest & request,Long3D & fid3d);
	float GetScore(int uid, const FeedItem & fu, bool sort);
	float GetScoreTest(int uid, const FeedItem & fu, bool sort);
	//暂时不用
	void SeperateCommentEdmRandom(
			int uid,
			GetRequest & request,
			map<pair<int,long >, float> & stypemerge_score);
};

class GetRequest {
public:
	GetRequest(int uid) //TODO
	{
		uid_ = uid;
		hot_ = hot;
		begin_ = begin;
		limit_ = limit;
		by_type_not_stype_ = by_type_not_stype;
	}

	bool SeperateCommentAndRandom(FeedItem & fu);
	void ExportEdmAndRandom(Long3D &fid3d);
	void PushCommentsIfExist(const FeedItem & fu, Long1D & fid1d);
	string ToString();
	int GetEdmCount(){
		if(show_edm_ && begin_==0 && limit_ > 0){
			return edms_.size();
		}else{
			return 0;
		}
	}
	bool sort_;
	bool by_type_not_stype_;

	int uid_;
	int begin_;
	int limit_;

	//附加信息
	typedef map<pair<int, long> , FeedItemPtr1D> StypemergeMap;
	StypemergeMap comments_;
	StypemergeMap stypemerge_map_;
	long random_fid_;
	FeedItem1D edms_;
	FeedItemSeq orig_items_;
	FeedItemSeq orig_edms_;
	FeedItem3D item3d;

	StypemergeMap stypemerge_fw1d_;
	multimap<float, StypemergeMap::iterator> score_map_;
	map<pair<int,int>,vector<pair<int,long> > > stypeactor_stypemerge;
};

//class MiddleData{
//public:
//	FeedItemSeq items;
//	map<pair<int, long> , FeedItem1D> comments_;
//	map<pair<int, long> , FeedItem1D> stypemerge_fu1d_;//第一次合并后的结果
//	map<float,map<pair<int, long> , FeedItem1D>::iterator > score_map_;
//	long random_fid_;
//	FeedItem1D edms_;
//};

class ViewCountPool{
public:
	void Get(int uid,hash_map & view_map);
	void Inc(int uid,const set<long> & fids);
};

}
}
#endif /* FEEDQUEUEI_H_ */
