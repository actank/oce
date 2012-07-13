#ifndef ID_CACHE_READER_I_H
#define ID_CACHE_READER_I_H

#include <ServiceI.h>
#include "TaskManager.h"
#include <IdCache.h>
#include <ext/hash_set>
#include "user/IdCache/share/IdCacheDataI.h"

namespace idcache
{
	const int USERID_RELATED = 0; 
	const int VIRTUAL_ID = 1; 

	const std::string DOUBLEACCOUNTID2RELATEDID_SETTER = "DS";
	const std::string VIRTUALID2RELATEDID_SETTER = "VS";
	const std::string ID_CACHE = "M";

	const int MIN_FILTER_ID = 2000000000;
	const int MAX_FILTER_ID = 2100000000;
	
	const int TASK_LEVEL_FILL = 1;

	class IdCacheReaderI : public IdCacheReader, public MyUtil::Singleton<IdCacheReaderI>
	{
		public:
			IdCacheReaderI();
			IdRelationInfoPtr getIdRelationInfo(::Ice::Int id, const ::Ice::Current& current);
			int getRelatedUserId(::Ice::Int virtualId, const ::Ice::Current& current);
			int getRelatedTransId(::Ice::Int virtualId, const ::Ice::Current& current);
			void createIdRelation(::Ice::Int id, ::Ice::Int transId, ::Ice::Int virtualId, const ::Ice::Current& current);
			void deleteIdRelation(::Ice::Int id, ::Ice::Int virtualId, const ::Ice::Current& current);

			bool isVirtualId(::Ice::Int id, const ::Ice::Current& current);

			void setData(const MyUtil::ObjectResultPtr& data, const ::Ice::Current& current);
			void setValid(bool flag);

		private:
			UserId2RelatedIdInfoIPtr locateUserId2RelatedIdInfo(int id);
			VirtualIdInfoIPtr locateVirtualIdInfo(int id);
			bool isValid();
		private:
			bool _isValid;
			IceUtil::Mutex _mutex;
	};

	class DoubleAccountId2RelatedIdSetterI : public DoubleAccountId2RelatedIdSetter, public MyUtil::Singleton<DoubleAccountId2RelatedIdSetterI>
	{
		public:
			void setData(const MyUtil::ObjectResultPtr& data, const ::Ice::Current& current);
			void setValid(bool flag, const ::Ice::Current& current);
			bool isValid();
		private:
			bool _isValid;
			IceUtil::Mutex _mutex;
	};
	
	class VirtualId2RelatedIdSetterI : public VirtualId2RelatedIdSetter, public MyUtil::Singleton<VirtualId2RelatedIdSetterI>
	{
		public:
			void setData(const MyUtil::ObjectResultPtr& data, const ::Ice::Current& current);
			void setValid(bool flag, const ::Ice::Current& current);
			bool isValid();
		private:
			bool _isValid;
			IceUtil::Mutex _mutex;
	};
	
	class IdFilter : public Ice::Object, public MyUtil::Singleton<IdFilter>
	{
		public:
			IdFilter();
			void setValid(bool flag);
			bool isValid();

			bool isFilterId(int id);
			bool isMaybeVirtualId(int id);

			void add(MyUtil::IntSeq ids);
		private:
			bool _isValid;
			IceUtil::Mutex _mutex;

			__gnu_cxx::hash_set<int> _idSet;
	};

	class IdFilterFillThread : public IceUtil::Thread, public MyUtil::Singleton<IdFilterFillThread>
	{
		public:
			void run();
	};

	class BatchUserIdHandler : public com::xiaonei::xce::ResultHandler
	{
		public:
			BatchUserIdHandler(MyUtil::IntSeq& ids, int& maxId);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			MyUtil::IntSeq& _ids;
			int& _maxId;
	};
	
	class FillTask: virtual public ::MyUtil::Task 
	{
		public:
			FillTask(const MyUtil::ObjectResultPtr& userData);
			virtual void handle();
		private:
			MyUtil::ObjectResultPtr _userData;
};

}

#endif
