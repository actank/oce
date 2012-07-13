#ifndef DB_HELPER_H_
#define DB_HELPER_H_

#include "QueryRunner.h"
#include "ObjectCacheI.h"
#include "socialgraph/recommendbyinfo/cpp/recommendbyinfo.h"

namespace xce {
namespace socialgraph {

class DbHelper {
public:
	static void ReadFriendList(int userid, std::vector<int>& friends);
	static void ReadApplyList(int userid, std::vector<int>& accepter_list);
	static void GetUnivInfo(int userid, InfoList& univ_info);
  static void GetHighschoolInfo(int userid, InfoList& highschool_info);
  static void GetJuniorschoolInfo(int userid, InfoList& juniorschool_info);
  static void GetElementaryschoolInfo(int userid, InfoList& elementaryschool_info);
	static bool InsertRecommendByInfoDb(int userid, int type, const std::vector<int>& recommends);
	static void GetRecommendFromInfoDb(int userid, int type, std::vector<int>& recommends);
	static void GetBatchAgeInfoDb(const std::vector<int>& total_ids, std::map<int, int>& id2age);
	static long GetCreateDataTime(int userid);

	static long GetCreateDataTimeFromNewInfoDb(int userid);
	static void DeleteRecommendByInfoDb(int userid, int stage);		//delete recommend items by type
	static bool InsertRecommendByInfoDbByStage(int userid, 
																						 int stage, 
																						 int stage_index, 
																						 const std::string school_name, 
																						 const std::vector<int>& recommends);		//insert data by user`stage
	static void GetRecommendFromInfoDbByStage(int userid, int stage, std::vector<RecommendItem>& recommend_items);
};

//-----------------------------------------------------------------

class ViewCountHandler: public com::xiaonei::xce::ResultHandler {
public:
  ViewCountHandler(int& last, Int2IntMap& view_count_map): 
    last_(last), view_count_map_(view_count_map) {}
  virtual bool handle(mysqlpp::Row& row) const;

private:                 
  int& last_;            
	Int2IntMap& view_count_map_;
};

//-----------------------------------------------------------------

class UnivBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UnivBatchReadHandler(int& last, Md2List& result_map): last_(last), result_map_(result_map) {}
  virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& last_;
  Md2List& result_map_;
};

//-----------------------------------------------------------------

class HighschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	HighschoolBatchReadHandler(int& last, Md2List& result_map): last_(last), result_map_(result_map) {}
  virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& last_;
  Md2List& result_map_;
};

//-----------------------------------------------------------------

class JuniorschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	JuniorschoolBatchReadHandler(int& last, Md2List& result_map): last_(last), result_map_(result_map) {}
  virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& last_;
  Md2List& result_map_;
};

//-----------------------------------------------------------------

class ElementaryschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	ElementaryschoolBatchReadHandler(int& last, Md2List& result_map): 
		last_(last), result_map_(result_map) {}
  virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& last_;
  Md2List& result_map_;
};

//-----------------------------------------------------------------

class FriendReadHandler : public com::xiaonei::xce::ResultHandler {
public:
  FriendReadHandler(std::vector<int>& friends) : friends_(friends) {}
  virtual bool handle(mysqlpp::Row& row) const;

private:
  std::vector<int>& friends_;
};

//-----------------------------------------------------------------

class UnivReadHandler : public com::xiaonei::xce::ResultHandler {
public:
  UnivReadHandler(InfoList& univ_info): univ_info_(univ_info) {}
  virtual bool handle(mysqlpp::Row& row) const;
    
private:
  InfoList& univ_info_;
};
  
//-----------------------------------------------------------------
    
class HighschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
  HighschoolReadHandler(InfoList& highschool_info): highschool_info_(highschool_info) {}
  virtual bool handle(mysqlpp::Row& row) const;

private:
  InfoList& highschool_info_;
};

//-----------------------------------------------------------------

class JuniorschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
  JuniorschoolReadHandler(InfoList& juniorschool_info): juniorschool_info_(juniorschool_info) {}
  virtual bool handle(mysqlpp::Row& row) const;

private:
  InfoList& juniorschool_info_;
};

//-----------------------------------------------------------------

class ElementaryschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
  ElementaryschoolReadHandler(InfoList& elementaryschool_info): elementaryschool_info_(elementaryschool_info) {}
  virtual bool handle(mysqlpp::Row& row) const;

private:
  InfoList& elementaryschool_info_;
};

//-----------------------------------------------------------------

class RecommendByInfoReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	RecommendByInfoReadHandler(std::vector<int>& recommends): recommends_(recommends) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<int>& recommends_;
};

//-----------------------------------------------------------------

class RecommendByInfoNewReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	RecommendByInfoNewReadHandler(std::vector<RecommendItem>& recommends): recommends_(recommends) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<RecommendItem>& recommends_;
};

//-----------------------------------------------------------------

class UserAgeInfoReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAgeInfoReadHandler(std::map<int, int>& id2age): id2age_(id2age) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, int>& id2age_;
};

//-----------------------------------------------------------------

class RelationResultHandler : public com::xiaonei::xce::ResultHandler {
public:
  RelationResultHandler(std::vector<int>& results, const std::string& toField);
  virtual ~RelationResultHandler();
protected: 
  virtual bool handle(mysqlpp::Row& row) const;
private:
  std::vector<int>& _results;
  std::string _toField;
};  

}
}

#endif
