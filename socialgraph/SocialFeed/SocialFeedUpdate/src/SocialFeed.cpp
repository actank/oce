#include "SocialFeed.h"
using namespace xce::socialgraph;
using namespace MyUtil;

SocialFeed::SocialFeed(const vector<USERID>& friends, const vector<USERID>& strangers,
		const map<int,float>& offerfriend_sort)
{
	// sort _strangers by offerfriends
	// TODO: 对friends按照of排序会更好，效率考虑，暂不做
	for (vector<USERID>::const_iterator iter = friends.begin();
			iter != friends.end(); ++iter)
	{
		_friends.push_back(make_pair<int,float>(*iter,0));
	}

	for (vector<USERID>::const_iterator iter = strangers.begin();
			iter != strangers.end(); ++iter)
	{
		float weight = 0;
		map<int,float>::const_iterator ofIter = offerfriend_sort.find(*iter);
		if (ofIter != offerfriend_sort.end())
			weight = ofIter->second;
		_strangers.push_back(make_pair<int,float>(*iter,weight));
	}
	sort(_strangers.begin(), _strangers.end(), FeedInnerComp());
}

char SocialFeed::GetType() const
{
	if (_friends.size()>1 && _strangers.size()>1)
		return 'c';
	if (_friends.size()>1 && _strangers.size()==1)
		return 'b';
	return 'a';  // if (_friends.size()==1 && _strangers.size()>0)

}

long SocialFeed::Finger() const
{
	Feed feed;
	Transfer(feed);
	sort( feed.friendIds.begin(), feed.friendIds.end() );
	sort( feed.newCommonIds.begin(), feed.newCommonIds.end() );

	ostringstream feedOrderedIdList;
	for(IntSeq::iterator iter=feed.friendIds.begin();
			iter!=feed.friendIds.end(); ++iter) {
		feedOrderedIdList << *iter;
		if( iter != feed.friendIds.end() - 1) { 
			feedOrderedIdList << ","; 
		}
	}

	feedOrderedIdList << ";";

	for(IntSeq::iterator iter=feed.newCommonIds.begin();
			iter!=feed.newCommonIds.end(); ++iter) {
		feedOrderedIdList << *iter;
		if( iter != feed.newCommonIds.end() - 1 ) {
			feedOrderedIdList << ",";
		}
	}

	//MCE_INFO("feedOrderedIdList : " << feedOrderedIdList.str());
	long feedValue = xce::md5::MD5Util::stringToMD5( feedOrderedIdList.str() );
	return feedValue;
}

int SocialFeed::Transfer(Feed& feed) const
{
	const size_t MAXNUM_FRIEND = 4;
	const size_t MAXNUM_COMMON = 6;
	feed.friendIds.reserve(_friends.size() < MAXNUM_FRIEND ? _friends.size() : MAXNUM_FRIEND);
	feed.newCommonIds.reserve(_strangers.size() < MAXNUM_COMMON ? _strangers.size() : MAXNUM_COMMON);

	for (vector<pair<USERID,float> >::const_iterator iter = _friends.begin();
			iter != _friends.end(); ++iter)
	{
		feed.friendIds.push_back(iter->first);
		if (feed.friendIds.size() >= MAXNUM_FRIEND)
			break;
	}
	for (vector<pair<USERID,float> >::const_iterator iter = _strangers.begin();
			iter != _strangers.end(); ++iter)
	{
		feed.newCommonIds.push_back(iter->first);
		if (feed.newCommonIds.size() >= MAXNUM_COMMON)
			break;
	}
	return 0;
}

int SocialFeed::Print() const
{
	for (vector<pair<USERID,float> >::const_iterator iter = _friends.begin();
			iter != _friends.end(); ++iter)
	{
		if (iter != _friends.begin())
			fprintf(stderr, ",");
		fprintf(stderr, "%d_%.2f", iter->first, iter->second);
	}

	fprintf(stderr, " + ");

	for (vector<pair<USERID,float> >::const_iterator iter = _strangers.begin();
			iter != _strangers.end(); ++iter)
	{
		if (iter != _strangers.begin())
			fprintf(stderr, ",");
		fprintf(stderr, "%d_%.2f", iter->first, iter->second);
	}
	fprintf(stderr, "\n");
	return 0;
}

