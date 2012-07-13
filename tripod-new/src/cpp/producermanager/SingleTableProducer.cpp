#include "SingleTableProducer.h"

namespace com {
namespace renren {
namespace tripod {

using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::CDbRServer;
	
using google::protobuf::Message;
using google::protobuf::FieldDescriptor;
using google::protobuf::MessageFactory;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::Reflection;

const std::string TABLE_USER_PASSPORT = "user_passport";

com::renren::tripod::KeySeq SingleTableProducer::createKeys(const std::string& beginKey, int limit) {
  MCE_DEBUG("[SingleTableProducer::createKeys] begin beginKey:" << beginKey << " limit:" << limit);
  std::vector<std::string> userKeys;
  Statement sql;
  sql << "select id from user_passport where id >= " << mysqlpp::quote << beginKey << " limit " << limit;
  CreateKeysResultHandler handler(userKeys, "id");
  QueryRunner("user_passport", CDbRServer).query(sql, handler);
  return userKeys;
}

KeySeq SingleTableProducer::createHotKeys(const std::string& beginKey, int limit) {
	return createKeys(beginKey,limit);
}

DataMap SingleTableProducer::create(const KeySeq& keys, bool isMaster) {
  MCE_DEBUG("[SingleTableProducer::create] begin keys.size:" << keys.size());
  DataMap result;

  if (keys.empty()) {
    MCE_WARN("[SingleTripodProducer::create] keys is empty");
    return result;
  }

  std::ostringstream oss;
  for (std::vector<std::string>::const_iterator iter = keys.begin(); iter != keys.end(); ++iter) {
    if (iter != keys.begin()) {
      oss << ",";
    }
    oss << *iter;
  }

  Statement sql;
  sql << "select " << fields_ << " from " << table_ << " where id in (" << oss.str() << ")";
	CreateValuesResultHandler handler(result,"id",reflection_);
	if(isMaster){
	  QueryRunner(desc_, CDbWServer).query(sql, handler);
	}else{
	  QueryRunner(desc_, CDbRServer).query(sql, handler);
	}

}
void ProtobufReflection::init(const std::string& type_name) {
	type_name_ = type_name;
  descriptor_ =
            DescriptorPool::generated_pool()->FindMessageTypeByName(type_name_);
}

Message* ProtobufReflection::createMessage()
{
  Message* message = NULL;
  const Descriptor* descriptor =
    DescriptorPool::generated_pool()->FindMessageTypeByName(type_name_);
  if (descriptor)
  {
    const Message* prototype =
      MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype)
    {
      message = prototype->New();
    }
  }
  return message;
}

void ProtobufReflection::serialize(const mysqlpp::Row& row, std::string& data){
  Message* message = createMessage();
  const Reflection* reflection = message->GetReflection();
	for(int i=0; i<descriptor_->field_count(); ++i){
      fill(message,descriptor_->field(i),reflection,row);
  }
  message->SerializeToString(&data);
}

void ProtobufReflection::fill(Message* message, const FieldDescriptor* descriptor, 
				const Reflection* reflection, const mysqlpp::Row& row){
  
  assert(descriptor != NULL);
  //current not support for repeated label
  if(descriptor->label() == FieldDescriptor::LABEL_REPEATED){
    return; 
  }

  const char* name = descriptor->name().c_str();

  switch(descriptor->type()){
    case FieldDescriptor::TYPE_FIXED64 : 
    case FieldDescriptor::TYPE_INT64 : 
      reflection->SetInt64(message, descriptor, row[name]);break;
    case FieldDescriptor::TYPE_UINT64 : 
      reflection->SetInt64(message, descriptor, row[name]);break;

    case FieldDescriptor::TYPE_FIXED32: 
    case FieldDescriptor::TYPE_INT32 :
      reflection->SetInt32(message, descriptor, row[name]);break;
    case FieldDescriptor::TYPE_UINT32 :
      reflection->SetInt32(message, descriptor, row[name]);break;

    case FieldDescriptor::TYPE_STRING :
      reflection->SetString(message, descriptor, (std::string)row[name]);break;

    case FieldDescriptor::TYPE_DOUBLE :
      reflection->SetDouble(message, descriptor, row[name]);break;
    case FieldDescriptor::TYPE_FLOAT :
      reflection->SetFloat(message, descriptor, row[name]);break;
    case FieldDescriptor::TYPE_BOOL :
      reflection->SetBool(message, descriptor, row[name]);break;
  }   
}

bool CreateValuesResultHandler::handle(mysqlpp::Row& row) const {
	std::string key = (std::string)row[primaryKey_.c_str()];	
	std::string data;
	reflection_.serialize(row,data);
	result_[key] = Data(data.begin(),data.end());
}

}
}
}

