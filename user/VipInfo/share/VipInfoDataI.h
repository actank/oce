#ifndef __VIPINFO_DATA_I_H__
#define __VIPINFO_DATA_I_H__

#include "VipInfo.h"
#include <Ice/Ice.h>
#include <TaskManager.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>

namespace xce{
namespace vipinfo{

//DB sources

const std::string DB_MEMBERS = "vipmember";

//DB tables

const std::string TABLE_VIP_MEMBER = "user_member_info";
const std::string TABLE_ICON_URL = "icon_url";
const std::string TABLE_HAT = "feed_icon_url";

//Data classes

class VipMemberMemDataI;
typedef IceUtil::Handle<VipMemberMemDataI> VipMemberMemDataIPtr;
class VipMemberMemDataI: virtual public VipMemberMemDataN{
	public:
		void setProps(const MyUtil::Str2StrMap& props);
		void parse(mysqlpp::Row& row);
		void parse(Ice::ObjectPtr& obj);
		VipMemberMemDataIPtr getData();
		VipMemberDataSeq getVipMemberDataSeq(const std::vector<int>& types);
		VipMemberDataNSeq getVipMemberDataNSeq(const std::vector<int>& types);

	private:
		VipMemberDataNPtr load(int type);
	private:
		IceUtil::Mutex _mutex;
};

class VipFullDataI: virtual public VipFullData{
public:
	VipFullDataI(const VipMemberDataPtr& memberData, const std::string& hideUrl);
};
typedef IceUtil::Handle<VipFullDataI> VipFullDataIPtr;

class VipFullDataNI: virtual public VipFullDataN{
public:
	VipFullDataNI(const VipMemberDataNPtr& memberData, const std::string& iconUrl, const std::string& hatUrl, const int hatStatus);
};
typedef IceUtil::Handle<VipFullDataNI> VipFullDataNIPtr;

class VipIconDataI: virtual public VipIconData{
public:
	VipIconDataI(const VipMemberDataPtr& memberData, const std::string& hideUrl);
};
typedef IceUtil::Handle<VipIconDataI> VipIconDataIPtr;

class VipFeedDataI: virtual public VipFeedData{
public:
	VipFeedDataI(const VipMemberDataNPtr& memberData, const Icon2UrlDataPtr& iconData, const HatDataPtr& hatData);
};
typedef IceUtil::Handle<VipFeedDataI> VipFeedDataIPtr;

class Icon2UrlDataI;
typedef IceUtil::Handle<Icon2UrlDataI> Icon2UrlDataIPtr;
class Icon2UrlDataI: virtual public Icon2UrlData{
public:
	void setProps(const MyUtil::Str2StrMap& props);
	Icon2UrlDataIPtr getData();
	void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
private:
	IceUtil::Mutex _mutex;
};

class HatDataI;
typedef IceUtil::Handle<HatDataI> HatDataIPtr;
class HatDataI: virtual public HatData{
public:
	void setProps(const MyUtil::Str2StrMap& props);
	HatDataIPtr getData();
	void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
private:
	IceUtil::Mutex _mutex;
};

class VipMemberMetaDataI;
typedef IceUtil::Handle<VipMemberMetaDataI> VipMemberMetaDataIPtr;
class VipMemberMetaDataI: virtual public VipMemberMetaData{
public:
	void parse(const VipMemberDataNPtr& data);
};

//Factories

class VipMemberMemDataFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class Icon2UrlDataFactoryI : public MyUtil::ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class HatDataFactoryI : public MyUtil::ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

//ResultHandlers

class VipMemberMemDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	VipMemberMemDataResultHandlerI(const VipMemberMemDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	VipMemberMemDataIPtr _result;	
};

class Icon2UrlDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	Icon2UrlDataResultHandlerI(const Icon2UrlDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	Icon2UrlDataIPtr _result;	
};

class HatDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	HatDataResultHandlerI(const HatDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	HatDataIPtr _result;
};

class BatchVipMemberMemDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchVipMemberMemDataResultHandlerI(const MyUtil::ObjectResultPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	MyUtil::ObjectResultPtr _result;	
};

class BatchIcon2UrlDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchIcon2UrlDataResultHandlerI(const MyUtil::ObjectResultPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	MyUtil::ObjectResultPtr _result;	
};

class BatchHatDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchHatDataResultHandlerI(const MyUtil::ObjectResultPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	MyUtil::ObjectResultPtr _result;
};

}
}

#endif
