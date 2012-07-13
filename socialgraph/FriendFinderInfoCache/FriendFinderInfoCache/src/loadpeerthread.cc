#include "socialgraph/FriendFinderInfoCache/FriendFinderInfoCache/src/loadpeerthread.h"

#include <boost/lexical_cast.hpp>

#include "util/cpp/MD5Util.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "base/logging.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

void LoadHighSchoolPeerThread::run() {
	using namespace com::xiaonei::xce;

	int last = 0;
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;

	Statement oss;
	oss << "SELECT * FROM high_school_info WHERE userid=" << uid_;
	try {
		QueryRunner("high_school_info_friendfinder", 
			CDbRServer).query(oss, HighSchoolInfoHandler(last, school_info_collector));
	} catch (Ice::Exception& e) {
		MCE_WARN("[LoadHighSchoolPeerThread] run Ice Exception " << e.what());
	} catch (...) {
		MCE_WARN("[LoadHighSchoolPeerThread] unknown Exception ");
	}
	//MCE_INFO("schoo_info_collector size : " << school_info_collector.size());
	if (school_info_collector.empty()) {
		return;
	}

	std::ostringstream logg;
	//for (std::map<int, SchoolInfo>::iterator iter = school_info_collector.begin();
	//		iter != school_info_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.userid_ << " " << iter->second.school_id_ << " " 
	//			<< iter->second.school_name_ << " " << iter->second.enroll_year_;
	//	MCE_INFO("SchoolInfo   " << logg.str());
	//}

	/*获取中学具体地里位置编码*/
	HighSchoolPeerHelper::GetSchoolMap("high_school", school_info_collector, school_code_collector);
	if (school_code_collector.empty()) {
		return;
	}
	//logg.str("");
	//for (std::map<int, SchoolCode>::iterator iter = school_code_collector.begin();
	//		iter != school_code_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.code_pro_ << " " << iter->second.code_city_
	//			<< " " << iter->second.code_county_;
	//	MCE_INFO("SchoolCode  " << logg.str());
	//}

	/*获取编码对应地理位置的名字*/
	HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);
	if (china_area_collector.empty()) {
		return;
	}
	//logg.str("");
	//for (std::map<std::string, std::string>::iterator iter = china_area_collector.begin();
	//		iter != china_area_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second;
	//	MCE_INFO("China area : " << logg.str());
	//}
	
	/*该用户资料*/
	HighSchoolPeerHelper::InserSchoolArea(school_info_collector, school_code_collector, china_area_collector, info_);
	logg.str("");
	for (MyUtil::StrSeq::iterator iter = info_.condition.begin();
			iter != info_.condition.end(); ++iter) {
		logg << *iter << ":";
	}
	MCE_INFO("highschool : " << logg.str());
	MCE_INFO("highschool md5 : " << info_.md5);
}

//-----------------------------------------------------------------------

void LoadJuniorHighSchoolPeerThread::run() {
	using namespace com::xiaonei::xce;
	int last = 0;
	
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;

	Statement oss;
	oss << "SELECT * FROM junior_high_school_info WHERE userid=" << uid_; 

	try {
		QueryRunner("junior_high_school_info_friendfinder", 
			CDbRServer).query(oss, JuniorHighSchoolInfoHandler(last, school_info_collector));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[LoadJuniorHighSchoolPeerThread] run Ice Exception " << e.what();
	} catch (...) {
		LOG(INFO) << "[LoadJuniorHighSchoolPeerThread] unknown Exception "; 
	}
	if (school_info_collector.empty()) {
		return;
	}

	std::ostringstream logg;
	//for (std::map<int, SchoolInfo>::iterator iter = school_info_collector.begin();
	//		iter != school_info_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.userid_ << " " << iter->second.school_id_ << " " 
	//			<< iter->second.school_name_ << " " << iter->second.enroll_year_;
	//	MCE_INFO("SchoolInfo   " << logg.str());
	//}

	/*获取初中具体地里位置编码*/
	HighSchoolPeerHelper::GetSchoolMap("junior_school", school_info_collector, school_code_collector);
	if (school_code_collector.empty()) {
		return;
	}

	//logg.str("");
	//for (std::map<int, SchoolCode>::iterator iter = school_code_collector.begin();
	//		iter != school_code_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.code_pro_ << " " << iter->second.code_city_
	//			<< " " << iter->second.code_county_;
	//	MCE_INFO("SchoolCode  " << logg.str());
	//}

	/*获取编码对应地理位置的名字*/
	HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);
	if (china_area_collector.empty()) {
		return;
	}
	//logg.str("");
	//for (std::map<std::string, std::string>::iterator iter = china_area_collector.begin();
	//		iter != china_area_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second;
	//	MCE_INFO("China area : " << logg.str());
	//}

	/*该用户资料*/
	HighSchoolPeerHelper::InserSchoolArea(school_info_collector, school_code_collector, china_area_collector, info_);
	logg.str("");
	for (MyUtil::StrSeq::iterator iter = info_.condition.begin();
			iter != info_.condition.end(); ++iter) {
		logg << *iter << ":";
	}
	MCE_INFO("juniorschool : " << logg.str());
	MCE_INFO("juniorschool md5 : " << info_.md5);
}

//-----------------------------------------------------------------------

void LoadSpecialSchoolPeerThread::run() {
	using namespace com::xiaonei::xce;
	int last = 0;
	
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;

	size_t college_info_query = 0;
	Statement oss;
	oss << "SELECT * FROM college_info WHERE userid=" << uid_; 

	try {
		college_info_query = QueryRunner("junior_high_school_info_friendfinder", 
				CDbRServer).query(oss, SpecialSchoolInfoHandler(last, school_info_collector));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[LoadSpecialSchoolPeerThread] run Ice Exception " << e.what();
	} catch (...) {
		LOG(INFO) << "[LoadSpecialSchoolPeerThread] unknown Exception "; 
	}
	if (school_info_collector.empty()) {
		return;
	}
	std::ostringstream logg;
	//for (std::map<int, SchoolInfo>::iterator iter = school_info_collector.begin();
	//		iter != school_info_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.userid_ << " " << iter->second.school_id_ << " " 
	//			<< iter->second.school_name_ << " " << iter->second.enroll_year_;
	//	MCE_INFO("SchoolInfo   " << logg.str());
	//}

	/*获取初中具体地里位置编码*/
	HighSchoolPeerHelper::GetSchoolMap("college", school_info_collector, school_code_collector);
	if (school_code_collector.empty()) {
		return;
	}
	//logg.str("");
	//for (std::map<int, SchoolCode>::iterator iter = school_code_collector.begin();
	//		iter != school_code_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second.code_pro_ << " " << iter->second.code_city_
	//			<< " " << iter->second.code_county_;
	//	MCE_INFO("SchoolCode  " << logg.str());
	//}

	/*获取编码对应地理位置的名字*/
	HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);
	if (china_area_collector.empty()) {
		return;
	}
	//logg.str("");
	//for (std::map<std::string, std::string>::iterator iter = china_area_collector.begin();
	//		iter != china_area_collector.end(); ++iter) {
	//	logg << iter->first << " : " << iter->second;
	//	MCE_INFO("China area : " << logg.str());
	//}

	/*该用户资料*/
	HighSchoolPeerHelper::InserSchoolArea(school_info_collector, school_code_collector, china_area_collector, info_);
	logg.str("");
	for (MyUtil::StrSeq::iterator iter = info_.condition.begin();
			iter != info_.condition.end(); ++iter) {
		logg << *iter << ":";
	}
	MCE_INFO("college : " << logg.str());
	MCE_INFO("college md5 : " << info_.md5);
}

void HighSchoolPeerHelper::GetSchoolMap(const std::string& source,
													 							const std::map<int, SchoolInfo>& school_info_collector,
													 							std::map<int, SchoolCode>& school_code_collector) 
{
	using namespace com::xiaonei::xce;
	Statement sql;
	sql << "SELECT id, code_pro, code_city, code_county FROM " << source << " WHERE id in (";

	size_t size = school_info_collector.size();
	size_t count = 0;
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_collector.begin();
			iter != school_info_collector.end(); ++iter) {
		sql << iter->second.school_id_;
		if (++count < size) {
			sql << ", ";
		}
	}
	sql << ")";

	try {
		QueryRunner("high_school_info_friendfinder", CDbRServer).query(sql, SchoolCodeHandler(school_code_collector));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[HighSchoolPeerHelper] GetSchoolMap run Ice Exception " << e.what();
	} catch (...) {
		LOG(INFO) << "[HighSchoolPeerHelper] GetSchoolMap unknown Exception "; 
	}
}

void HighSchoolPeerHelper::GetChinaAreaMap(const std::map<int, SchoolCode>& school_code_collector,
											 										 std::map<std::string, std::string>& china_area_collector)
{
	std::vector<std::string> index_list;
	for (std::map<int, SchoolCode>::const_iterator iter = school_code_collector.begin();
			iter != school_code_collector.end(); ++iter) {
		//if (BEIJING == iter->second.code_pro_ || //不是这么存的
		//		TIANJIN == iter->second.code_pro_ ||
		//		SHANGHAI == iter->second.code_pro_ ||
		//		CHONGQING == iter->second.code_pro_) 
		//{
		//	if (!iter->second.code_city_.empty()) {
		//		index_list.push_back(iter->second.code_city_);
		//	}	
		//} else {
		if (!iter->second.code_county_.empty()) {
			index_list.push_back(iter->second.code_county_);
		}
		//}
	}
	
	DbHelper::GetChinaArea(index_list, china_area_collector);
}

void HighSchoolPeerHelper::GetUserGenderMap(const std::map<int, SchoolInfo>& school_info_map,
															 							std::map<int, int>& id_gender)
{
	std::vector<int> uid_list;
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_map.begin();
			iter != school_info_map.end(); ++iter) {
		uid_list.push_back(iter->first);
	}

	DbHelper::GetUserGender(uid_list, id_gender);
}

void HighSchoolPeerHelper::InserSchoolArea(const std::map<int, SchoolInfo>& school_info_collector,
														  						 const std::map<int, SchoolCode>& school_code_collector,
														  						 const std::map<std::string, std::string>& china_area_collector,
														  						 Md5Info& info) 
{
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_collector.begin();
			iter != school_info_collector.end(); ++iter) {
		//查找区县code
		std::map<int, SchoolCode>::const_iterator schoolcodefit = school_code_collector.find(iter->second.school_id_);
		if (schoolcodefit != school_code_collector.end()) {
			std::string county = "";
			//if (BEIJING == schoolcodefit->second.code_pro_ ||  //不是这么存的
			//		TIANJIN == schoolcodefit->second.code_pro_ ||
			//		SHANGHAI == schoolcodefit->second.code_pro_ ||
			//		CHONGQING == schoolcodefit->second.code_pro_) 
			//{
			//	county = schoolcodefit->second.code_city_;
			//} else {
			county = schoolcodefit->second.code_county_;
			//}
			//查找code对应的名字
			std::map<std::string, std::string>::const_iterator chinaareafit = china_area_collector.find(county);
			if (chinaareafit != china_area_collector.end()) {
				std::string area_name = chinaareafit->second;
				std::ostringstream key;
				key << area_name << iter->second.enroll_year_;
				
				info.condition.push_back(area_name);
				info.condition.push_back(boost::lexical_cast<std::string>(iter->second.enroll_year_));
				info.md5 = MyUtil::MD5Util::stringToMD5(key.str());
			}
		}
	}
}

void HighSchoolPeerHelper::LoadProtoData(const std::map<long, std::list<People> >& school_peer_map) {
	using namespace xce::socialgraph;

	for (std::map<long, std::list<People> >::const_iterator iter = school_peer_map.begin();
			iter != school_peer_map.end(); ++iter) {
		std::list<People> plist = iter->second;
		if (plist.size() > MEMCACHE_VALUE_THRESHOLD) {
			plist.resize(MEMCACHE_VALUE_THRESHOLD);
		}
		Items items;
		for (std::list<People>::iterator it = plist.begin();
				it != plist.end(); ++it) {
			Item* item = items.add_items();
			item->set_id(it->uid_);
			item->set_field(it->gender_);
		}

		std::vector<unsigned char> binary_data;
		int byte_size = items.ByteSize();
		binary_data.resize(byte_size);
		items.SerializeToArray(binary_data.data(), byte_size);

		bool insert = SetDistcache(iter->first, binary_data);
		if (insert) {
			std::vector<unsigned char> tmp_data;
			GetDistcache(iter->first, tmp_data);
			Items tmp;
			tmp.ParseFromArray(tmp_data.data(), tmp_data.size());
			LOG(INFO) << "insert success tmp:" << tmp.items_size() << " binary_data " << iter->first 
								<< " " << binary_data.size() << " items size:" << items.items_size();
		} else {
			LOG(INFO) << "insert fail binary_data " << iter->first << " " << binary_data.size() 
								<< " items size:" << items.items_size();
		}
	}
}

bool HighSchoolPeerHelper::SetDistcache(long key, const std::vector<unsigned char>& data) {
	int tmp_id = (int)key;
	std::ostringstream oss;	
	oss << "OFSA_" << tmp_id;
	LOG(INFO) << "osskey : " << oss.str() << "  tmp_id:" << tmp_id << "  key:" << key;
	return xce::MemcachedClient::GetInstance().SetByDistCache2(oss.str(), data);
}

bool HighSchoolPeerHelper::GetDistcache(long key, std::vector<unsigned char>& data) {
	int tmp_id = (int)key;
	std::ostringstream oss;	
	oss << "OFSA_" << tmp_id;
	LOG(INFO) << "osskey : " << oss.str() << "  tmp_id:" << tmp_id << "  key:" << key;
	return xce::MemcachedClient::GetInstance().GetByDistCache2(oss.str(), data);
}

}
}
