/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDSORTI_H_
#define FEEDSORTI_H_

#include "FeedSort.h"
#include <queue>
#include "Singleton.h"
#include "TaskManager.h"
#include "NewsUtil.h"
//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
using xce::feed::MergeUtil::MergeConfig;
using xce::feed::MergeUtil::FeedWrapperHandler;
using xce::feed::SortParam;
namespace xce {
namespace feed {

class SortRequest;
typedef vector<FeedWrapper> FeedWrapper1D;
typedef vector<vector<FeedWrapper> > FeedWrapper2D;
typedef vector<vector<vector<FeedWrapper> > > FeedWrapper3D;

typedef vector<Ice::Long> Long1D;
typedef vector<Long1D> Long2D;
typedef vector<Long2D> Long3D;

typedef map<pair<int, long> , FeedWrapper1D> StypeMerge_FW1D_Map;

class FeedSortI: public FeedSort, public MyUtil::Singleton<FeedSortI> {
public:
	virtual SortResultPtr Sort(int uid, const ::xce::feed::SortParamPtr & param,
			const Ice::Current& = Ice::Current());
private:
	void MergeFeed(int uid,const SortParamPtr & param, SortRequest & request);
	void GatherFeed(int uid,SortRequest & request,Long3D & fid3d);
	void GatherFeedSimple(int uid,SortRequest & request,Long3D & fid3d);
	float GetScore(int uid, const FeedWrapper & fw, bool sort);
//	float GetScoreTest(int uid, const FeedWrapper & fw, bool sort);
//	float GetScoreTestV2(int uid, const FeedWrapper & fw, bool sort);
	//暂时不用
	void ReplaceByCommentScoreAndPushComments(
			int uid,
			SortRequest & request,
			map<pair<int,long >, float> & stypemerge_score);
};

class SortRequest {
public:
	SortRequest(int uid, const SortParamPtr & param) :
		uid_(uid), 
		random_fid_(0),
		show_comment_(param->showcomment),
		show_edm_(param->showedm), 
		show_random_(param->showrandom),
	 	sort_(param->sort),
		gather_by_time_(param->gatherbytime),
		last_get_time_(param->lastGetTime),
		begin_(param->begin),
		limit_(param->limit){
	}

	bool SeperateCommentEdmRandom(FeedWrapper & fw);
	void ExportEdmAndRandom(Long3D &fid3d);
	void PushCommentsIfExist(const FeedWrapper & fw, Long1D & fid1d);
	string ToString();
	int GetEdmCount(){
		if(show_edm_ && begin_==0 && limit_ > 0){
			return edms_.size();
		}else{
			return 0;
		}
	}
	int uid_;
	long random_fid_;
	bool show_comment_;
	bool show_edm_;
	bool show_random_;
	bool sort_;//按照权重排序还是不排序
	bool gather_by_time_;
	time_t last_get_time_;
	int begin_;
	int limit_;
	map<pair<int, long> , FeedWrapper1D> comments_;
	FeedWrapper1D edms_;

	StypeMerge_FW1D_Map stypemerge_fw1d_;
	multimap<float, StypeMerge_FW1D_Map::iterator> score_map_;
	map<pair<int,int>,vector<pair<int,long> > > stypeactor_stypemerge;
};

}
}
#endif /* FEEDQUEUEI_H_ */
