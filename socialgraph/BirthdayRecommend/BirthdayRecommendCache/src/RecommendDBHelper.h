#ifndef _RECOMMEND_DBHELPER_H_
#define _RECOMMEND_DBHELPER_H_

#include <string>
#include <QueryRunner.h>

namespace xce {
namespace helper {

class RecommendDBHelper {
public:
	static void RemoveRecommend(int hostId, int friendId);

};

//****************************************************************************************

class TableName {
public:
	static std::string BirthdayBlockTable(int id) {
		ostringstream oss;
		oss << "birthdayrecommend_block_" << (id % 10);
		return oss.str();
	}
};

}
}

#endif
