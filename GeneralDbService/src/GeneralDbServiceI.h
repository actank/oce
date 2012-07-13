#ifndef GENERALDBSERVICE_H_
#define GENERALDBSERVICE_H_

#include <GeneralDbService.h>
#include <ObjectCacheI.h>
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>

namespace xce{
namespace generaldbservice{

const int CATE_OBJECT = 0;


struct DbDesc {
	std::vector<std::string> columns;
	std::string table_name;    
	int table_cluster;         
	std::string id_column;     
	std::string db_name;       
	std::string pattern_prefix;
};

DbDesc DBDESC;

class GeneralDbServiceManagerI:virtual public GeneralDbServiceManager, public MyUtil::Singleton<GeneralDbServiceManagerI>{
public:
	GeneralDbServiceManagerI();
	virtual ~GeneralDbServiceManagerI();
	virtual void set(int id,const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual MyUtil::Str2StrMap get(int id, const Ice::Current& = Ice::Current());
	virtual void reload(int id, const Ice::Current& = Ice::Current());
	virtual void create(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
};

class GeneralDbServiceFactoryI:public MyUtil::ServantFactory{
public:
	GeneralDbServiceFactoryI();
	virtual ~GeneralDbServiceFactoryI();
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class GeneralDbServiceData:public Ice::Object {
public:
	void setProperties(const std::vector<std::string>& data);
	std::vector<string> getProperties();
private:
	std::vector<std::string> _data;
	IceUtil::RWRecMutex _dataMutex;
};

typedef IceUtil::Handle<GeneralDbServiceData> GeneralDbServiceDataPtr;

class GeneralDbServiceDataResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	GeneralDbServiceDataResultHandlerI(const GeneralDbServiceDataPtr& data);
	virtual ~GeneralDbServiceDataResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
private:
	GeneralDbServiceDataPtr _data;
};

class GeneralDbServiceFieldsResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	GeneralDbServiceFieldsResultHandlerI();
	~GeneralDbServiceFieldsResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

}
}
#endif
