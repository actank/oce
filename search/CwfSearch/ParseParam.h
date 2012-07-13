#ifndef _PARSEPARAM_H_
#define _PARSEPARAM_H_

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace xce {
  namespace parseparam {

    class ParseParam {
      public:
        std::string getKey(const std::string& type, int index, const std::string& subType) {
          std::stringstream key;
          if (type == "base") {
            if (subType == "gend") {
              key << "GENERAL|gender";
            } else if (subType == "prov") {
              key << "GENERAL|home_province";
            } else if (subType == "city") {
              key << "GENERAL|home_city";
            }
          } else if (type == "age") {
            key << "GENERAL|range.age";
          } else if (type == "high") {
            if (subType == "name") {
              key << "HIGH_SCHOOL_INFO|" << index << "|high_school_name";
            } else if (subType == "year") {
              key << "HIGH_SCHOOL_INFO|" << index << "|enroll_year";
            }
          } else if (type == "juni") {
            if (subType == "name") {
              key << "JS_INFO|" << index << "|junior_high_school_info.junior_high_school_name";
            } else if (subType == "year") {
              key << "JS_INFO|" << index << "|junior_high_school_info.junior_high_school_year";
            }
          } else if (type == "univ") {
            if (subType == "id") {
              key << "UNIVERSITY_INFO|" << index << "|university_id";
            } else if (subType == "name") {
              key << "UNIVERSITY_INFO|" << index << "|university_name";
            } else if (subType == "year") {
              key << "UNIVERSITY_INFO|" << index << "|enroll_year";
            }
          } else if (type == "work") {
            key << "WORKPLACE_INFO|" << index << "|workplace_name";
          }
          return key.str();
        }
      
      std::map<std::string, std::string> json2map(const std::string& json) {
          std::string subString = std::string(json,1,json.size() - 2);
          int first = 0, count = 0, num = 0, index = 0;
          std::string key = "", text = "", value = "", type = "";
          std::vector<std::string> results;
          std::map<std::string, std::string> str2str;
          for (int i = 0; i < subString.size(); i++) {
            count++;
            if (subString[i] == '{') {
              first = i;
              count = 0;
            }
            if (subString[i] == '}') {
              results.push_back(std::string(subString, first + 1, count - 1));
            }
          }
          for (int j = 0; j < results.size(); j++) {
            count = 0;
            first = 0;
            for (int k = 0; k < results[j].size(); k++) {
              count++;
              if (results[j][k] == '\"') {
                if (num % 2 != 0) {
                  text = std::string(results[j], first + 1, count - 1);
                }
                if (num % 2 == 0) {
                  count = 0;
                  first = k;
                }
                num++;
              }
              if (results[j][k] == ':') {
                key = text;
              }
              if (results[j][k] == ',') {
                value = text;
                if (key == "t") {
                  index++;
                  type = value;
                } else {
                  key = getKey(type,index,key);
                  str2str[key] = value;
                }
              }
              if (k == results[j].size() - 1){
                value = text;
                if (key == "t") {
                  index++;
                  type = value;
                } else {
                  key = getKey(type,index,key);
                  str2str[key] = value;
                }
              }
            }
          }
          return str2str;
        }
    };
  };
};
#endif
