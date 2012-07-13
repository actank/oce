#ifndef UGC_MODEL_H_
#define UGC_MODEL_H_

#include <ObjectCacheI.h>

#include <string>
#include <map>
#include <vector>

struct RawUgcModel {
	RawUgcModel(int ownerid, long ugcid, int comment_count, int view_count, int weight, int type, const std::string& interest) {
		ownerid_ = ownerid;
		ugcid_ = ugcid;
		comment_count_ = comment_count;
		view_count_ = view_count;
		weight_ = weight;
		type_ = type;
		interest_ = interest;
	}

	int ownerid_;
	long ugcid_;
	int comment_count_;
	int view_count_;
	int weight_;
	int type_;
	std::string interest_;
};

//---------------------------------------------------------------------------------------------

class RawUgcManager {
public:
	void LoadDataFromFile(const std::string& file_name);
	std::vector<RawUgcModel> GetUgcList(int ownerid);
	void PrintInfo();

private:
	std::map<int, std::vector<RawUgcModel> > user_ugc_map_;
};

#endif
