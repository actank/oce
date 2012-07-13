/*
 * parser.h
 *
 *  Created on: Apr 8, 2010
 *      Author: yejingwei
 */
#ifndef __HEADER_EXPRESSIONPARSER__H__
#define __HEADER_EXPRESSIONPARSER__H__

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "LogWrapper.h"


namespace xce {
namespace feed {
using namespace std;


const string kFactorTypes =  "ufaApsvgzxc";//表达式因子类型列表

const string kOpTypes = "+-&";//表达式运算符列表


//class TempResult:public IceUtil::Shared{ //用来保存中间结果
//public:
//  set<int> ids_;
//};
//typedef IceUtil::Handle<TempResult> TempResultPtr;

class Factor: public IceUtil::Shared {
public:
 // virtual void PrintInfo()=0;
  Factor(char type, int id) :
    type_(type), param_id_(id) {
  }
  virtual void Plus(set<int>&)=0;
  virtual void Minus(set<int>&)=0;
  virtual void And(set<int>&)=0;
  //void FetchIds(const char & op, set<int> & ids);
  int param_id() {
    return param_id_;
  }
  char type() {
    return type_;
  }
  virtual ~Factor(){}
protected:
  char type_;
  int param_id_;
};

typedef IceUtil::Handle<Factor> FactorPtr;

//-----------------------------------------------------------------------------------
class Op {
public:
  Op(int op) :
    op_type_(op) {
  }
  char op_type() {
    return op_type_;
  }
  void PrintInfo() {
    MCE_DEBUG("Op: " << op_type_);
  }
private:
  char op_type_;
};

//-----------------------------------------------------------------------------------
class Parser {
public:
  bool Init(string & expr) ;
  vector<FactorPtr> Tokenize();
  set<int> CalculateIds();
  void PrintInfo();
  void PrintIds();
private:
  FactorPtr ScanFactor(size_t & pos);
  int ScanInteger(size_t & pos);

  string expr_str_;

  vector<FactorPtr> factors_;
  vector<Op> ops_;

  set<int> expr_ids_;//计算结果
};


//---------------------------------------------------------------------------------------------
//各种类型的factor
class UserFactor: public Factor {
public:
  UserFactor(char type, int value) :
    Factor(type,value) {
    ;
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~UserFactor(){}
private:
  string instance_,table_;
};
class FriendFactor: public Factor {
public:
  FriendFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~FriendFactor(){}
private:
  set<int> FiltFriend();
  string instance_,table_;
  string GenerateSQL();
};
class AppFactor: public Factor {
public:
  AppFactor(char type, int value) :
    Factor(type,value) ,instance_("app_user"){
    ;
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~AppFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};
class AppFanFactor: public Factor {
public:
  AppFanFactor(char type, int value) :
    Factor(type,value) ,instance_("app_fans"){
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~AppFanFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};

class PageFactor: public Factor {
public:
  PageFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~PageFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};

class FollowFactor: public Factor {
public:
  FollowFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~FollowFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};

class HighSchoolFactor: public Factor {
public:
  HighSchoolFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~HighSchoolFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();
};

class MiniGroupFactor: public Factor {
public:
  MiniGroupFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~MiniGroupFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();
};

class XiaoZuFactor: public Factor {
public:
  XiaoZuFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~XiaoZuFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};

class MiniSiteFactor: public Factor {
public:
  MiniSiteFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~MiniSiteFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};

class CheWenFactor: public Factor {
public:
  CheWenFactor(char type, int value) :
    Factor(type,value) {
  }
  virtual void Plus(set<int>&);
  virtual void Minus(set<int>&);
  virtual void And(set<int>&);
  virtual ~CheWenFactor(){}
private:
  string instance_,table_;
  string GenerateSQL();

};


}
;
}
;
#endif
