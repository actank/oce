#include "SrcDataGenerator.h"
#include <string.h>

//#define _USE_BOOST_1_4_5_
//#define TEST

#ifdef _USE_BOOST_1_4_5_
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#endif 

using std::string;
using namespace com::xiaonei::xce;

const string db_name= "adminsearch";


LoadBusinessThread::LoadBusinessThread(string& business_type, int table_no, string& config_file, string& output_path) : business_type_(business_type), table_no_(table_no) , config_file_(config_file), output_path_(output_path), min_id_(0), max_id_(numeric_limits<long>::max()) {
}

void LoadBusinessThread::AddField(const string& name, const string& type, const string& other) {
    FieldInfo temp_field(name, type, other);
    fields_.push_back(temp_field);
}

void LoadBusinessThread::AddFilter(const string& filter) {
  filters_.push_back(filter);
}

void LoadBusinessThread::setMinId(long min_id) {
  min_id_ = min_id;
}

void LoadBusinessThread::setMaxId(long max_id) {
  max_id_ = max_id;
}

bool LoadBusinessThread::ProcessConfigFile(string& config_file) {
  //config.ini配置文件格式：
  ///fields_num=2
  ///field0_name=id
  ///field0_type=long
  ///field0_info=null
  ///field1_name=owner
  ///field1_type=int
  ///field1_info=null
#ifdef _USE_BOOST_1_4_5_
  using boost::property_tree::ptree;
  ptree pt; 
  read_ini(config_file, pt);

  int fields_num = pt.get<int>("fields_num");
  cout << "fields_num=" << fields_num << endl;

  for(int i=0; i<fields_num; ++i) {
    stringstream ss; 
    ss << "field" << i << "_";
    string name = pt.get<string>(ss.str() + "name");
    string type = pt.get<string>(ss.str() + "type");
    string info = pt.get<string>(ss.str() + "info");

    cout << i << ": name=" << name << ", type=" << type << ", info=" << info << endl;
    AddField(name, type, info);
  }


  int filter_num = pt.get<int>("filter_num");
  cout << "filter_num=" << filter_num <<endl;

  for (int i=0; i<filter_num; ++i) {
    stringstream ss;
    ss << "filter" << i;
    string filter = pt.get<string>(ss.str());
    cout << ss.str() << "=" << filter << endl;
    AddFilter(filter);
  }

  //added by min.shang@renren-inc.com, 添加要查寻的最大id和最小id，这样是为了避免在sql中出现where id > 0 and date > "2010-05-27"这样的语句，
  //我们可以先查到date > 2010-05-27的最小的id, 然后改写每次循环的sql, 不用将date > "2010-05-27"这个条件一直带进去
  try {
    string min_id_filter = pt.get<string>("min_id_filter");
    string max_id_fitler = pt.get<string>("max_id_filter");
    //因为在sql中作order by id desc, order by id asec这样的操作是比较耗时的，所以最大最小的id都是当天的最小id
    long min_id = getId("select id from " + getTableName() + " where " + min_id_filter + " limit 1");
    long max_id = getId("select id from " + getTableName() + " where " + max_id_fitler + " limit 1");
    min_id_ = min_id;
    max_id_ = max_id;
  } catch(exception ex) {
    //什么也不做，说明用户没有指定min_id和max_id
  }
#endif 
 return true;
}

long LoadBusinessThread::getId(const string& sql) {
  long id;
  Statement sql_statement;
  sql_statement << sql;
  MinMaxIdResultHandlerI handler(id);
  QueryRunner(db_name, CDbRServer, getTableName()).query(sql_statement, handler);
  return id;
}

string LoadBusinessThread::getTableName() {
  stringstream ss;
  ss << table_no_;
  string table = business_type_ + "_" + ss.str();
  return table;
}

void LoadBusinessThread::run()  {
  string table = getTableName();
  std::cout<<"table=" << table <<std::endl;
  int count = 0;

  string output_file_name = output_path_ + table;
  std::ofstream output_file(output_file_name.c_str());
  
  ProcessConfigFile(config_file_);

  long begin_business_id = min_id_;
  long max_business_id = 0;
  while((min_id_ == begin_business_id || max_business_id < begin_business_id) && begin_business_id < max_id_)  {
    cout << "min_id_ = " << min_id_ << " begin_business_id = " << begin_business_id << " max_id_ = " << max_id_ << endl;
    max_business_id = begin_business_id;
    Statement sql;

    stringstream ss;
    vector<FieldInfo>::iterator iter=fields_.begin();
    if (iter!=fields_.end()) {
      ss << iter->name_;
      ++iter;
    }
    for (; iter!=fields_.end(); ++iter) {
      ss << ", " << iter->name_;
    }

    stringstream filters;
    for (vector<string>::iterator iter=filters_.begin(); iter!=filters_.end(); ++iter) {
      filters << " AND " << *iter ;
    }

//    cout << "[DEBUG] filters:" << filters.str() << endl;

    stringstream sql_ss;
    sql_ss << "SELECT " << ss.str() << " FROM " << table << " WHERE id >= \""<< begin_business_id << "\" and id <= " << max_id_ << " " << filters.str() << " ORDER BY id LIMIT "<< BATCH_URL_SIZE;
    begin_business_id++;
    cout << "[DEBUG] sql_ss:" << sql_ss.str() << endl;
    sql << sql_ss.str();
    BatchBusinessResultHandlerI handler(begin_business_id, output_file, fields_);
    QueryRunner(db_name, CDbRServer, table).query(sql, handler);
    std::cout<<" cycle*batch :"<<count << "*"<< BATCH_URL_SIZE << std::endl;
    count++; 

#ifdef TEST
    if (count == 20)
      break;
#endif

  }
  std::cout<<"Fill task done!"<<std::endl;
}

BatchBusinessResultHandlerI::BatchBusinessResultHandlerI(long& max_business_id, 
    ofstream& output, vector<FieldInfo> &fields) :max_business_id_(max_business_id), output_(output), fields_(fields)  {
}

MinMaxIdResultHandlerI::MinMaxIdResultHandlerI(long& min_max_id) : min_max_id_(min_max_id) {
}

bool MinMaxIdResultHandlerI::handle(mysqlpp::Row& row) const {
  cout << "**************************************************" << row["id"] << endl;
  min_max_id_ = (long) row["id"];
  return true;
}

bool BatchBusinessResultHandlerI::handle(mysqlpp::Row& row) const  {   

  for (vector<FieldInfo>::iterator iter=fields_.begin(); iter!=fields_.end(); ++iter) {

    if (iter->type_ == "int") {
      int field_value = (int) row[(iter->name_).c_str()];
      output_<<field_value;
    } else if (iter->type_ == "long") {
      long field_value = (long) row[(iter->name_).c_str()];
      output_<<field_value;
    } else if (iter->type_ == "string") {
      string field_value = (string) row[(iter->name_).c_str()];
      //if (iter->other_ == "need_filter") {
      field_value.erase(remove(field_value.begin(), field_value.end(), '\001'), field_value.end());
      field_value.erase(remove(field_value.begin(), field_value.end(), '\r'), field_value.end());
      field_value.erase(remove(field_value.begin(), field_value.end(), '\n'), field_value.end());
      if (field_value == "")
        field_value = "NULL";
      //}
      output_<<field_value;
    }
    output_<<"\001";
  } 

  output_<< std::endl;

  long business_id = (long) row["id"];
  max_business_id_ = max_business_id_ > business_id ? max_business_id_:business_id;
  return true;
}

void Usage() {
  std::cerr<<"Usage: <业务类型(blog|share|doing) 第几个表 -config 配置文件(关于数据库字段) 输出路径>"<<std::endl;
}

  int main(int argc, char* argv[])  {
    if (1 == argc || 6 != argc)
    {
      Usage(); 
      return -1;
    }

    if (0 != strcmp(argv[3], "-config")) {
      Usage(); 
      return -1;
    }
    MyUtil::default_init_logger("Mce", "TRACE");

    string business_type(argv[1]);
    stringstream ss(argv[2]);
    int table_no; //第几台数据库
    ss >> table_no;
    com::xiaonei::xce::ConnectionPoolManager::instance().initialize();

    string config_file(argv[4]);
    string output_path(argv[5]);

    IceUtil::ThreadPtr t = new LoadBusinessThread(business_type, table_no, config_file, output_path);
    IceUtil::ThreadControl tc = t->start();
    tc.join();

    std::cout<<"FINISH BOOT"<<std::endl;
    return 0;
  }


