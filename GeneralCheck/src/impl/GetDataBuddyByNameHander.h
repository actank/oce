#ifndef __GETDATA_BUDDYBYNAME_HANDLER_H__
#define __GETDATA_BUDDYBYNAME_HANDLER_H__
namespace xce {
namespace generalcheck {
class GetDataBuddyByNameHandler : public GetDataHandler {
public:
	GetDataBuddyByNameHandler(const string&name) : GetDataHandler(name) {
	}

	virtual MyUtil::ObjectMap getData(const MyUtil::LongSeq& ids) {
		MCE_INFO("debug GetDataBuddyByNameHandler::getData ids size = " << ids.size() << " begins");
		MyUtil::ObjectMap res;
		MyUtil::Int2IntMap counts = BuddyByNameCacheAdapter::instance().getFriendCounts(MyUtil::IntSeq(ids.begin(), ids.end()));
		if(counts.size() != ids.size()) {
			MCE_INFO("debug GetDataBuddyByNameHandler::getData Exception: getFriendCounts failed ids.size = " << ids.size() << " resultMap.size = "
					<< counts.size());	
		}
		MyUtil::Int2IntMap::const_iterator endm = counts.end();
		for(MyUtil::LongSeq::const_iterator it = ids.begin(), end = ids.end(); it != end; ++it) {
			MyUtil::Int2IntMap::const_iterator itm = counts.find(*it);
			if(itm == endm) {
				MCE_INFO("getFriendCounts redo id = " << *it);
				continue;
			}
			CheckDataPtr data = new CheckData(itm->second);
			res.insert(make_pair(*it, data));
		}
		MCE_INFO("debug GetDataBuddyByNameHandler::getData ids size = " << ids.size() << " end");
		return res;	
	}
}; 
}
}
#endif
