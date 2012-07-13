#ifndef __SrcDataGenerator_H_
#define __SrcDataGenerator_H_

#include <boost/shared_ptr.hpp>
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <numeric>

const int THREAD_NUM = 1;
const int BATCH_URL_SIZE = 1000;

using std::string;
using std::vector;
using std::ofstream;

struct FieldInfo {
  //TODO:使用枚举值
  FieldInfo (const string& name, const string& type, const string& other) : name_(name), type_(type), other_(other) {
  }
  string name_;
  string type_;
  string other_;
};

class LoadBusinessThread : virtual public IceUtil::Thread  {
  public:
    LoadBusinessThread(string& business_type, int table_no, string& config_file, string& output_path);
    bool ProcessConfigFile(string& config_file);
    void AddField(const string& name, const string& type, const string& other);
    void AddFilter(const string& filter);
    void run();
    void setMinId(long min_id);
    void setMaxId(long max_id);
  private:
    string getTableName();
    //获得最大最小id的函数
    long getId(const string& sql);
    long min_id_;
    long max_id_;
    string business_type_;
    int table_no_;
    string config_file_;
    string output_path_;
    vector<FieldInfo> fields_;
    vector<string> filters_;
};

typedef std::vector<LoadBusinessThread *>  LoadBusinesssVec;

class BatchBusinessResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BatchBusinessResultHandlerI(long& max_business_id, ofstream & output, vector<FieldInfo> & fields);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    long& max_business_id_;
    ofstream & output_;
    vector<FieldInfo> & fields_;
};

class MinMaxIdResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    MinMaxIdResultHandlerI(long& min_max_id);
    virtual bool handle(mysqlpp::Row& row) const; 
  private:
    long& min_max_id_;
};

#endif
