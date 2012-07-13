#ifndef ID_CACHE_DATA_I_H
#define ID_CACHE_DATA_I_H

#include <ServiceI.h>
#include <QueryRunner.h>
#include <IdCache.h>

namespace idcache
{
	const std::string TABLE_VIRTUAL_ID = "page_admin";
	const std::string TABLE_USER_PASSPORT = "user_passport";
	const std::string TABLE_MULTI_IDS = "multi_ids_map";
	
	const std::string DB_VIRTUAL_ID = "page";
	const std::string DB_USER_PASSPORT = "user_passport";
	const std::string DB_MULTI_IDS = "user_passport";

	const std::string DB_VIRTUAL_ID_PRELOADER = "page_preloader";
	const std::string DB_USER_PASSPORT_PRELOADER = "user_passport_preloader";
	const std::string DB_MULTI_IDS_PRELOADER = "user_passport_preloader";

	class VirtualIdInfoI;
	typedef IceUtil::Handle<VirtualIdInfoI> VirtualIdInfoIPtr;
	class VirtualIdInfoI : public VirtualIdInfo
	{
		public:
			VirtualIdInfoIPtr getData();
			int getVirtualId();
			int getUserId();
			int getTransId();
			void parse(mysqlpp::Row& row);
			void parse(const Ice::ObjectPtr& data);
		private:
			IceUtil::Mutex _mutex;
	};
	
	class UserId2RelatedIdInfoI;
	typedef IceUtil::Handle<UserId2RelatedIdInfoI> UserId2RelatedIdInfoIPtr;
	class UserId2RelatedIdInfoI : public UserId2RelatedIdInfo
	{
		public:
			UserId2RelatedIdInfoI();
			UserId2RelatedIdInfoIPtr getData();
			void parse(const Ice::ObjectPtr& data);
			void setDoubleAccountId(int id);
			void addVirtualIdInfo(VirtualIdInfoIPtr vi);
			void deleteVirtualId(int id);
		private:
			IceUtil::Mutex _mutex;
	};

	class UserId2RelatedIdInfoFactoryI : public MyUtil::ServantFactory 
	{
		public:
			virtual Ice::ObjectPtr create(Ice::Int id);
			static UserId2RelatedIdInfoIPtr load(int id);
	};
	
	class VirtualIdInfoFactoryI : public MyUtil::ServantFactory 
	{
		public:
			virtual Ice::ObjectPtr create(Ice::Int id);
			static VirtualIdInfoIPtr load(int id);
	};

	class UserId2RelatedIdInfoResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			UserId2RelatedIdInfoResultHandlerI(const UserId2RelatedIdInfoIPtr& result);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			UserId2RelatedIdInfoIPtr m_result;
	};
	
	class VirtualIdInfoResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			VirtualIdInfoResultHandlerI(VirtualIdInfoIPtr& result);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			VirtualIdInfoIPtr m_result;
	};
	
	class BatchVirtualIdResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			BatchVirtualIdResultHandlerI(VirtualIdInfoSeq& result);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			VirtualIdInfoSeq& m_result;
	};
	
	class UserIdResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			UserIdResultHandlerI(const int id, int& doubleAccountId);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			int m_id;
			int& m_doubleAccountId;
	};
	
}

#endif
