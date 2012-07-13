#include <IceUtil/IceUtil.h>
#include <FriendInputAdapter.h>
#include <UserBase.h>
#include <UserBaseAdapter.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "LogWrapper.h"
#include <ConnectionHolder.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <gtest/gtest.h>
#include <mysql/mysql.h>
using namespace std;
using namespace MyUtil;
using namespace xce;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::services::FriendInput;
using namespace xce::adapter::userbase;

const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";
ifstream pinyin_dict("/data/xce/tools/pinyin_dictionary.txt");

void Usage()  {
  cout << "Usage<FriendInputTestTools> -t<test_case> -i<id> -q<query>" << endl;
}

void splitWord(string& value, vector<string>& words, vector<int> & englishInName)
{
  int count =0;
  for(vector<string>::size_type i=0;i<value.size();++i) {
    if((value[i]&0x80)!=0) {
      words.push_back(value.substr(i,3));
      count++;
      i+=2;
    }else {
      if(value[i] >= 'A' && value[i] <= 'Z')
        value[i] += 32;
      words.push_back(string(1,value[i]));
      englishInName.push_back(count);
      count++;
    }
  }
}

map<string,vector<string> > dict_;
void  readPinYinDict()  {
  ifstream input_file("pinyin_dictionary.txt");
  string characters, pinyin, line;
  int index;
  while(!input_file.eof())  {
    getline(input_file,line,'\n');
    index = line.find("\t");
    characters = line.substr(0,index);
    pinyin = line.substr(index+1,line.length() - index - 1);
    dict_[characters].push_back(pinyin);
  }
}

void composePinYin(const string& name, vector< vector<string> >& words)  {
  readPinYinDict();
  string characters;
  vector<string> temp;
  for(size_t i = 0; i < name.length(); ++i)  {
    if((name[i]&0x80) == 0x80)  {
      characters = name.substr(i,3);
      for(size_t j = 0; j < dict_[characters].size(); j++)  {
        temp.push_back(dict_[characters].at(j));
      }
      words.push_back(temp);
      temp.clear();
      i+=2;
    }
  }
}

int queryType(string query)  {
  if(query == "")  return -1;
  else  {
    int count = 0;
    for(size_t i = 0; i < query.length(); ++i)  {
      if((query[i] & 0x80) == 0x80)  {
        count++;
        i += 2;      
      }
    }
    if(count == 0)  return 0;
    else if(count*3 < query.length())  return 1; 
    else if(count*3 == query.length())  return 2;
  }
}

void transform(string& query)  {
  for(int i=0; i<query.length(); ++i)  {
    if((query[i] & 0x80) == 0x80)
      i += 2;
    else  {
      if(query[i] >= 'A' && query[i] <= 'Z')  {
        query[i] += 32;
      }
    }
  }
}

bool startsWith(const std::string& base, const std::string& pre) {
  if (base.size()<pre.size()) {
    return false;
  }
  int pc=pre.size();
  int po=0;
  while (--pc >= 0) {
    if (base[po] != pre[po++] ) {
      return false;
    }
  }
  return true;
}

bool decideHZ(vector<string> & split_HZ, vector<string> & unit_name)  {
  int symbol = 0, count = 0;
  for(int i = 0; i < split_HZ.size(); i++)  {
    for(int j = symbol; j < unit_name.size(); j++)  {
      if(split_HZ.at(i) == unit_name.at(j))  {
        count++;
        symbol = j+1;
        break;
      }
    }
  }
  if(count == split_HZ.size())
    return true;
  return false;
}


  bool nameIncludePY(string& query_pinyin, vector<string> & unit_name, vector<int> & englishInName)  {
    if(englishInName.empty())
      return false;
    else  {
      if(query_pinyin.length() > englishInName.size())
        return false;
      else  {
        int symbol = 0, count = 0;
        for(int i = 0; i < query_pinyin.length(); i++)  {
          for(int j = symbol; j < englishInName.size(); j++)  {
            if(query_pinyin.substr(i,1) == unit_name.at(englishInName.at(j)))  {
              count++;
              symbol = j+1;
              break;
            }
          }
        }
        if(count == query_pinyin.length())
          return true;
      }
      return false;
    }
  }

bool splitPY(string& query_pinyin, unsigned int start, int num, vector< vector<string> > & name_pinyin)  {
  if(start == query_pinyin.length())
    return true;
   string split_temp;
  for(int end = start + 1; end <= query_pinyin.length(); end++)  {
    split_temp = query_pinyin.substr(start,end-start);
    for(int j = num; j < name_pinyin.size(); j++)  {
      for(int k = 0; k < name_pinyin.at(j).size(); k++)  {
        if(startsWith(name_pinyin.at(j).at(k), split_temp))  {
          if(splitPY(query_pinyin, end, num+1, name_pinyin))  {
            return true;
          }
        }
      }
    }
  }
}

bool decidePY(string& query_pinyin, unsigned int start, int num, vector< vector<string> > & name_pinyin,
    vector<string> & unit_name, vector<int> & englishInName)  {
  if(nameIncludePY(query_pinyin, unit_name, englishInName)  ||  splitPY(query_pinyin, start, num, name_pinyin))  {
    std::cout<<"拼音query:"<<query_pinyin<<std::endl;
    std::cout<<"englishInName:";
    for(int i=0; i<englishInName.size(); i++)
      std::cout<<englishInName.at(i)<<" ";
    std::cout<<endl;
    if(nameIncludePY(query_pinyin, unit_name, englishInName)) 
      std::cout<<"nameIncludePY返回true"<<std::endl;
    if(splitPY(query_pinyin, start, num, name_pinyin))
      std::cout<<"splitPY返回true"<<std::endl;
    return true;
 }
  std::cout<<"decidePY返回false"<<std::endl;
  return false;
}


TEST(FriendInputTestCase, TestFriendInput) {
  int id = 25701953;
  string query = "lijie";
  vector<int>  results;
  vector<string> names;

  results = FriendInputAdapter::instance().getFriendInput(id,query,100);
  transform(query);

  for(size_t i=0; i<results.size() ;i++)
    names.push_back((UserBaseAdapter::instance().getUserBaseCommonView(results.at(i)))->name());
  int query_type = queryType(query);
  for(size_t i=0; i<results.size() ;i++)  {
    vector<string> unit_in_name;
    unit_in_name.clear();
    vector< vector<string> > pinyin_in_name;
    pinyin_in_name.clear();
    vector<int> englishInName;
    englishInName.clear();
    vector<string> split_query;
    split_query.clear();
    splitWord(names[i], unit_in_name, englishInName);
    composePinYin(names[i], pinyin_in_name);
    
    cerr<<names.at(i)<<endl; 
    if(query_type == 0)  {
      EXPECT_TRUE(decidePY(query, 0, 0, pinyin_in_name, unit_in_name, englishInName));
    }
    }
}

int main(int argc, char ** argv)
{
   MyUtil::init_logger("Mce", "FriendInput_tools.log", "DEBUG");
   if (1 == argc || 0 == strcmp(argv[1], "--help")) {
     Usage();
     return -1;
   }
   else if (0 == strcmp(argv[1], "t"))  {
     testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
    }
   else if (0 == strcmp(argv[1], "i") && 0 == strcmp(argv[1], "q")) {
     
   }
   else {
     Usage();
     return -1;
   }
 }
