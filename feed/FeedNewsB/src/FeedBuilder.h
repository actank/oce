/*
 * FeedBuilder.h
 *
 *  Created on: Sep 14, 2011
 *      Author: yejingwei
 */
#ifndef FEEDBUILDER_H_
#define FEEDBUILDER_H_
#include "RFeed.h"
#include "FeedItemI.h"
#include "Common.h"
#include <boost/foreach.hpp>
#include "UserInfoCache.h"
#include "GetRequest.h"
namespace xce {
namespace feed {
//
//class FeedBuilder {
//public:
//	FeedBuilder(FeedContentDict & fdict,StypemergeSFeed1DMap & comments,bool show_comment)
//		:fdict_(fdict),comment_map_(comments), show_comment_(show_comment){}
//
//	FeedDataResPtr BuildFeedData(GetRequest &);
//	const set<long> & GetNullFids(){ return nullfids_;}
//
//private:
//	FeedDataPtr BuildAFeed(SFeed2D &);
//
//	bool BuildFrontFeed(SFeed1D & , FeedDataPtr &, FeedReplyPtr&,ostringstream & );
//	bool BuildComments(SFeed1D & , FeedDataPtr &, ostringstream & );
//
//	void AppendReplyToXml(FeedReplyPtr & reply, ostringstream & xml);
//	void AppendHiddenFidsToXml(set<long> & hiddens, ostringstream & xml);
//
//	bool ShouldAdd(const SharedFeedPtr & sfeed, int merge_count);
//	FeedContentPtr GetContent(long fid);
//
//private:
//	FeedContentDict & fdict_;
//	StypemergeSFeed1DMap & comment_map_;
//	bool show_comment_;
//	set<long> nullfids_;
//};
//
////---------------------------------------------------
//FeedDataResPtr FeedBuilder::BuildFeedData(GetRequest & req) {
//	FeedDataResPtr res = new FeedDataRes;
//
//	SFeed3D &  sfeed3d = req.GetShowedSharedFeed3D();
//
//	int suc = 0,fail = 0;
//	BOOST_FOREACH(SFeed2D & sfeed2d,sfeed3d) {
//		FeedDataPtr afeed = BuildAFeed(sfeed2d);
//		if(afeed) {
//			++suc;
//			afeed->type = TypeUtil::GetStype(afeed->type);
//			res->data.push_back(afeed);
//		}else{
//			++fail;
//		}
//	}
//	return res;
//}
//
//FeedDataResPtr FeedBuilder::BuildFeedData(GetRequest & req) {
//	FeedDataResPtr res = new FeedDataRes;
//
//	SFeed3D &  sfeed2d = req.GetShowedSharedFeed3D();
//
//	int suc = 0,fail = 0;
//	BOOST_FOREACH(SFeed1D & sfeed1d,sfeed2d) {
//		FeedDataPtr afeed = BuildAFeed(sfeed1d);
//		if(afeed) {
//			++suc;
//			afeed->type = TypeUtil::GetStype(afeed->type);
//			res->data.push_back(afeed);
//		}else{
//			++fail;
//		}
//	}
//	return res;
//}
//
//FeedDataPtr FeedBuilder::BuildAFeed(SFeed2D & sfeed2d) {
//	FeedDataPtr global_data;
//	FeedReplyPtr global_reply;
//	ostringstream os;
//	set<long> hiddens;
//
//	BOOST_FOREACH(SFeed1D & sfeed1d, sfeed2d) {
//		if(!global_data) {
//			BuildFrontFeed(sfeed1d,global_data,global_reply,os);
//		} else {
//			hiddens.insert(sfeed1d.front()->GetFid());
//		}
//	}
//
//	if (!global_data) return 0;
//
//	if(global_reply)
//		AppendReplyToXml(global_reply,os);
//
//	if(!hiddens.empty())
//		AppendHiddenFidsToXml(hiddens,os);
//
//	global_data->xml = os.str();
//	return global_data;
//}
//
//bool FeedBuilder::BuildFrontFeed(SFeed1D & sfeed1d ,
//		FeedDataPtr & global_data ,FeedReplyPtr& global_reply,ostringstream & os){
//	if(sfeed1d.empty() || global_data || global_reply){
//		MCE_WARN("FeedBuilder::BuildFrontFeed. input data and reply must be null!!");
//		return false;//考虑用异常
//	}
//	bool has_comment = false;
//	if(show_comment_ && MergeConfig::instance().IsMain(sfeed1d.at(0)->GetStype()))
//	 	has_comment = BuildComments(sfeed1d,global_data,os);
//	int added_count = 0;
//	BOOST_FOREACH(SharedFeedPtr & sfeed,sfeed1d) {
//		FeedContentPtr content = GetContent(sfeed->GetFid());
//		if(!content) {continue;}
//		if(!has_comment){ global_data = content->data;}//!!!如果有评论新鲜事，返回的data的数据结构必须是comment的
//		global_reply = content->reply;
//
//		if(!ShouldAdd(sfeed,added_count)) break; // 已经足够了
//
//		os << content->data->xml;//不能用global_data的xml
//		++added_count;
//	}
//	if(added_count == 0) { global_data = 0; global_reply = 0;}
//	return global_data;
//}
//
//bool FeedBuilder::BuildComments(SFeed1D & sfeed1d ,
//		FeedDataPtr & global_data ,ostringstream & os){
// 	static const int COMMENT_SIZE = 5;
//
// 	StypemergeSFeed1DMap::iterator it = comment_map_.find(sfeed1d.front()->GetStypeMerge());
//	if(it == comment_map_.end()) return false;
//
//	SFeed1D & comment1d = it->second;
//	int added_count = 0;
//	BOOST_FOREACH(SharedFeedPtr & comment,comment1d) {
//		FeedContentPtr content = GetContent(comment->GetFid());
//		if (!content) continue;
//		global_data = content->data;
//
//		os << global_data->xml;
//		++added_count;
//
//		if (added_count > COMMENT_SIZE) break;
//	}
//
//	return added_count > 0;
//}
//
//FeedContentPtr FeedBuilder::GetContent(long fid) {
//	FeedContentDict::const_iterator dict_it = fdict_.find(fid);
//	if(dict_it == fdict_.end()) {
////		MCE_INFO("FeedBuilder::GetContent. not found content. fid:" << fid);
//		return 0;//在memcache中不存在，在db中可能有
//	}
//	FeedContentPtr content = dict_it->second;
//	if(!content || !(content->data)){//防止data为空
//		nullfids_.insert(fid);
//		return 0;
//	}
//	return content;
//}
//
//bool FeedBuilder::ShouldAdd(const SharedFeedPtr & sfeed, int merge_count) {
//	int mergetype = sfeed->GetFeedNewsMergeType();
//	if(mergetype != FeedMergeReplace && mergetype != FeedMergeAppend){
//		MCE_WARN("FeedBuilder::ShouldAdd. unknown mergetype: " << mergetype << " fid:" << sfeed->GetFid());
//	}
//	static const int MERGECOUNT = 5;
//	return (sfeed->IsMergeReplace() && merge_count == 0 )
//			|| (sfeed->IsMergeAppend() && merge_count < MERGECOUNT);
//}
//
//void FeedBuilder::AppendReplyToXml(FeedReplyPtr & reply,ostringstream & xml ) {
//	xml << reply->oldReply
//			<< reply->newReply
//			<< "<reply_count>" << reply->count << "</reply_count>";
//}
//
//void FeedBuilder::AppendHiddenFidsToXml(set<long> & hiddens,ostringstream & xml) {
//	if (hiddens.empty()) { return; }
//
//	xml << "<merge_feed><count>" << hiddens.size() << "</count>";
//	BOOST_REVERSE_FOREACH(long fid,hiddens){
//		xml << "<fid>" << fid << "</fid>";
//	}
//	xml << "</merge_feed>";
//}


class FeedBuilderB {
public:
	FeedBuilderB(FeedContentDict & fdict,StypemergeSFeed1DMap & comments,bool show_comment)
		:fdict_(fdict),comment_map_(comments), show_comment_(show_comment), succ_(0), fail_(0),splitter_(0){}

	FeedDataResPtr BuildFeedData(GetRequest &);
	const set<long> & GetNullFids(){ return nullfids_;}
	int GetSucc() const { return succ_;}
	int GetFail() const { return fail_;}
	int GetSplitter() const { return splitter_;}
private:
	FeedDataPtr BuildAFeed(SFeedPtr1D &);
	FeedDataPtr BuildSplitterData(SharedFeedPtr & sfeed,GetRequest & req);
//	bool BuildFrontFeed(SFeedPtr1D & , FeedDataPtr &, FeedReplyPtr&,ostringstream & );
	bool BuildComments(SFeedPtr1D & , FeedDataPtr &, ostringstream & );

	void AppendReplyToXml(FeedReplyPtr & reply, ostringstream & xml);
//	void AppendHiddenFidsToXml(set<long> & hiddens, ostringstream & xml);

	bool ShouldAdd(const SharedFeedPtr & sfeed, int merge_count);
//	FeedContentPtr GetContent(long fid);
	FeedContentPtr GetContent(SharedFeedPtr & sfeed);
	void AppendFakeReply(bool ismark,string & xml);
	void RemoveTailSplitter(FeedDataSeq & dataseq);
private:
	FeedContentDict & fdict_;
	StypemergeSFeed1DMap & comment_map_;
	bool show_comment_;
	set<long> nullfids_;
	int succ_;
	int fail_;
	int splitter_;
};


//class FeedData{
//    long feed;		// type and stype 都在ID的高位指定
//	long source;  //可以考虑不要
//	int actor;
//	int type;	// 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
//	string xml;
//	int time;
//	int weight; 	//高8位暂时无用,次高8位为baseWeightId, 低16位为weight
////		int commentCount;
//};
//---------------------------------------------------

void FeedBuilderB::RemoveTailSplitter(FeedDataSeq & dataseq){
	bool succ = false;
	if(!dataseq.empty() && FeedDataUtil::GetStype((*dataseq.rbegin())->type) == kSplitterStype){
		succ = true;
		dataseq.pop_back();
	}
	MCE_INFO("FeedBuilderB::RemoveTailSplitter. remove one splitter. succ:" << succ);
}
FeedDataPtr FeedBuilderB::BuildSplitterData(SharedFeedPtr & sfeed,GetRequest & req) {
	FeedDataPtr data = new FeedData();
	data->feed = sfeed->GetFid();
	data->source = 0;
	data->actor = sfeed->GetActor();
	data->type = kSplitterStype | 0x10000;
	data->time = sfeed->GetTime();
	ostringstream os;
	long xmltime = sfeed->GetTime()* 1000;
	MyUtil::Date date(sfeed->GetTime());
	os << "<f version=\"1\" tpl=\"cs\"><time>" << xmltime << "</time><timeStr>"<<date.toDateTime()<<"</timeStr></f>";
	data->xml = os.str();
	data->weight = 0;
	MCE_INFO("FeedBuilderB::BuildSplitterData. xmltime:" << xmltime << " xml:" << data->xml);
	return data;
}

FeedDataResPtr FeedBuilderB::BuildFeedData(GetRequest & req) {
	FeedDataResPtr res = new FeedDataRes;

	SFeedPtr2D &  sfeed2d = req.GetShowedSharedFeed2D();
	MCE_INFO("FeedBuilderB::BuildFeedData. line:" << __LINE__ << " marked count:" << req.marked_fids.size());
	vector<FeedDataPtr> dataseq;
//	map<long,long> refreshes = req.GetRefreshes();
	BOOST_FOREACH(SFeedPtr1D & sfeed1d,sfeed2d) {


		SharedFeedPtr & sfeed = sfeed1d.at(0);
		if(sfeed->IsSplitter()){
			if(res->data.empty())
				continue;
			if(!res->data.empty() && FeedDataUtil::GetStype((*res->data.rbegin())->type) == kSplitterStype)
				continue;
			FeedDataPtr afeed = BuildSplitterData(sfeed,req);
			res->data.push_back(afeed);
			++splitter_;
			continue;
		}
		FeedDataPtr afeed = BuildAFeed(sfeed1d);
		if(afeed) {
			++succ_;
//			afeed->type = FeedDataUtil::GetStype(afeed->type);
			bool ismarked = req.marked_fids.count(sfeed1d.front()->GetFid());
			afeed->weight = FeedDataUtil::SetMark(afeed->weight,ismarked);
			
			res->data.push_back(afeed);
		}else{
			++fail_;
		}
	}

	RemoveTailSplitter(res->data);
	MCE_INFO("FeedBuilderB::BuildFeedData. res FeedData count:" << res->data.size() << " splitter count:" << splitter_);
//	MCE_INFO("FeedBuilderB::BuildFeedData. line:" << __LINE__ << " marked count:" << req.marked_fids.size());
	return res;
}

//FeedDataResPtr FeedBuilderB::BuildFeedData(GetRequest & req) {
//	FeedDataResPtr res = new FeedDataRes;
//
//	SFeedPtr2D &  sfeed2d = req.GetShowedSharedFeed3D();
//
//	int suc = 0,fail = 0;
//	BOOST_FOREACH(SFeedPtr1D & sfeed1d,sfeed2d) {
//		FeedDataPtr afeed = BuildAFeed(sfeed1d);
//		if(afeed) {
//			++suc;
//			afeed->type = TypeUtil::GetStype(afeed->type);
//			res->data.push_back(afeed);
//		}else{
//			++fail;
//		}
//	}
//	return res;
//}

FeedDataPtr FeedBuilderB::BuildAFeed(SFeedPtr1D & sfeed1d) {
	FeedDataPtr global_data;
	FeedReplyPtr global_reply;
	ostringstream os;
	bool has_comment = false;
	if(show_comment_ && MergeConfig::instance().IsMain(sfeed1d.at(0)->GetStype())){
		has_comment = BuildComments(sfeed1d,global_data,os);
	}
	int added_count = 0;
	BOOST_FOREACH(SharedFeedPtr & sfeed,sfeed1d) {
		FeedContentPtr content = GetContent(sfeed);
		if(!content) {
//			MCE_INFO("content is null");
			continue;
		}
		//!!!如果有评论新鲜事，返回的FeedData的数据结构必须是comment
		if(!global_data){
			global_data = content->data;
			global_reply = content->reply;
		}
		//如果有评论，最好能取到一条reply
		if(has_comment && !global_reply){
			global_reply = content->reply;
		}
		if(!ShouldAdd(sfeed,added_count)) {
			MCE_INFO("not add");
			break;
		// 已经足够了
		}
		os << content->data->xml;//不能用global_data的xml
		++added_count;
	}
//	MCE_INFO("FeedBuilderB::BuildAFeed. line:" << __LINE__ << " added_count:" << added_count);
//	MCE_INFO("global_data:" <<( global_data == 0) << " has_comment:" << has_comment << " stype:" <<sfeed1d.at(0)->GetStype() );
	if(added_count == 0) {
		global_data = 0;
		global_reply = 0;
		MCE_INFO(__FILE__ << ":" << __LINE__);
		return 0;
	}
	if(global_reply){
		AppendReplyToXml(global_reply,os);
	}
	global_data->xml = os.str();

	return global_data;
}


bool FeedBuilderB::BuildComments(SFeedPtr1D & sfeed1d ,
		FeedDataPtr & global_data ,ostringstream & os){
 	static const int COMMENT_SIZE = 5;

 	StypemergeSFeed1DMap::iterator it = comment_map_.find(sfeed1d.front()->GetStypeMerge());
	if(it == comment_map_.end()) return false;

	SFeedPtr1D & comment1d = it->second;
	int added_count = 0;
	BOOST_FOREACH(SharedFeedPtr & comment,comment1d) {
		FeedContentPtr content = GetContent(comment);
		if (!content) continue;
		global_data = content->data;

		os << global_data->xml;
		++added_count;

		if (added_count > COMMENT_SIZE) break;
	}
	MCE_INFO(__FILE__ << ":" << __LINE__);

	return added_count > 0;
}

//FeedContentPtr FeedBuilderB::GetContent(long fid) {
//	FeedContentDict::const_iterator dict_it = fdict_.find(fid);
//	if(dict_it == fdict_.end()) {
//		MCE_INFO("FeedBuilderB::GetContent. not found content. fid:" << fid);
//		return 0;//在memcache中不存在，在db中可能有
//	}
//	FeedContentPtr content = dict_it->second;
//	if(!content || !(content->data)){//防止data为空
//		nullfids_.insert(fid);
//		return 0;
//	}
//	return content;
//}

FeedContentPtr FeedBuilderB::GetContent(SharedFeedPtr & sfeed) {
	long fid = sfeed->GetFid();
	FeedContentDict::const_iterator dict_it = fdict_.find(fid);
	if(dict_it == fdict_.end()) {
//		MCE_INFO("FeedBuilderB::GetContent. not found content. fid:" << fid << " stype:" << sfeed->GetStype()());
		return 0;//在memcache中不存在，在db中可能有
	}
	FeedContentPtr content = dict_it->second;
	if(!content || !(content->data)){//防止data为空
		MCE_INFO("FeedBuilderB::GetContent. null content. fid:" << fid << " stype:" << (sfeed->GetStype()));
		nullfids_.insert(fid);
		return 0;
	}
	return content;
}

bool FeedBuilderB::ShouldAdd(const SharedFeedPtr & sfeed, int merge_count) {
	int mergetype = sfeed->GetFeedNewsMergeType();
	if(mergetype != FeedMergeReplace && mergetype != FeedMergeAppend){
		MCE_WARN("FeedBuilderB::ShouldAdd. unknown mergetype: " << mergetype << " fid:" << sfeed->GetFid());
	}
	static const int MERGECOUNT = 5;
	return (sfeed->IsMergeReplace() && merge_count == 0 )
			|| (sfeed->IsMergeAppend() && merge_count < MERGECOUNT);
}

void FeedBuilderB::AppendReplyToXml(FeedReplyPtr & reply,ostringstream & xml ) {
//	return;//TODO
	xml << reply->oldReply
			<< reply->newReply
			<< "<reply_count>" << reply->count << "</reply_count>";
}
void FeedBuilderB::AppendFakeReply(bool ismark,string & xml){
	ismark = true;
	string flag = ismark?"TOP":"TIMELINE";
	const static string fakereply ="<f> <reply> <id>9169373603</id> <type>0</type> <time>2011-11-17 16:24</time> <body>&quot;c&quot;</body> <im> <body>c</body> </im> <from> <id>347348947</id> <name>贾~@~K</name>name> <tinyimg>http://hdn.xnimg.cn/photos/hdn121/20111103/1830/tiny_RhFW_181334p019118.jpg</tinyimg> </from> </reply> </f><reply_count>1</reply_count>";
			//	const static string fakereply = "<f> <reply> <id>1504368671</id> <type>0</type> <time>2011-11-17 11:46</time> <body>&quot;"+flag+"!!&quot;</body> <im> <body>TOP</body> </im> <from> <id>244271872</id> <name>机器人</name> <tinyimg>http://hdn.xnimg.cn/photos/hdn221/20110811/2025/tiny_fXPe_88278e019118.jpg</tinyimg> </from> </reply> </f><reply_count>9</reply_count>";
	xml += fakereply;
}
};
};

#endif
