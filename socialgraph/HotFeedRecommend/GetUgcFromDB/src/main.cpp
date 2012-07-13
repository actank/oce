#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "UgcDBhelper.h"
#include "StreamObject.h"

using namespace std;
using namespace xce::dbhelper;
using namespace MyUtil;
using namespace item;
using namespace com::xiaonei::xce;

void GetPhoto(int day, int interval, std::vector<item::UGCItem>& ugcList);
void GetBlog(int day, int interval, std::vector<item::UGCItem>& ugcList);
void WriteFile(const std::string& fileName, std::vector<item::UGCItem>& ugcList);
void WriteOwnerUgcFile(const std::string& fileName, std::vector<item::UGCItem>& ugcList);

int main(int argc, char* argv[]) {
	ConnectionPoolManager::instance().initialize();

	vector<UGCItem> photoList;
	GetPhoto(15, 3600, photoList);
	cout << "photoList size : " << photoList.size() << endl;

	vector<UGCItem> blogList;
	GetBlog(15, 3600, blogList);
	cout << "blogList size : " << blogList.size() << endl;

	string fileName = "/data/xce/Project/hotfeedrecommend/data/owner_list";
	WriteFile(fileName, photoList);
	WriteFile(fileName, blogList);

	string owner_ugc_file = "/data/xce/Project/hotfeedrecommend/data/owner_ugc_list";
	WriteOwnerUgcFile(owner_ugc_file, photoList);
	WriteOwnerUgcFile(owner_ugc_file, blogList);
}

void GetPhoto(int day, int interval, std::vector<item::UGCItem>& ugcList) {
	for (int index = 0; index < 100; ++index) {
		string tableName = TableName::PhotoTableName(index);		
		string pattern = TableName::PhotoTablePattern(index);

		cout << "photo tableName : " << tableName << endl;
		cout << "photo pattern : " << pattern << endl;

		for (int n = day; n > 0; --n) {
    		time_t t = time(0);
    		t = t - n * 24 * 3600;     //回退1天0点
    		struct tm *Tm = localtime(&t);
    		time_t ct = t - Tm->tm_hour * 3600 - Tm->tm_min * 60 - Tm->tm_sec;

		    for (int i = 3600; i <= 24 * 3600; i = i + interval) {       //以1小时为间隔
    	    	char startTime[30];
    	    	struct tm *AscTm_1 = localtime(&ct);
    	    	sprintf(startTime, "%04d-%02d-%02d %02d:%02d:%02d",
    	    	        AscTm_1->tm_year+1900,
    	    	        AscTm_1->tm_mon+1,
    	    	        AscTm_1->tm_mday,
    	    	        AscTm_1->tm_hour,
    	    	        AscTm_1->tm_min,
    	    	        AscTm_1->tm_sec);
				//cout << "startTime : " << string(startTime) << endl;

    	    	char endTime[30];
    	    	ct += interval;
    	    	struct tm *AscTm_2 = localtime(&ct);
    	    	sprintf(endTime, "%04d-%02d-%02d %02d:%02d:%02d",
    	    	        AscTm_2->tm_year+1900,
    	    	        AscTm_2->tm_mon+1,
    	    	        AscTm_2->tm_mday,
    	    	        AscTm_2->tm_hour,
    	    	        AscTm_2->tm_min,
    	    	        AscTm_2->tm_sec);
				//cout << "endTime : " << string(endTime) << endl;

				Statement sql;
				sql
						<< "SELECT * FROM " << tableName << " WHERE time BETWEEN " 
						<< mysqlpp::quote << startTime << " AND " << mysqlpp::quote << endTime;

				try {
						QueryRunner("adminsearch", CDbRServer, pattern).query(sql, PhotoDataReadHelper(ugcList));
				} catch (Ice::Exception& e) {
						cout << "[HotFeedUpdateDBHelper] GetPhoto Connection Exception: " << e.what() << endl;
				} catch (...) {
						cout << "[HotFeedUpdateDBHelper] GetPhoto DB Unknow Exception" << endl;
				}
			}
		}
	}
}

void GetBlog(int day, int interval, std::vector<item::UGCItem>& ugcList) {
	for (int index = 0; index < 100; ++index) {
		string tableName = TableName::BlogTableName(index);		
		string pattern = TableName::BlogTablePattern(index);

		cout << "blog tableName : " << tableName << endl;
		cout << "blog pattern : " << pattern << endl;

		for (int n = day; n > 0; --n) {
    		time_t t = time(0);
    		t = t - n * 24 * 3600;     //回退1天0点
    		struct tm *Tm = localtime(&t);
    		time_t ct = t - Tm->tm_hour * 3600 - Tm->tm_min * 60 - Tm->tm_sec;

		    for (int i = 3600; i <= 24 * 3600; i = i + interval) {       //以1小时为间隔
    	    	char startTime[30];
    	    	struct tm *AscTm_1 = localtime(&ct);
    	    	sprintf(startTime, "%04d-%02d-%02d %02d:%02d:%02d",
    	    	        AscTm_1->tm_year+1900,
    	    	        AscTm_1->tm_mon+1,
    	    	        AscTm_1->tm_mday,
    	    	        AscTm_1->tm_hour,
    	    	        AscTm_1->tm_min,
    	    	        AscTm_1->tm_sec);
				//cout << "startTime : " << string(startTime) << endl;

    	    	char endTime[30];
    	    	ct += interval;
    	    	struct tm *AscTm_2 = localtime(&ct);
    	    	sprintf(endTime, "%04d-%02d-%02d %02d:%02d:%02d",
    	    	        AscTm_2->tm_year+1900,
    	    	        AscTm_2->tm_mon+1,
    	    	        AscTm_2->tm_mday,
    	    	        AscTm_2->tm_hour,
    	    	        AscTm_2->tm_min,
    	    	        AscTm_2->tm_sec);
				//cout << "endTime : " << string(endTime) << endl;

				Statement sql;
				sql
						<< "SELECT * FROM " << tableName << " WHERE time BETWEEN " 
						<< mysqlpp::quote << startTime << " AND " << mysqlpp::quote << endTime;

				try {
						QueryRunner("adminsearch", CDbRServer, pattern).query(sql, BlogDataReadHelper(ugcList));
				} catch (Ice::Exception& e) {
						cout << "[HotFeedUpdateDBHelper] GetBlog Connection Exception: " << e.what() << endl;
				} catch (...) {
						cout << "[HotFeedUpdateDBHelper] GetBlog DB Unknow Exception" << endl;
				}
			}
		}
	}
}

void WriteFile(const std::string& fileName, std::vector<item::UGCItem>& ugcList) {
	ofstream outfile(fileName.c_str(), ofstream::app); 		
	StreamObjectPtr ptr = new StreamObject(outfile);
	
	map<long, int> result_map;
	for (vector<UGCItem>::const_iterator iter = ugcList.begin();
			iter != ugcList.end(); ++iter) {
		int id = iter->getOwnerId();
		long ugc_id = iter->getUgcId();
		result_map[ugc_id] = id;
	}

	for (map<long, int>::iterator iter = result_map.begin();
			iter != result_map.end(); ++iter) {
		ostringstream oss;
		oss << iter->second << "\t" << iter->first << "\t" << 1 << "\n";

		ptr->WriteString(oss.str());
	}
}

void WriteOwnerUgcFile(const std::string& fileName, std::vector<item::UGCItem>& ugcList) {
	ofstream outfile(fileName.c_str(), ofstream::app);
	StreamObjectPtr ptr = new StreamObject(outfile);
	
	map<long, UGCItem> result_map;
	for (vector<UGCItem>::const_iterator iter = ugcList.begin();
			iter != ugcList.end(); ++iter) {
		map<long, UGCItem>::iterator fit = result_map.find(iter->getUgcId()); 

		if (fit != result_map.end()) {
			fit->second = (fit->second.getUGCWeight() > iter->getUGCWeight() ? fit->second : *iter);
			continue;
		}

		pair<map<long, UGCItem>::iterator, bool> res =
			result_map.insert(make_pair<long, UGCItem>(iter->getUgcId(), *iter));
	}

	for (map<long, UGCItem>::iterator iter = result_map.begin();
			iter != result_map.end(); ++iter) {
		ostringstream oss;
		oss << iter->second.getOwnerId() << "\t" << iter->second.getUgcId() << "\t" << iter->second.getCommentCount()
				<< "\t" << iter->second.getViewCount() << "\t" << iter->second.getUGCWeight() << "\t" 
				<< iter->second.getUGCType() << "\n";
		ptr->WriteString(oss.str());
	}
}
