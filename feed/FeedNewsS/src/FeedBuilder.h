/*
 * FeedBuilder.h
 *
 *  Created on: Sep 14, 2011
 *      Author: yejingwei
 */
#include <boost/foreach.hpp>
#include "RFeed.h"
namespace xce {
namespace feed {

class FeedBuilder {
public:
	static FeedDataSeq BuildFeedData();
private:
	static FeedDataPtr BuildAFeed(set<long> & nullfids);
	static void BuildFeedData();
	static void BuildFeedWithComments();
	static int GetContent(long fid, FeedContentDict & fdict,
			FeedContentPtr & content);
	static void ShouldAdd(int merge_type, int merge_count);
	static bool BuildShowedFeed(FeedUnit1D & fu1d, removedfids,
			set<long> & nullfids);
	static void AppendReplyToXml(ostringstream & xml, FeedReplyPtr & reply);
	static void AppendHiddenFids(ostringstream & xml, set<long> & hidens);
};

FeedDataSeq FeedBuilder::BuildFeedData() {
	FeedDataSeq dataseq;
	BuildEdmsAndRandom();
		BOOST_FOREACH(FeedWrapper2D & fw2d,fw3d)
	{	FeedDataPtr afeed = BuildAFeed(fw2d);
		if(afeed) {
			dataseq.push_back(afeed);
		}
	}
}
FeedDataPtr FeedBuilder::BuildAFeed(set<long> & nullfids) {
	FeedDataPtr data;
	FeedReplyPtr reply;
	ostringstream xml;
	vector<Ice::Long> valid_feedids;
	int stype = 0;
	int gather_count = 0;
	bool get_showed = false;
	BOOST_FOREACH(FeedUnit1d & fu1d, fu2d)
	{	if(!get_showed) {
			get_showed = BuildShowedFeed(fdict,data,reply,xml,nullfids);
		} else {
			GetHiddenFeedIds(hiden_fids);
		}
	}
	if (!get_showed) {
		return 0;
	}
	AppendReplyToXml(xml, reply);
	AppendHidenFids(xml, hidens);
	data->xml = xml.str();
	return data;
}
void FeedBuilder::BuildFeedData() {
	BuildEdmsAndRandom();
	BOOST_FOREACH(FeedWrapper2D & fw2d,fw3d){
		BuildAFeed(fw2d);
	}
}
void FeedBuilder::BuildFeedWithComments() {

}
int FeedBuilder::GetContent(long fid, FeedContentDict & fdict,
		FeedContentPtr & content) {
	FeedContentDict::const_iterator dict_it = fdict.find(fid);
	if (dict_it != fdict.end()) {
		if (dict_it->second) {
			content = dict_it->second;
			return 1;//在memcache中找到
		} else {
			return -1;//空指针，在db中也不存在
		}
	} else {
		return 0;//在memcache中不存在，在db中可能有
	}
}
void FeedBuilder::ShouldAdd(int merge_type, int merge_count) {
	static int MERGECOUNT = 5;
	return (merge_type == FeedMergeAppend && merge_count < MERGECOUNT)
			|| (merge_type == FeedMergeAppend && merge_count < MERGECOUNT);
}
bool FeedBuilder::BuildShowedFeed(FeedUnit1D & fu1d, removedfids,
		set<long> & nullfids) {
	FeedDataPtr data;
	FeedReplyPtr reply;
	ostringstream os;
	int merge_count = 0;
	BOOST_FOREACH(FeedUnit & fu,fu1d)
	{	FeedContentPtr content = 0;
		int flag = GetContent(fu.GetFeedId(),fdict,content);
		if(flag == 1) {
			if(!data) {
				data = content->data;
				reply = content->reply;
			}
			if(ShouldAdd(merge_type,merge_count)) {
				os << data->xml;
				++merge_count;
			}
		} else if(flag == -1) {
			nullfids.insert(fu.GetFeedId());
			continue;
		}
	}
	return merge_count > 0;
}
void FeedBuilder::AppendReplyToXml(ostringstream & xml, FeedReplyPtr & reply) {
	xml << reply->oldReply << reply->newReply << "<reply_count>" << reply->count
	<< "</reply_count>";
}
void FeedBuilder::AppendHiddenFids(ostringstream & xml, set<long> & hidens) {
	if (hidens.empty()) {
		return;
	}
	static string TAG = "merge_feed";
	xml << "<" << TAG << "><count>" << valid_feedids.size() << "</count>";
	for (size_t j = 0; j < valid_feedids.size(); j++) {
		xml << "<fid>" << valid_feedids.at(j) << "</fid>";
	}
	xml << "</" << TAG << ">";
}
};
};
