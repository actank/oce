/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2011 04:44:30 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "DBUtils.h"
#include "SetUtils.h"
#include <iostream>

using namespace xce::utils;
using namespace std;

int main(int argc, char* argv[]) {
  string prefix = "user_config";
  cout << "TableName = " << DBUtils::getTableName<int>(134343, 100, prefix) << endl; 

  vector<int> intNums;
  cout << "IdsStr = " << DBUtils::getIdsStr<int>(intNums) << endl;
  vector<long> longNums;
  longNums.push_back(13143143);
  cout << "IdsStr = " << DBUtils::getIdsStr<long>(longNums) << endl;
  longNums.push_back(131431343);
  cout << "IdsStr = " << DBUtils::getIdsStr<long>(longNums) << endl;

  longNums.push_back(1234222);
  longNums.push_back(1312344);
  longNums.push_back(1343344);
  map<long, vector<long> > seperatedIds = DBUtils::seperateIdsByMod(longNums, 100);
  for (map<long, vector<long> >::const_iterator it = seperatedIds.begin(); it != seperatedIds.end(); ++it) {
    cout << "-->" << it->first << ": " << endl;
    for (vector<long>::const_iterator itp = it->second.begin(); itp != it->second.end(); ++itp) {
      cout << *itp << " ";
    }
    cout << endl;
  }
  return 0;
}
