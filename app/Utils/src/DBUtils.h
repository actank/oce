/*
 * =====================================================================================
 *
 *       Filename:  DBUtils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2011 04:31:01 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __DBUTILS_H__
#define __DBUTILS_H__
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <boost/lexical_cast.hpp>
#include <QueryRunner.h>

namespace xce {
namespace utils {

  using namespace std;

  class DBUtils {
    public:
      template<typename T>
      static string getTableName(T id, T mod, const string& prefix);
      template<typename T>
      static string getIdsStr(const vector<T>& ids);
      template<typename T>
      static map<T, vector<T> > seperateIdsByMod(const vector<T>& ids, int mod);
  };

  template<typename T>
  class BatchIdResultHandler : public com::xiaonei::xce::ResultHandler {
    public:
      BatchIdResultHandler(const string& idColumnName, vector<T>& ids);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      string _idColumnName;
      vector<T>& _ids;
  };

  template<typename T>
  string DBUtils::getTableName(T id, T mod, const string& prefix) {
    return prefix + "_" + boost::lexical_cast<string>(id % mod);
  }

  template<typename T>
  string DBUtils::getIdsStr(const vector<T>& ids) {
    ostringstream result;
    result << "(";
    if(ids.size() > 0) {
      result << boost::lexical_cast<string>(ids[0]);
      for(size_t i = 1; i < ids.size(); ++i) {
        result << ", " << boost::lexical_cast<string>(ids[i]);
      }
    }
    result << ")";
    return result.str();
  }

  template<typename T>
  map<T, vector<T> > DBUtils::seperateIdsByMod(const vector<T>& ids, int mod) {
    map<T, vector<T> > result;
    for(size_t i = 0; i < ids.size(); ++i) {
      typename map<T, vector<T> >::iterator foundIt = result.find(ids[i] % mod);
      if(foundIt == result.end()) {
        vector<T> modVector;
        modVector.push_back(ids[i]);
        result.insert(make_pair<T, vector<T> >(ids[i] % mod, modVector));
      } else {
        foundIt->second.push_back(ids[i]);
      }
    }
    return result;
  }

  template<typename T>
  BatchIdResultHandler<T>::BatchIdResultHandler(const string& idColumnName, vector<T>& ids) : _idColumnName(idColumnName), _ids(ids) {
  }

  template<typename T>
  bool BatchIdResultHandler<T>::handle(mysqlpp::Row& row) const {
    T id = (T)row[_idColumnName.c_str()];
    _ids.push_back(id);
    return true;
  }

}
}

#endif
