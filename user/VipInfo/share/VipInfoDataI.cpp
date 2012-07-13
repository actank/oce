#include "VipInfoDataI.h"
#include "Date.h"

namespace xce
{
namespace vipinfo
{

//VipMemberDataI

void VipMemberMemDataI::setProps(const MyUtil::Str2StrMap& props)
{
	MyUtil::Str2StrMap::const_iterator findIt = props.find("TYPE");
	if(findIt == props.end())
	{
		return;
	}
	
	int type = boost::lexical_cast<int>(findIt->second);
	
	IceUtil::Mutex::Lock lock(_mutex);
	VipMemberDataNMap::const_iterator findData = data.find(type);
	if(findData == data.end())
	{
		VipMemberDataNPtr newData = load(type);
		if(newData == NULL)
		{
			MCE_WARN("VipMemberDataNI::load Exception no type " << boost::lexical_cast<std::string>(type) << "for id " << boost::lexical_cast<std::string>(id));
			return;
		}
		std::pair<VipMemberDataNMap::iterator, bool> insertPair = data.insert(make_pair(type, newData));
		if(insertPair.second == false)
		{
			MCE_WARN("VipMemberDataNI::load insert Exception for type " << boost::lexical_cast<std::string>(type) << "of id " << boost::lexical_cast<std::string>(id));
			return;
		}
		findData = insertPair.first;
	}

	for (MyUtil::Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it)
	{
		if(it->first == "STATUS")
		{
			findData->second->status = boost::lexical_cast<int>(it->second);
		}else if (it->first == "LEVEL")
		{
			findData->second->level = boost::lexical_cast<int>(it->second);
		}else if (it->first == "ICON_ID")
		{
			findData->second->iconId = boost::lexical_cast<int>(it->second);
		}else if (it->first == "HAT_ID")
		{
			findData->second->hatId = boost::lexical_cast<int>(it->second);
		}else if (it->first == "INT_INFO1")
		{
			findData->second->intInfo1 = boost::lexical_cast<int>(it->second);
		}else if (it->first == "INT_INFO2")
		{
			findData->second->intInfo2 = boost::lexical_cast<int>(it->second);
		}else if (it->first == "CHAR_INFO1")
		{
			findData->second->charInfo1 = it->second;
		}else if (it->first == "CHAR_INFO2")
		{
			findData->second->charInfo2 = it->second;
		}
	}
}

VipMemberDataNPtr VipMemberMemDataI::load(int type)
{
	VipMemberMemDataIPtr result = new VipMemberMemDataI;
	result->id = id;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_VIP_MEMBER << " WHERE uid = '" << id << "' and type = '" << type <<"'";
	VipMemberMemDataResultHandlerI handler(result);
	com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
	if (result->data.empty()) {
		MCE_WARN("VipMemberMemDataI::load, Exception NoSuchVipMemberData, id = " << id);
		return NULL;
	}
	return result->data.at(0);
}

void VipMemberMemDataI::parse(mysqlpp::Row& row)
{
	VipMemberDataNPtr newData = new VipMemberDataN;
	newData->id = (int) row["uid"];
	newData->type = (int) row["type"];
	newData->status = (int) row["status"];
	newData->level = (int) row["level"];
	newData->iconId = (int) row["icon_id"];
	newData->hatId = (int) row["hat_id"];
	newData->intInfo1 = (int) row["int_info1"];
	newData->intInfo2 = (int) row["int_info2"];
	newData->charInfo1 = (std::string) row["char_info1"];
	newData->charInfo2 = (std::string) row["char_info2"];
	IceUtil::Mutex::Lock lock(_mutex);
	data.insert(make_pair(newData->type, newData));
}

void VipMemberMemDataI::parse(Ice::ObjectPtr& obj)
{
	if(obj->ice_isA("::xce::vipinfo::VipMemberMemDataN")){
		VipMemberMemDataNPtr newData = VipMemberMemDataNPtr::dynamicCast(obj);
		this->id = newData->id;
		this->data = newData->data;
	}else if(obj->ice_isA("::xce::vipinfo::VipMemberMemData")){
		VipMemberMemDataPtr newData = VipMemberMemDataPtr::dynamicCast(obj);
		this->id = newData->id;
		for(VipMemberDataMap::const_iterator it = newData->data.begin(); it != newData->data.end(); ++it){
			VipMemberDataNPtr item = new VipMemberDataN;
			item->id = it->second->id;
			item->type = it->second->type;
			item->status = it->second->status;
			item->level = it->second->level;
			item->iconId = it->second->iconId;
			item->hatId = 0; //数据库默认值
			item->intInfo1 = it->second->intInfo1;
			item->intInfo2 = it->second->intInfo2;
			item->charInfo1 = it->second->charInfo1;
			item->charInfo2 = it->second->charInfo2;
			this->data.insert(make_pair(it->first, item));
		}			
	}
}

VipMemberMemDataIPtr VipMemberMemDataI::getData()
{
	IceUtil::Mutex::Lock lock(_mutex);
	VipMemberMemDataIPtr newData = new VipMemberMemDataI;
	newData->id = id;
	for(VipMemberDataNMap::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		VipMemberDataNPtr data = new VipMemberDataN;
		data->id = it->second->id;
		data->type = it->second->type;
		data->status = it->second->status;
		data->level = it->second->level;
		data->iconId = it->second->iconId;
		data->hatId = it->second->hatId;
		data->intInfo1 = it->second->intInfo1;
		data->intInfo2 = it->second->intInfo2;
		data->charInfo1 = it->second->charInfo1;
		data->charInfo2 = it->second->charInfo2;
		newData->data.insert(make_pair(it->first, data));
	}
	return newData;
}

VipMemberDataSeq VipMemberMemDataI::getVipMemberDataSeq(const std::vector<int>& types)
{
	IceUtil::Mutex::Lock lock(_mutex);
	VipMemberDataSeq dataSeq;
	for(std::vector<int>::const_iterator it = types.begin(); it != types.end(); ++it)
	{
		VipMemberDataNMap::const_iterator findIt = data.find(*it);
		if(findIt != data.end())
		{
			VipMemberDataPtr data = new VipMemberData;
			data->id = findIt->second->id;
			data->type = findIt->second->type;
			data->status = findIt->second->status;
			data->level = findIt->second->level;
			data->iconId = findIt->second->iconId;
			data->intInfo1 = findIt->second->intInfo1;
			data->intInfo2 = findIt->second->intInfo2;
			data->charInfo1 = findIt->second->charInfo1;
			data->charInfo2 = findIt->second->charInfo2;
			dataSeq.push_back(data);
		}
	}
	return dataSeq;
}

VipMemberDataNSeq VipMemberMemDataI::getVipMemberDataNSeq(const std::vector<int>& types)
{
	IceUtil::Mutex::Lock lock(_mutex);
	VipMemberDataNSeq dataSeq;
	for(std::vector<int>::const_iterator it = types.begin(); it != types.end(); ++it)
	{
		VipMemberDataNMap::const_iterator findIt = data.find(*it);
		if(findIt != data.end())
		{
			VipMemberDataNPtr data = new VipMemberDataN;
			data->id = findIt->second->id;
			data->type = findIt->second->type;
			data->status = findIt->second->status;
			data->level = findIt->second->level;
			data->iconId = findIt->second->iconId;
			data->hatId = findIt->second->hatId;
			data->intInfo1 = findIt->second->intInfo1;
			data->intInfo2 = findIt->second->intInfo2;
			data->charInfo1 = findIt->second->charInfo1;
			data->charInfo2 = findIt->second->charInfo2;
			dataSeq.push_back(data);
		}
	}
	return dataSeq;
}

//VipFullData

VipFullDataI::VipFullDataI(const VipMemberDataPtr& memberData, const std::string& hideUrl)
{
	this->id = memberData->id;
	this->memberData = memberData;
	this->hideUrl = hideUrl;
}

//VipFullDataN

VipFullDataNI::VipFullDataNI(const VipMemberDataNPtr& memberData, const std::string& iconUrl, const std::string& hatUrl, const int hatStatus)
{
	this->id = memberData->id;
	this->memberData = memberData;
	this->iconUrl = iconUrl;
	this->hatUrl = hatUrl;
	this->hatStatus = hatStatus;
}

//VipIconDataI

VipIconDataI::VipIconDataI(const VipMemberDataPtr& memberData, const std::string& hideUrl)
{
	id = memberData->id;
	iconId = memberData->iconId;
	this->hideUrl = hideUrl;
	level = memberData->level;
	status = memberData->status;
}

//VipFeedDataI

VipFeedDataI::VipFeedDataI(const VipMemberDataNPtr& memberData, const Icon2UrlDataPtr& iconData, const HatDataPtr& hatData)
{
	member = new VipMemberMetaDataI;
	VipMemberMetaDataIPtr::dynamicCast(member)->parse(memberData);
	icon = Icon2UrlDataIPtr::dynamicCast(iconData)->getData();
	hat = HatDataIPtr::dynamicCast(hatData)->getData();
}

//Icon2UrlDataI

void Icon2UrlDataI::setProps(const MyUtil::Str2StrMap& props)
{
	IceUtil::Mutex::Lock lock(_mutex);
	for (MyUtil::Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it)
	{
		if (it->first == "URL")
		{
			hideUrl= it->second;
		}
	}
}

void Icon2UrlDataI::parse(Ice::ObjectPtr& obj)
{
	Icon2UrlDataPtr data = Icon2UrlDataPtr::dynamicCast(obj);
	iconId = data->iconId;
	hideUrl = data->hideUrl;
}

void Icon2UrlDataI::parse(mysqlpp::Row& row)
{ 
	iconId = (int) row["id"];
	hideUrl = (std::string) row["url"];
}

Icon2UrlDataIPtr Icon2UrlDataI::getData()
{
	IceUtil::Mutex::Lock lock(_mutex);
	Icon2UrlDataIPtr newData = new Icon2UrlDataI;
	newData->iconId = iconId;
	newData->hideUrl = hideUrl;
	return newData;
}

//HatDataI

void HatDataI::setProps(const MyUtil::Str2StrMap& props)
{
	IceUtil::Mutex::Lock lock(_mutex);
	for (MyUtil::Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it)
	{
		if (it->first == "URL")
		{
			hideUrl= it->second;
		}else if (it->first == "STATUS")
		{
			status= boost::lexical_cast<int>(it->second);
		}
	}
}

void HatDataI::parse(Ice::ObjectPtr& obj)
{
	HatDataPtr data = HatDataPtr::dynamicCast(obj);
	hatId = data->hatId;
	status = data->status;
	hideUrl = data->hideUrl;
}

void HatDataI::parse(mysqlpp::Row& row)
{
	hatId = (int) row["id"];
	status = (int) row["status"];
	hideUrl = (std::string) row["url"];
}

HatDataIPtr HatDataI::getData()
{
	IceUtil::Mutex::Lock lock(_mutex);
	HatDataIPtr newData = new HatDataI;
	newData->hatId = hatId;
	newData->status = status;
	newData->hideUrl = hideUrl;
	return newData;
}

//VipMemberMetaData

void VipMemberMetaDataI::parse(const VipMemberDataNPtr& data)
{
	id = data->id;
	type = data->type;
	level = data->level;
	status = data->status;
}

//VipMemberMemDataFactoryI 

Ice::ObjectPtr VipMemberMemDataFactoryI::create(Ice::Int id) {
	VipMemberMemDataIPtr result = new VipMemberMemDataI;
	result->id = id;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_VIP_MEMBER << " WHERE uid = '" << id << "'";
	VipMemberMemDataResultHandlerI handler(result);
	size_t count = com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
	if (count ==0 | result->data.empty()) {
		MCE_WARN("VipMemberMemDataFactoryI::create, Exception NoSuchVipMemberData, id = " << id);
		return NULL;
	}
	return result;
}

//Icon2UrlDataFactoryI

Ice::ObjectPtr Icon2UrlDataFactoryI::create(Ice::Int id) {
	Icon2UrlDataIPtr result = new Icon2UrlDataI;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_ICON_URL << " WHERE id = '" << id << "'";
	Icon2UrlDataResultHandlerI handler(result);
	size_t count = com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
	if (count == 0 | id != result->iconId) {
		MCE_WARN("Icon2UrlDataFactoryI::create, Exception NoSuchIcon2UrlData, id = " << id);
		return NULL;
	}
	return result;
}

//HatDataFactoryI

Ice::ObjectPtr HatDataFactoryI::create(Ice::Int id) {
	HatDataIPtr result = new HatDataI;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_HAT << " WHERE id = '" << id << "'";
	HatDataResultHandlerI handler(result);
	size_t count = com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
	if (count == 0 | id != result->hatId) {
		MCE_WARN("HatDataFactoryI::create, Exception NoSuchHatData, id = " << id);
		return NULL;
	}
	return result;
}

//VipMemberMemDataResultHandlerI

VipMemberMemDataResultHandlerI::VipMemberMemDataResultHandlerI(const VipMemberMemDataIPtr& result) :
	_result(result)
{
}

bool VipMemberMemDataResultHandlerI::handle(mysqlpp::Row& row) const
{
	_result->parse(row);
	return true;
}

//Icon2UrlDataResultHandlerI

Icon2UrlDataResultHandlerI::Icon2UrlDataResultHandlerI(const Icon2UrlDataIPtr& result) :
	_result(result)
{
}

bool Icon2UrlDataResultHandlerI::handle(mysqlpp::Row& row) const
{
	_result->parse(row);
	return true;
}

//HatDataResultHandlerI

HatDataResultHandlerI::HatDataResultHandlerI(const HatDataIPtr& result) :
	_result(result)
{
}

bool HatDataResultHandlerI::handle(mysqlpp::Row& row) const
{
	_result->parse(row);
	return true;
}

//BatchVipMemberDataResultHandlerI

BatchVipMemberMemDataResultHandlerI::BatchVipMemberMemDataResultHandlerI(const MyUtil::ObjectResultPtr& result) :
	_result(result)
{
}

bool BatchVipMemberMemDataResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["uid"];

	MyUtil::ObjectMap::const_iterator findIt = _result->data.find(id);

	if(findIt == _result->data.end())
	{
		VipMemberMemDataIPtr data = new VipMemberMemDataI;
		data->parse(row);
		_result->data.insert(make_pair(id, data));
	}else
	{
		VipMemberMemDataIPtr::dynamicCast(findIt->second)->parse(row);
	}
	return true;
}

//BatchIcon2UrlDataResultHandlerI

BatchIcon2UrlDataResultHandlerI::BatchIcon2UrlDataResultHandlerI(const MyUtil::ObjectResultPtr& result) :
	_result(result)
{
}

bool BatchIcon2UrlDataResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];

	Icon2UrlDataIPtr data = new Icon2UrlDataI;
	data->parse(row);

	_result->data.insert(make_pair(id, data));
	return true;
}

//BatchHatDataResultHandlerI

BatchHatDataResultHandlerI::BatchHatDataResultHandlerI(const MyUtil::ObjectResultPtr& result) :
	_result(result)
{
}

bool BatchHatDataResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];

	HatDataIPtr data = new HatDataI;
	data->parse(row);

	_result->data.insert(make_pair(id, data));
	return true;
}

}
}
