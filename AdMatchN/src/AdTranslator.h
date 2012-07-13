#ifndef ADTRANSLATOR_H
#define ADTRANSLATOR_H

#include <Ice/Ice.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
#include <iostream>
#include <string>
#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include "Singleton.h"
#include "ServiceI.h"



namespace xce{
namespace ad{
using namespace std;

const static int AREA_MAP_SIZE = 83;
const static int GREADE_MAP_SIZE = 70;



static int AREA_MAP[AREA_MAP_SIZE] = {
0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,2,3,4,5,-1,-1,-1,-1,-1,6,7,8,-1,-1,-1,-1,
-1,-1,-1,9,10,11,12,13,14,15,-1,-1,-1,16,17,18,19,20,21,-1,-1,-1,22,23,24,25,
26,-1,-1,-1,-1,-1,-1,27,28,29,30,31,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,
-1,33,34
};

static int GREADE_MAP[GREADE_MAP_SIZE] = {
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //25
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,-1,-1,-1,-1,-1,5,6,7,-1,
-1,-1,-1,-1,-1,-1,8,8,8,8,8,8,8,8,8
};


const int AREA_POS = 0;               //0
const int GRADE_POS = 0 + 36;         //36
const int STAGE_POS = 0 + 36 + 10;     //46
const int GENDER_POS = 0 + 36 + 10 + 3;//49
const int AGE_POS = 0 + 36 + 10 + 3 + 2;//51

const uint64_t ONLY_AREA_SET = 68719476735;
const uint64_t ONLY_GRADE_SET = 70300024700928;
const uint64_t ONLY_STAGE_SET = 492581209243648;
const uint64_t ONLY_GENDER_SET = 1688849860263936;
const uint64_t ONLY_AGE_SET = 2303591209400008704;

struct BidUnit{
  BidUnit(){
    gender = 1;
    stage = 4;
    grade = "";
    age = "";
    area = "";
    school = "";
  }
  Ice::Long bid_unit_id;
	int gender;
	int stage;
	string grade;
	string age;
	string area;
	string school;
};
struct User{
  User(){
    gender = 1;
    stage = 4;
    grade = 41;
    age = 25;
    area = "";
  }
	int gender;
	int stage;
	int grade;
	int age;
	string area;
};

class Translator : public MyUtil::Singleton<Translator>{
public:
  int server_index;
	uint64_t GetIndex(const User& user);
	uint64_t GetIndex(const BidUnit& bid_unit);
	uint64_t GetIndex(const UserProfile& profile);
	vector<uint64_t> ParseAreas(const string& str);
	vector<uint64_t> ParseSchools(const string& str);
	vector<uint64_t> Split(const string& str);
	bool IsArea(uint64_t area_or_school) {
	  return area_or_school > 999999999999;//学校码小于这个数，地域码都大于这个数
	}
	void set_server_index(int si){
	  server_index = si;
	}
/*	uint64_t GetThresholdByAge(int pos){

	}*/
private:
	uint64_t CodeAge(int age);
	uint64_t CodeAge(const string& age);
	uint64_t CodeArea(int area);
	uint64_t CodeArea(const string& area);
	uint64_t CodeGender(int gender);
	uint64_t CodeStage(int stage);
	uint64_t CodeGrade(int grade, int stage);
	uint64_t CodeGrade(const string& grade, int stage);
	uint64_t CodeSchool(const string& school);
};
};
};
#endif
