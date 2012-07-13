#ifndef ID_CACHE_ICE
#define ID_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module idcache
{

	class IdRelationInfo{
		int directId;
		MyUtil::Int2IntSeqMap relatedIds;
	};

	class VirtualIdInfo{
		int virtualId;
		int userId;
		int transId;
	};

	sequence<VirtualIdInfo> VirtualIdInfoSeq;
	
	class UserId2RelatedIdInfo
	{
		int doubleAccountId;
		VirtualIdInfoSeq virtualIds;
	};

	class DoubleAccountId2RelatedIdSetter
	{
		void setData(MyUtil::ObjectResult data);
		void setValid(bool flag);
	};
	
	class VirtualId2RelatedIdSetter
	{
		void setData(MyUtil::ObjectResult data);
		void setValid(bool flag);
	};

	class IdCacheReader{
		IdRelationInfo getIdRelationInfo(int id);
		int getRelatedUserId(int virtualId);
		int getRelatedTransId(int virtualId);
		void createIdRelation(int id, int transId, int virtualId);
		void deleteIdRelation(int id, int virtualId);

		bool isVirtualId(int id);

		void setData(MyUtil::ObjectResult data);
	};

	class IdCacheWriter{
		void createIdRelation(int id, int transId, int virtualId);
		void deleteIdRelation(int id, int virtualId);
		void reload(int id);
		
		void setValid(bool flag);
	};
};

#endif
