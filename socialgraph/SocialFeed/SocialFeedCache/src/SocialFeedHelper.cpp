#include "SocialFeedHelper.h"

using namespace MyUtil;
using namespace std;
using namespace xce::helper;
using namespace xce::md5;
using namespace xce::socialgraph;
using namespace com::xiaonei::xce;

void SocialFeedHelper::removeSocialFeed(int userId, const Feed& feedItem) {
	string tableName = TableName::FeedBlockTable( userId );
	MCE_INFO("[SocialFeedHelper] removeSocialFeed userId(" << userId << ") tableName(" << tableName << ")");
	IntSeq friendIds = feedItem.friendIds;
	IntSeq newCommonIds = feedItem.newCommonIds;

	if( friendIds.empty() || newCommonIds.empty() ) {
		return;
	}

	MCE_INFO("friendIds size = " << friendIds.size());
	for(IntSeq::iterator iter=friendIds.begin();
			iter!=friendIds.end(); ++iter) {
		MCE_INFO("friend id " << *iter);
	}

	MCE_INFO("newCommonIds size = " << newCommonIds.size());

	for(IntSeq::iterator iter=newCommonIds.begin();
			iter!=newCommonIds.end(); ++iter) {
		MCE_INFO("common id " << *iter);
	}

	sort( friendIds.begin(), friendIds.end() );
	sort( newCommonIds.begin(), newCommonIds.end() );

	ostringstream feedOrderedIdList;
	for(IntSeq::iterator iter=friendIds.begin();
			iter!=friendIds.end(); ++iter) {
		feedOrderedIdList << *iter;
		if( iter != friendIds.end() - 1) {
			feedOrderedIdList << ",";
		}
	}

	feedOrderedIdList << ";";	

	for(IntSeq::iterator iter=newCommonIds.begin();
			iter!=newCommonIds.end(); ++iter) {
		feedOrderedIdList << *iter;
		if( iter != newCommonIds.end() - 1 ) {
			feedOrderedIdList << ",";
		}
	}

	MCE_INFO("feedOrderedIdList : " << feedOrderedIdList.str());	

	long feedValue = MD5Util::stringToMD5( feedOrderedIdList.str() );

	MCE_INFO("feedValue : " << feedValue);

	Statement sqlInsert;
	sqlInsert << "insert into " << tableName 
			<< " (owner_id, feed_index) values (" 
			<< userId << "," << feedValue << ")" 
			<< " on duplicate key update feed_index=VALUES(feed_index);";

	try{	
		QueryRunner("social_feed", CDbWServer, tableName).execute( sqlInsert );
	} catch ( Ice::Exception& e ) {
		MCE_WARN("[SocialFeedHelper] removeSocialFeed id=" << userId 
				<< " Ice Exception " << e.what());
	} catch ( std::exception& e ) {
		MCE_WARN("[SocialFeedHelper] removeSocialFeed id=" << userId
				<< " std exception " << e.what());
	} catch ( ... ) {
		MCE_WARN("[SocialFeedHelper] removeSocialFeed id=" << userId
				<< " unknown exception ");
	}
}
