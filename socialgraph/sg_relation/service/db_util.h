#ifndef _SG_RELATION_DB_UTIL_H_
#define _SG_RELATION_DB_UTIL_H_

#include "QueryRunner.h"
#include "SocialGraphRelation.h"

namespace xce {
namespace socialgraph {

struct FirstLevelRow {
  int index_value_;
  std::string desc_;
};

typedef std::vector<FirstLevelRow> FirstLevelRowSeq;

struct SecondLevelRow {
  int index_value_;
  std::string desc_;
  int parent_level_;
};

class FirstLevelResultHandler : public com::xiaonei::xce::ResultHandler {
 public:
  FirstLevelResultHandler(FirstLevelRowSeq& seq):seq_(seq) { }
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  FirstLevelRowSeq& seq_;
};

typedef std::vector<SecondLevelRow> SecondLevelRowSeq;

class SecondLevelResultHandler : public com::xiaonei::xce::ResultHandler {
 public:
  SecondLevelResultHandler(SecondLevelRowSeq& seq):seq_(seq) { }
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  SecondLevelRowSeq& seq_;
};

struct EmotionStateRow {
  int user_id_;
  int first_level_;
  int second_level_;
};

struct SgRelationRow {
  int host_;
  int guest_;
  int first_level_;
  int second_level_;
};

typedef std::vector<SgRelationRow> SgRelationRowSeq;

class EmotionStateResultHandler : public com::xiaonei::xce::ResultHandler {
 public:
  EmotionStateResultHandler(EmotionStateRow& es_row) : es_row_(es_row) { }
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  EmotionStateRow& es_row_;
};

class SgRelationResultHandler : public com::xiaonei::xce::ResultHandler {
 public:
  SgRelationResultHandler(SgRelationRowSeq& seq) : seq_(seq) {}
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  SgRelationRowSeq& seq_;
};

class DbUtil {
 public:
  static FirstLevelRowSeq GetFullFirstLevel();
  static SecondLevelRowSeq GetFullSecondLevel();
  static EmotionStateRow GetEmotionState(int user_id); 
  static SgRelationRowSeq GetSgRelation(int user_id);
  static void UpdateEmotionState(int user_id, const LevelTuple&);
  static void UpdateSgRelation(int host, int guest, const LevelTuple&);
  static void DeleteSgRelation(int host, int guest, const LevelTuple&);
  static std::string GetSgRelationTableName(int user_id);
 
 public:
  static const int kSgRelationTableScalar = 100;
};
}}

#endif

