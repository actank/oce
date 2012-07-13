#ifndef _PAGEFANS_ICE_
#define _PAGEFANS_ICE_

#include <PageCommon.ice>
#include <ObjectCache.ice>

module xce {
module buddy {

interface PageFansManager {	
	
	PageEntry getFan(int pageId, int userId);	
	//PageEntry getFanById(int id, int pageId, int userId);
	
	//MyUtil::IntSeq getFansIds(int pageid, int begin , int limit);
	PageEntryList getFans( int pageId,int begin , int limit);
	PageEntryList getFansByUserIds(int pageId, MyUtil::IntSeq  userIds);
	PageEntryList getFansByState(int pageId, int state, int begin,  int limit);	
	
	int getFansCountByState(int pageId, int state);
	bool isFan(int pageId, int userId);

	//-- for fans ------
	int addFan(PageEntry pe);
	int removeFan(int pageid, int userid);
	int updateFanWithState(PageEntry pe);
	int updateFansWithState(int pageId, int oriState, int tarState);
	
	//----for black list ---
	int addBlock(int pageId, int userId, int operatorId, int timestamp);
	int removeBlock(int pageId, int userId);
	int findBlock(int pageId, int userId);
	PageEntryList getBlockList(int pageId);

	void setValid(bool valid);
	void setData(MyUtil::ObjectResult data);
	bool isValid();
};

interface PageFansReloader{
	int addFan(PageEntry pe);
	int removeFan(int pageid, int userid);
	int updateFanWithState(PageEntry pe);
	int updateFansWithState(int pageId, int oriState, int tarState);

	int addBlock(int pageId, int userId, int operatorId, int timestamp);
	int removeBlock(int pageId, int userId);
	
	void reload(int id);
	void setValid(bool valid);
	bool isValid();
};


};
};

#endif

