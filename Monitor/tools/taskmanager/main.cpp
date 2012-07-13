/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  monitor the taskmanager
 *
 *        Version:  1.0
 *        Created:  2011年09月09日 11时04分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "Date.h"
#include <IceUtil/IceUtil.h>
#include "Channel.h"
#include "TaskManagerAPI.h"
#include "util/cpp/String.h"

#include <MonitorAdapter.h>

using namespace std;
using namespace MyUtil;
using namespace xce::svcapi;

class ChannelUtil: public IceUtil::Shared {
public:
	ChannelUtil() {};
	~ChannelUtil() {};

	void initialize() {
		channels_["Footprint"] = new FootprintChannel;
		channels_["Xiaonei"] = new OceChannel;
		channels_["Buddy"] = new BuddyChannel;
		channels_["User"] = new UserChannel;
	}

	ChannelPtr getChannel(std::string cluster) {
		return channels_[cluster];
	}
private:
	std::map<std::string, ChannelPtr> channels_;
};
typedef IceUtil::Handle<ChannelUtil> ChannelUtilPtr;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "TaskManagerMonitor config.file" << std::endl;
		exit(1);
	}

	ifstream ifile(argv[1]);
	if (!ifile) {
		std::cerr << "error " << std::endl;
		exit(1);
	}

	ChannelUtilPtr channel_util = new ChannelUtil();
	channel_util->initialize();

	std::vector<std::string> config;

	char str[100];

	while(ifile.getline(str,sizeof(str))) {
		config.push_back(string(str));
	}

	while (true) {
		for (std::vector<std::string>::iterator it = config.begin();
				it != config.end(); ++it) {

			std::vector<std::string> split_config = MyUtil::StrUtil::split((*it), " ");
	//		std::cout << "service: " << split_config[0] << " cluster: " << split_config[1] << " max: " << split_config[2]  << " scope: "  << split_config[3] << std::endl;

			std::string service = split_config[0];
			std::string cluster = split_config[1];

			ChannelPtr channel = channel_util->getChannel(cluster);

			ostringstream oss;
			oss << "TMA@" << service;

			TaskManagerAPIPrx tm;
			if (channel) {
				Ice::ObjectPrx objprx = channel->getCommunicator()->stringToProxy(oss.str());
				tm = TaskManagerAPIPrx::checkedCast(objprx);

				int max = boost::lexical_cast<int>(split_config[2]);
				int scope = boost::lexical_cast<int>(split_config[3]);

				for (int i = -7; i < max; i++ ) {
					if (tm->size(i) >= scope) {
						ostringstream oss;
						oss << service << " task" << i << " buffer size: " << tm->size(i);

						ostringstream oss_time;
						oss_time << Date(time(NULL)).str();

						std::cout << oss_time.str() << " subject: " << cluster << " msg: " << oss.str() << std::endl;

						monitor::MonitorAlerterAdapter::instance().alert(cluster, oss.str(), 2, 900);
					}
				}
			} else {
				std::cout << "error: channel is not find!!" << std::endl;
			}

		}
		sleep(10);
	}

	std::cout << std::endl;
	ifile.clear();
	return 0;
}
