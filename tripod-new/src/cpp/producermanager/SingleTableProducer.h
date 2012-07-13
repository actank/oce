#ifndef __PRODUCERMANAGER_SINGLETABLEPRODUCER_H__
#define __PRODUCERMANAGER_SINGLETABLEPRODUCER_H__

#include "TripodProducer.h"
#include <QueryRunner.h>
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


namespace com {
namespace renren {
namespace tripod {

class ProtobufReflection
{
public:
	void init(const std::string& type_name_);
  void serialize(const mysqlpp::Row& row, string& data);

private:
	google::protobuf::Message* createMessage();
	void fill(google::protobuf::Message* message, const google::protobuf::FieldDescriptor* descriptor,
			 const google::protobuf::Reflection* reflection, const mysqlpp::Row& row);

	std::string type_name_;
	const google::protobuf::Descriptor* descriptor_;

};


class SingleTableProducer : public TripodProducer {

public :
		 SingleTableProducer(const std::string& desc,const std::string& table,
					const std::string& fields, const std::string proto_type_name):
						desc_(desc),table_(table),fields_(fields),proto_type_name_(proto_type_name){
				reflection_.init(proto_type_name);
		 };
	   virtual ~SingleTableProducer() {}
     virtual DataMap create(const KeySeq& keys, bool isMaster);
     virtual KeySeq createHotKeys(const std::string& beginKey, int limit);
     virtual KeySeq createKeys(const std::string& beginKey, int limit);

private:
		std::string desc_;
		std::string table_;
		std::string fields_;
		std::string proto_type_name_;

		ProtobufReflection reflection_;
};

class CreateKeysResultHandler : virtual public com::xiaonei::xce::ResultHandler {

public:
	CreateKeysResultHandler(std::vector<std::string>& keys, const std::string& primaryKey) 
				: keys_(keys), primaryKey_(primaryKey) {}
protected: 
	virtual bool handle(mysqlpp::Row& row) const {
		keys_.push_back((std::string)row[primaryKey_.c_str()]);
		return true;
	} 
private:
	std::vector<std::string>& keys_;
	std::string primaryKey_;
};

class CreateValuesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
  CreateValuesResultHandler(DataMap& result,const std::string& primaryKey,ProtobufReflection& reflection)
			 : result_(result),primaryKey_(primaryKey),reflection_(reflection){}
protected: 
	virtual bool handle(mysqlpp::Row& row) const ;
private:
  DataMap& result_;
  std::string primaryKey_;
	ProtobufReflection& reflection_;
};



}
}
}

#endif
