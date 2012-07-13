#ifndef RECOMMEND_BY_INFO_H_
#define RECOMMEND_BY_INFO_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "socialgraph/socialgraphutil/calculator/calculator.h"
#include "UtilCxx/src/TaskManager.h"

namespace xce {
namespace socialgraph {

static const int LEVEL_UPDATE_RECOMMEND = 5;
static const int UNIV_TYPE = 0;
static const int HIGHSCHOOL_TYPE = 1;
static const int JUNIORSCHOOL_TYPE = 2;
static const int ELEMENTARYSCHOOL_TYPE = 3;

static const int AGE_INTERVAL = 3;		//取用户年龄间隔在3以内的
static const int RECOMMEND_DB_SIZE = 1000;		//db 每种类型最多存1000个用户

static const long REBUILD_INTERVAL = 2 * 24 * 3600;		//重算时间为2天

//-----------------------------------------------------------------

typedef std::map<long, std::vector<int> > Md2List;
typedef std::map<int, int> Int2IntMap;
typedef std::vector< std::vector<std::string> > InfoList;

/*Data Struct returned from recommend_by_info_new table*/
struct RecommendItem {
	RecommendItem() {
		userid_ = 0;
	}

	int userid_;
	std::string school_name_;
	std::vector<int> recommend_list_;
};

//-----------------------------------------------------------------

/*根据用户资料推荐好友*/
class RecommendByInfo: public Calculator {
public:
	RecommendByInfo(const std::string& load_file): load_file_(load_file) {}
	void Detail();

public:
	static Md2List univ_info_map;
	static Md2List highschool_info_map;
	static Md2List juniorschool_info_map;
	static Md2List elementary_info_map;
	static Int2IntMap view_count_map;

private:
	void Initialize();
	void GetLoginIds(const std::string& file, std::vector<int>& userids);

	std::string load_file_;
};

//-----------------------------------------------------------------

/*更新按资料推荐数据的线程*/
class UpdateRecommendDataTask: public MyUtil::Task {
public:
	UpdateRecommendDataTask(int userid, int level = LEVEL_UPDATE_RECOMMEND):
		MyUtil::Task(level), userid_(userid) {
	}
	virtual void handle();

private:
	void GetBlockList(int userid, std::vector<int>& block_list);

	void InsertUnivData(const std::vector<int>& friends);
	void InsertHighschoolData(const std::vector<int>& friends);
	void InsertJuniorschoolData(const std::vector<int>& friends);
	void InsertElementaryschoolData(const std::vector<int>& friends);
	void ConvertToSet(const std::vector<int>& source, std::set<int>& dest);
	void ConvertToVector(const std::set<int>& source, std::vector<int>& dest);

	void FilterByFriends(const std::vector<int>& friends, std::set<int>& dest);
	void FilterByPhoto(std::vector<int>& recommends);
	void FilterByStatus(std::vector<int>& recommends);
	void FilterByUserAge(std::vector<int>& recommends);

	void ResizeRecommends(std::vector<int>& recommends);

	bool IfCalculate(int userid);

	void PrintSet(const std::set<int>& idset) const;
	void PrintVec(const std::vector<int>& idvec) const;

	int userid_;
};

}
}

#endif
