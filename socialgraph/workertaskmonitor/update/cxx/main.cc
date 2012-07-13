#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "socialgraph/socialgraphutil/zkclient.h"
#include "TaskManagerAPI.h"
#include "SocialGraphWorker.h"
#include "IceExt/src/Channel.h"
#include "IceExt/src/AdapterI.h"

const std::string SG_ZNODE = "/SOCIALGRAPHLOGIC";
const std::string g_token_1 = ":";
const std::string g_token_2 = "_";

//----------------------------------------------------------------------------------------

struct WorkerTaskData {
WorkerTaskData() {
	worker = "";
	level = 1;
	size_list.resize(24);
}

std::string worker;
int level;
std::vector<int> size_list;
}; 

struct Data {
	Data() {
		time = "";	
		size = 0;
	}

	std::string time;
	int size;
};

//----------------------------------------------------------------------------------------

struct Item {
	Item() {
		level_ = 0;
		size_ = 0;	
	}

	int level_;
	int size_;
};

struct Row {
	Row() {
		time_ = "";
	}
	
	std::string time_;
	std::vector<Item> item_list_;
};

//----------------------------------------------------------------------------------------

void GetEndpoint(std::map<std::string, int>& endpoint_map);
void PrintRelativeLevel(const std::map<std::string, int>& endpoint_map);
void CreateStatsByHour(const std::map<std::string, int>& endpoint_map, std::map<std::string, std::vector<int> >& endpoint_level);
void CreateTotalStats(const std::map<std::string, int>& endpoint_map);
void CreateTotalStatsByHour(const std::map<std::string, int>& endpoint_map, const std::map<std::string, std::vector<int> >& endpoint_level);

int main(int argc, char* argv[]) {
	std::map<std::string, int> endpoint_map;
	
	//std::cout << "worker task monitor" << std::endl;
	GetEndpoint(endpoint_map);

	for (std::map<std::string, int>::iterator iter = endpoint_map.begin();
			iter != endpoint_map.end(); ++iter) {
		std::cout << iter->first << " : " << iter->second << std::endl;
	}

	//PrintRelativeLevel(endpoint_map);
	
	std::map<std::string, std::vector<int> > endpoint_level;	
	CreateStatsByHour(endpoint_map, endpoint_level);

	//CreateTotalStats(endpoint_map);
	
	CreateTotalStatsByHour(endpoint_map, endpoint_level);
}

void GetEndpoint(std::map<std::string, int>& endpoint_map) {
	std::string head = "Controller";
	std::string value;
	int limit = 5;
	while (!xce::ZooConfig::GetInstance().Get(SG_ZNODE, &value)) {
		if (--limit <= 0) {
			break;
		}
	}
	std::cout << value << std::endl;
	std::cout << "Controller value:" << value << " endpoint_map:" << endpoint_map.size() << std::endl;

	std::vector<std::string> raw_info;
	boost::algorithm::split(raw_info, value, boost::algorithm::is_any_of(g_token_1));
	
	for (std::vector<std::string>::iterator iter = raw_info.begin();
			iter != raw_info.end(); ++iter) {
		std::vector<std::string> info;
		boost::algorithm::split(info, *iter, boost::algorithm::is_any_of(g_token_2));

		if (2 == (int)info.size()) {
			MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx> adapter(info[1], 120, 300);
			int cluster = adapter.getCluster();
			size_t pos = info[1].find(head);
			if (std::string::npos != pos) {
				std::string sub = info[1].substr(pos + head.length());
				endpoint_map[sub] = cluster; 
			}
		}
	}
}

void PrintRelativeLevel(const std::map<std::string, int>& endpoint_map) {
	MyUtil::SocialGraphChannel channel = MyUtil::SocialGraphChannel();
	xce::svcapi::TaskManagerAPIPrx tma = NULL;

	for (std::map<std::string, int>::const_iterator iter = endpoint_map.begin();
			iter != endpoint_map.end(); ++iter) {
		for (int server = 0; server < iter->second; ++server) {
			ostringstream oss;
			oss << "TMA@" << iter->first << server;
			std::cout << oss.str() << std::endl;
			try {
				Ice::ObjectPrx prx = channel.getCommunicator()->stringToProxy(oss.str());
				tma = xce::svcapi::TaskManagerAPIPrx::checkedCast(prx);
				
				for (int level = 0; level < 10; ++level) {
					std::cout << tma->size(level) << std::endl;
				}
				std::cout << std::endl;
			} catch (Ice::Exception& e) {
				std::cout << oss.str() << " Ice Exception : " << e.what() << std::endl;
			} catch (std::exception& e) {
				std::cout << oss.str() << " Std Exception : " << e.what() << std::endl;
			} catch (...) {
				std::cout << oss.str() << " Unknown Exception : " << std::endl;
			}
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void CreateStatsByHour(const std::map<std::string, int>& endpoint_map, std::map<std::string, std::vector<int> >& endpoint_level) {
	std::cout << "CreateStatsByHour endpoint_map size:" << endpoint_map.size() << " endpoint_level:" << endpoint_level.size() << std::endl;
	time_t t = time(0);
	//t = t - 3600;     	//一小时前
	struct tm *Tm = localtime(&t);
	char str[40];
	
	sprintf(str, "%04d-%02d-%02d-%02d",
			Tm->tm_year + 1900,
			Tm->tm_mon + 1,
			Tm->tm_mday,
			Tm->tm_hour);
	std::ostringstream path;
	path << "/data/xce/Project/workermoniter/tmp/" << std::string(str);
	//std::cout << path.str() << std::endl;

	ofstream outfile(path.str().c_str(), ofstream::trunc);

	MyUtil::SocialGraphChannel channel = MyUtil::SocialGraphChannel();
	xce::svcapi::TaskManagerAPIPrx tma = NULL;

	for (std::map<std::string, int>::const_iterator iter = endpoint_map.begin();
			iter != endpoint_map.end(); ++iter) {
		for (int server = 0; server < iter->second; ++server) {
			ostringstream oss;
			oss << "TMA@" << iter->first << server;

			ostringstream ss;
			ss << iter->first << server;
			try {
				Ice::ObjectPrx prx = channel.getCommunicator()->stringToProxy(oss.str());
				tma = xce::svcapi::TaskManagerAPIPrx::checkedCast(prx);
				MyUtil::Int2IntMap level_map = tma->getLevel2Size();

				for (MyUtil::Int2IntMap::iterator ii = level_map.begin();
						ii != level_map.end(); ++ii) {
					if (0 > ii->first) {
						continue;
					}
					outfile << ss.str() << "\t" << ii->first << "\t" << ii->second << "\n";
					endpoint_level[ss.str()].push_back(ii->first);
				}
				
			} catch (Ice::Exception& e) {
				std::cout << oss.str() << " Ice Exception : " << e.what() << std::endl;
			} catch (std::exception& e) {
				std::cout << oss.str() << " Std Exception : " << e.what() << std::endl;
			} catch (...) {
				std::cout << oss.str() << " Unknown Exception : " << std::endl;
			}
		}
	}
}

void CreateTotalStats(const std::map<std::string, int>& endpoint_map) {
	time_t t = time(0);
	t = t - 3600;     	//一小时前
	struct tm *Tm = localtime(&t);

	std::vector<std::string> file_vec;
	std::vector<std::string> tt;
	for (int i = 0; i <= Tm->tm_hour; ++i) {
		char str[40];
		
		sprintf(str, "%04d-%02d-%02d-%02d",
				Tm->tm_year + 1900,
				Tm->tm_mon + 1,
				Tm->tm_mday,
				i);
//		std::cout << std::string(str) << std::endl;
		//std::ostringstream oss;
		//oss << "/data/xce/Project/workermoniter/tmp/" << std::string(str);
		file_vec.push_back(std::string(str));
		//file_vec.push_back(oss.str());
		char rr[10];
		sprintf(rr, "%02d", i);
		tt.push_back(std::string(rr));
	}

	std::map<std::pair<std::string, int>, std::vector<Data> > total_data;
	std::vector<Data> info;
	info.resize(24);

	for (std::map<std::string, int>::const_iterator iter = endpoint_map.begin();
			iter != endpoint_map.end(); ++iter) {
		for (int cluster = 0; cluster < iter->second; ++cluster) {
			std::ostringstream oss;
			oss << iter->first << cluster;

			for (int level = 0; level < 10; ++level) {
				total_data[make_pair<std::string, int>(oss.str(), level)] = info;
			}
		}
	}

//	//for (std::map<std::pair<std::string, int>, std::vector<Data> >::iterator iter = total_data.begin();
//	//		iter != total_data.end(); ++iter) {
//	//	std::cout << iter->first.first << " : " << iter->first.second << " : " << iter->second.size() << std::endl;
//	//}
//
//	std::cout << "2 :" << std::endl;
//
	std::string line = "";
	for (int i = 0; i < (int)file_vec.size() ; ++i) {
		std::ostringstream oss;
		oss << "/data/xce/Project/workermoniter/tmp/" << file_vec.at(i);
		//ifstream infile(file_vec.at(i).c_str(), ifstream::in);
		ifstream infile(oss.str().c_str(), ifstream::in);
		std::string endpoint;
		int level;
		int size;
		
		while (getline(infile, line)) {
			istringstream stream(line);
			stream >> endpoint >> level >> size;
			Data data;
			data.time = file_vec.at(i);
			data.size = size;
			total_data[make_pair<std::string, int>(endpoint, level)].at(i) = data;
		}
	}

//	for (std::map<std::pair<std::string, int>, std::vector<Data> >::iterator iter = total_data.begin();
//			iter != total_data.end(); ++iter) {
//		std::cout << iter->first.first << " : " << iter->first.second << " : " << iter->second.size() << std::endl;
//		for (std::vector<Data>::iterator it = iter->second.begin();
//				it != iter->second.end(); ++it) {
//			std::cout << it->time << ":" << it->size << " ";	
//		}
//		std::cout << std::endl;
//	}
//	std::cout << "3 :" << std::endl;

	t = time(0);
	Tm = localtime(&t);
	char tmp[20];

	sprintf(tmp, "%04d-%02d-%02d",
			Tm->tm_year + 1900,
			Tm->tm_mon + 1,
			Tm->tm_mday);

	std::string out_path = "/data/xce/Project/workermoniter/tasksize_";
	out_path += std::string(tmp);
	out_path += ".html";
	ofstream outfile(out_path.c_str(), ofstream::trunc);
	
	std::string name = "";
	//outfile << "<html>" << "\n";

	outfile << "<body>";
	for (std::map<std::pair<std::string, int>, std::vector<Data> >::iterator iter = total_data.begin();
			iter != total_data.end(); ++iter) {
		if (iter->first.first != name) {
			if ("" != name) {
				outfile << "</table>" << "\n";
			}
			//outfile << "\n";
			name = iter->first.first;
			//outfile << "<table border=\"1\">\n";
			outfile << "<table bordercolor=\"#000033\" bgcolor=\"#99ccff\" border=\"1\">\n";
			outfile << "<tr><td>" << iter->first.first << "</td></tr>\n";
			outfile << "<tr><td>level" << "</td>";
			for (std::vector<std::string>::iterator it = tt.begin();
					it != tt.end(); ++it) {
				outfile << "<td>" << *it << "</td>";
			}
			outfile << "</tr>";
			outfile << "\n";
		}

		outfile << "<tr><td>" << iter->first.second << "</td>";
		for (int j = 0; j < (int)file_vec.size(); ++j) {
			outfile << "<td>" << iter->second.at(j).size << "</td>";
		}
		outfile << "</tr>";
		outfile << "\n";
	}
	outfile << "</table>" << "\n";

	outfile << "</body>" << "\n";
}

void CreateTotalStatsByHour(const std::map<std::string, int>& endpoint_map, const std::map<std::string, std::vector<int> >& endpoint_level) {
	time_t t = time(0);
	//t = t - 3600;     	//一小时前
	struct tm *Tm = localtime(&t);

	std::vector<std::string> file_vec;
	std::vector<std::string> tt;
	for (int i = 0; i <= Tm->tm_hour; ++i) {
		char str[40];
		
		sprintf(str, "%04d-%02d-%02d-%02d",
				Tm->tm_year + 1900,
				Tm->tm_mon + 1,
				Tm->tm_mday,
				i);

		file_vec.push_back(std::string(str));
		char rr[20];
		sprintf(rr, "%02d:00-%02d:59", i, i);
		tt.push_back(std::string(rr));
	}

	std::map<std::string, std::vector<Row> > total_data;
	for (std::map<std::string, int>::const_iterator iter = endpoint_map.begin();
			iter != endpoint_map.end(); ++iter) {
		for (int cluster = 0; cluster < iter->second; ++cluster) {
			std::ostringstream oss;	
			oss << iter->first << cluster;

			std::vector<Row> row_list;
			for (int i = 0; i < (int)file_vec.size(); ++i) {
				Row r;
				//r.item_list_.resize(size);
				row_list.push_back(r);
			}
			total_data[oss.str()] = row_list;
		}
	}

	std::string line = "";
	for (int i = 0; i < (int)file_vec.size() ; ++i) {
		std::ostringstream oss;
		oss << "/data/xce/Project/workermoniter/tmp/" << file_vec.at(i);
		ifstream infile(oss.str().c_str(), ifstream::in);
		std::string endpoint;
		int level;
		int size;
		
		while (getline(infile, line)) {
			istringstream stream(line);
			stream >> endpoint >> level >> size;

			std::map<std::string, std::vector<int> >::const_iterator fit = endpoint_level.find(endpoint);
			if (fit == endpoint_level.end()) {
				continue;
			}
			if (total_data[endpoint].at(i).item_list_.size() >= fit->second.size()) {
				continue;
			}

			total_data[endpoint].at(i).time_ = tt.at(i);;
			Item item;
			item.level_ = level;
			item.size_ = size;
			total_data[endpoint].at(i).item_list_.push_back(item);
		}
	}

	t = time(0);
	Tm = localtime(&t);
	char tmp[20];

	sprintf(tmp, "%04d-%02d-%02d",
			Tm->tm_year + 1900,
			Tm->tm_mon + 1,
			Tm->tm_mday);

	std::string out_path = "/data/xce/Project/workermoniter/tasksize_";
	out_path += std::string(tmp);
	out_path += ".html";
	ofstream outfile(out_path.c_str(), ofstream::trunc);

	outfile << "<body>" << "\n";
	for (std::map<std::string, std::vector<Row> >::iterator iter = total_data.begin();
			iter != total_data.end(); ++iter) {
		outfile << "<h4>" << iter->first << "</h4>" << "\n";
		outfile << "<table bordercolor=\"#000033\" bgcolor=\"#99ccff\" border=\"1\">" << "\n";
		outfile << "<tr><td>Time</td>";
		
		std::map<std::string, std::vector<int> >::const_iterator fit = endpoint_level.find(iter->first);
		if (fit == endpoint_level.end()) {
			continue;
		}
		for (std::vector<int>::const_iterator ii = fit->second.begin();
				ii != fit->second.end(); ++ii) {
			outfile << "<td>" << "Level_" << *ii << "</td>";
		}
		outfile << "</tr>\n";
		for (std::vector<Row>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			outfile << "<tr><td>" << it->time_<< "</td>";
			//std::cout << it->time_ << "	"; 
			for (std::vector<Item>::iterator ii = it->item_list_.begin();
					ii != it->item_list_.end(); ++ii) {
				//std::cout << "(" << ii->level_ << ":" << ii->size_ << ")	";
				outfile << "<td>" << ii->size_ << "</td>";
			}
			//std::cout << std::endl;
			outfile << "</tr>\n";
		}
		//std::cout << std::endl;
		//std::cout << std::endl;
		outfile << "</table>" << "\n";
		outfile << "<br></br>" << "\n";
	}

	outfile << "</body>" << "\n";
}
