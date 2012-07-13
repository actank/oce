#include "socialgraph/OfferFriends/sameareapeer/loadpeerthread.h"

#include <boost/algorithm/string.hpp>
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
	LOG(INFO) << "[LoadHighSchoolPeerThread] run";
	using namespace com::xiaonei::xce;

	int last = 0;
	int limit = 10000;
	
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;
	std::map<int, int> id_gender_collector;

	do {
		size_t high_school_info_query = 0;
		Statement oss;
		oss << "SELECT * FROM high_school_info WHERE id > " << last << " LIMIT " << limit;

		try {
			high_school_info_query = QueryRunner("high_school_info_friendfinder", 
				CDbRServer).query(oss, HighSchoolInfoHandler(last, school_info_collector));
		} catch (Ice::Exception& e) {
			LOG(INFO) << "[LoadHighSchoolPeerThread] run Ice Exception " << e.what();
		} catch (...) {
			LOG(INFO) << "[LoadHighSchoolPeerThread] unknown Exception "; 
		}

		if (0 == high_school_info_query) {
			LOG(INFO) << "LoadHighSchoolPeerThread at end of table";
			break;
		}
		if (school_info_collector.empty()) {
			continue;
		}

		/*过滤用户的stage*/
		HighSchoolPeerHelper::FliterUserStage(school_info_collector);

		/*获取中学具体地里位置编码*/
		HighSchoolPeerHelper::GetSchoolMap("high_school", school_info_collector, school_code_collector);
		if (school_code_collector.empty()) {
			continue;
		}

		/*获取编码对应地理位置的名字*/
		HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);

		/*用户id对应的性别信息*/
		HighSchoolPeerHelper::GetUserGenderMap(school_info_collector, id_gender_collector);

		/*存储信息*/
		HighSchoolPeerHelper::InserSchoolPeerMap(school_info_collector, school_code_collector, 
																						china_area_collector, id_gender_collector, highschool_peer_map_);

		LOG(INFO) << "HigSchool " << last << " : " << school_info_collector.size() << " : " << school_code_collector.size() << " : " 
			<< china_area_collector.size() << " : " << id_gender_collector.size() << " : " << highschool_peer_map_.size();

		/*清空容器*/
		school_info_collector.clear();
		school_code_collector.clear();
		china_area_collector.clear();
		id_gender_collector.clear();
	} while (true);

	HighSchoolPeerHelper::LoadProtoData(highschool_peer_map_);
}

//-----------------------------------------------------------------------

void LoadJuniorHighSchoolPeerThread::run() {
	using namespace com::xiaonei::xce;
	LOG(INFO) << "[LoadJuniorHighSchoolPeerThread] run";

	int last = 0;
	int limit = 10000;
	
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;
	std::map<int, int> id_gender_collector;

	do {
		size_t junior_high_school_info_query = 0;
		Statement oss;
		oss << "SELECT * FROM junior_high_school_info WHERE id > " << last << " LIMIT " << limit;

		try {
			junior_high_school_info_query = QueryRunner("junior_high_school_info_friendfinder", 
				CDbRServer).query(oss, JuniorHighSchoolInfoHandler(last, school_info_collector));
		} catch (Ice::Exception& e) {
			LOG(INFO) << "[LoadJuniorHighSchoolPeerThread] run Ice Exception " << e.what();
		} catch (...) {
			LOG(INFO) << "[LoadJuniorHighSchoolPeerThread] unknown Exception "; 
		}

		if (0 == junior_high_school_info_query) {
			LOG(INFO) << "LoadJuniorHighSchoolPeerThread at end of table";
			break;
		}
		if (school_info_collector.empty()) {
			continue;
		}

		/*过滤用户的stage*/
		HighSchoolPeerHelper::FliterUserStage(school_info_collector);

		/*获取初中具体地里位置编码*/
		HighSchoolPeerHelper::GetSchoolMap("junior_school", school_info_collector, school_code_collector);
		if (school_code_collector.empty()) {
			continue;
		}

		/*获取编码对应地理位置的名字*/
		HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);
		if (china_area_collector.empty()) {
			continue;
		}

		/*用户id对应的性别信息*/
		HighSchoolPeerHelper::GetUserGenderMap(school_info_collector, id_gender_collector); 

		/*存储信息*/
		HighSchoolPeerHelper::InserSchoolPeerMap(school_info_collector, school_code_collector, 
																						china_area_collector, id_gender_collector, juniorschool_peer_map_);

		LOG(INFO) << "JuniorSchool " << last << " : " << school_info_collector.size() << " : " << school_code_collector.size() 
			<< " : " << china_area_collector.size() << " : " << id_gender_collector.size() << " : " << juniorschool_peer_map_.size();

		/*清空容器*/
		school_info_collector.clear();
		school_code_collector.clear();
		china_area_collector.clear();
		id_gender_collector.clear();
	} while (true);
	HighSchoolPeerHelper::LoadProtoData(juniorschool_peer_map_);
}

//-----------------------------------------------------------------------

void LoadSpecialSchoolPeerThread::run() {
	using namespace com::xiaonei::xce;
	LOG(INFO) << "[LoadSpecialSchoolPeerThread] run";

	int last = 0;
	int limit = 10000;
	
	std::map<int, SchoolInfo> school_info_collector;
	std::map<int, SchoolCode> school_code_collector;
	std::map<std::string, std::string> china_area_collector;
	std::map<int, int> id_gender_collector;

	do {
		size_t college_info_query = 0;
		Statement oss;
		oss << "SELECT * FROM college_info WHERE id > " << last << " LIMIT " << limit;

		try {
			college_info_query = QueryRunner("junior_high_school_info_friendfinder", 
					CDbRServer).query(oss, SpecialSchoolInfoHandler(last, school_info_collector));
		} catch (Ice::Exception& e) {
			LOG(INFO) << "[LoadSpecialSchoolPeerThread] run Ice Exception " << e.what();
		} catch (...) {
			LOG(INFO) << "[LoadSpecialSchoolPeerThread] unknown Exception "; 
		}

		if (0 == college_info_query) {
			LOG(INFO) << "LoadSpecialSchoolPeerThread at end of table";
			break;
		}
		if (school_info_collector.empty()) {
			continue;
		}

		/*过滤用户的stage*/
		HighSchoolPeerHelper::FliterUserStage(school_info_collector);

		/*获取初中具体地里位置编码*/
		HighSchoolPeerHelper::GetSchoolMap("college", school_info_collector, school_code_collector);
		if (school_code_collector.empty()) {
			continue;
		}

		/*获取编码对应地理位置的名字*/
		HighSchoolPeerHelper::GetChinaAreaMap(school_code_collector, china_area_collector);
		if (china_area_collector.empty()) {
			continue;
		}

		/*用户id对应的性别信息*/
		HighSchoolPeerHelper::GetUserGenderMap(school_info_collector, id_gender_collector); 

		/*存储信息*/
		HighSchoolPeerHelper::InserSchoolPeerMap(school_info_collector, school_code_collector,
																						china_area_collector, id_gender_collector, college_peer_map_);

		LOG(INFO) << "college " << last << " : " << school_info_collector.size() << " : " << school_code_collector.size() 
			<< " : " << china_area_collector.size() << " : " << id_gender_collector.size() << " : " << college_peer_map_.size();

		/*清空容器*/
		school_info_collector.clear();
		school_code_collector.clear();
		china_area_collector.clear();
		id_gender_collector.clear();
	} while (true);
	HighSchoolPeerHelper::LoadProtoData(college_peer_map_);
}

void HighSchoolPeerHelper::GetSchoolMap(const std::string& source,
													 							const std::map<int, SchoolInfo>& school_info_collector,
													 							std::map<int, SchoolCode>& school_code_collector) 
{
	using namespace com::xiaonei::xce;
	if (school_info_collector.empty()) {
		return;
	}

	Statement sql;
	std::ostringstream oss;
	oss << "SELECT id, code_pro, code_city, code_county FROM " << source << " WHERE id in (";

	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_collector.begin();
			iter != school_info_collector.end(); ++iter) {
		if (0 != iter->second.school_id_) {
			oss << iter->second.school_id_ << ", ";
		}
	}
	std::string tmp = oss.str();
	if (boost::ends_with(tmp, ", ")) {
		boost::erase_last(tmp, ", ");
	} else if (boost::ends_with(tmp, "(")) {
		return;
	}

	tmp += ")";
	sql << tmp;

	try {
		QueryRunner("high_school_info_friendfinder", CDbRServer).query(sql, SchoolCodeHandler(school_code_collector));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[HighSchoolPeerHelper] GetSchoolMap run Ice Exception " << e.what() << " " << tmp;
	} catch (...) {
		LOG(INFO) << "[HighSchoolPeerHelper] GetSchoolMap unknown Exception " << tmp; 
	}
}

void HighSchoolPeerHelper::GetChinaAreaMap(const std::map<int, SchoolCode>& school_code_collector,
											 										 std::map<std::string, std::string>& china_area_collector)
{
	if (school_code_collector.empty()) {
		return;
	}
	std::vector<std::string> index_list;
	for (std::map<int, SchoolCode>::const_iterator iter = school_code_collector.begin();
			iter != school_code_collector.end(); ++iter) {
		if (!iter->second.code_county_.empty()) {
			try {
				boost::lexical_cast<long>(iter->second.code_county_);
			} catch (boost::bad_lexical_cast& e) {
				continue;	
			} catch (...) {
				continue;
			}
			index_list.push_back(iter->second.code_county_);
		}
	}
	
	DbHelper::GetChinaArea(index_list, china_area_collector);
}

void HighSchoolPeerHelper::GetUserGenderMap(const std::map<int, SchoolInfo>& school_info_map,
															 							std::map<int, int>& id_gender)
{
	if (school_info_map.empty()) {
		return;
	}
	std::vector<int> uid_list;
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_map.begin();
			iter != school_info_map.end(); ++iter) {
		uid_list.push_back(iter->first);
	}

	DbHelper::GetUserGender(uid_list, id_gender);
}

void HighSchoolPeerHelper::InserSchoolPeerMap(const std::map<int, SchoolInfo>& school_info_collector,
																							const std::map<int, SchoolCode>& school_code_collector,
																							const std::map<std::string, std::string>& china_area_collector,
																							const std::map<int, int>& id_gender_collector,
																							std::map<long, std::list<People> >& school_peer_map) 
{
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_collector.begin();
			iter != school_info_collector.end(); ++iter) {
		//查找区县code
		std::map<int, SchoolCode>::const_iterator schoolcodefit = school_code_collector.find(iter->second.school_id_);
		if (schoolcodefit != school_code_collector.end()) {
			std::string county = "";
			//if (BEIJING == schoolcodefit->second.code_pro_ ||  //不是这么存的，日日日
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
				//查找uid对应的性别
				std::map<int, int>::const_iterator genderfit = id_gender_collector.find(iter->first);
				if (genderfit != id_gender_collector.end()) {
					People pe;
					pe.uid_ = iter->first;
					pe.gender_ = genderfit->second;

					std::ostringstream key;
					key << area_name << iter->second.enroll_year_;
					long md5key = MyUtil::MD5Util::stringToMD5(key.str());
					school_peer_map[md5key].push_back(pe);
					//LOG(INFO) << iter->first << ": " << area_name << ": " << iter->second.enroll_year_ 
					//					<< ": " << key.str() << ": " << md5key;
				} else {
					People pe;
					pe.uid_ = iter->first;
					pe.gender_ = 0;

					std::ostringstream key;
					key << area_name << iter->second.enroll_year_;
					long md5key = MyUtil::MD5Util::stringToMD5(key.str());
					school_peer_map[md5key].push_back(pe);
					//LOG(INFO) << iter->first << ": " << area_name << ": " << iter->second.enroll_year_ 
					//					<< ": " << key.str() << ": " << md5key;
				}
			}
		}
	}
}

void HighSchoolPeerHelper::LoadProtoData(const std::map<long, std::list<People> >& school_peer_map) {
	LOG(INFO) << "[HighSchoolPeerHelper] LoadProtoData school_peer_map size:" << school_peer_map.size();
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
	std::ostringstream oss;	
	oss << "OFSA_" << key;
	return xce::MemcachedClient::GetInstance().SetByDistCache2(oss.str(), data);
}

bool HighSchoolPeerHelper::GetDistcache(long key, std::vector<unsigned char>& data) {
	std::ostringstream oss;	
	oss << "OFSA_" << key;
	return xce::MemcachedClient::GetInstance().GetByDistCache2(oss.str(), data);
}

void HighSchoolPeerHelper::FliterUserStage(std::map<int, SchoolInfo>& school_info_map) {
	if (school_info_map.empty()) {
		return;
	}
	std::vector<int> uid_list;
	for (std::map<int, SchoolInfo>::const_iterator iter = school_info_map.begin();
			iter != school_info_map.end(); ++iter) {
		uid_list.push_back(iter->first);
	}
	//LOG(INFO) << "uid_list size : " << uid_list.size();
	std::map<int, int> user_stage;
	DbHelper::GetUserStage(uid_list, user_stage);
	//xce::PRINT_KEY2VALUE(user_stage);

	for (std::map<int, int>::iterator iter = user_stage.begin();
			iter != user_stage.end(); ++iter) {
		if (10 != iter->second) {
			std::map<int, SchoolInfo>::iterator fit = school_info_map.find(iter->first);
			if (fit != school_info_map.end()) {
				school_info_map.erase(fit);
			}
		}
	}
}

}
}
