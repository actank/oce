/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/04/2011 06:05:35 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include <iostream>
#include "FriendsRecentPhotos.h"
#include "../../FriendsRecentPhotosCommon/src/FriendsRecentPhotosCommon.cpp"
#include "app/MiniGroup/MiniGroupCommon/src/MiniGroupCommon.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "ServiceI.h"

using namespace std;
using namespace xce::friendsphotos; 

void printUsage() {
  cout << "Usage:" << endl;
  cout << "getUserPhotosInfo [userIds] | insertUserPhotosInfo";
  cout << " [userIds photoN uploadTime1 uploadTime2 .... uploadTimeN photoId1 photoId2 ... photoIdN]" << endl;
}

void handleGetUserPhotosInfo() {
  cout << "Input userIds end with 0: " << endl;
  vector<int> userIds;
  int n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }
    userIds.push_back(n);
  }
  map<int, vector<int> > seperatedIds = xce::mngp::DBUtil<int>::seperateIdsByMod(TABLE_MOD, userIds);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) { 
    Statement sql;
    sql << "select " << COLUMN_USERID << ", " << COLUMN_PHOTOS << " from " << TABLE_NAME << "_" << itv->first % TABLE_MOD
        << " where " << COLUMN_USERID << " in " << xce::mngp::DBUtil<int>::getIdsStr(itv->second);
    vector<UserPhotosInfoPtr> ptrs;
    string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(itv->first % TABLE_MOD);
    QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, UserPhotosInfoResultHandlerI(ptrs));
    cout << "Result for " << xce::mngp::DBUtil<int>::getIdsStr(itv->second) << " is : " << endl;
    for (vector<UserPhotosInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      cout << (*it)->toString() << endl;
    }
  }
}

void handleInsertUserPhotosInfo() {
  while (true) {
    cout << "Continue ? please enter yes or no : " << endl;
    string answer;
    cin >> answer;
    if (answer.compare("yes") == 0) {
      int userId;
      cout << "Input userId:" << endl;
      cin >> userId;
      vector<int> uploadTimes;
      vector<long> photoIds;
      int uploadTime;
      long photoId;
      cout << "Input (uploadTime, photoIds)s, terminated with 0, 0:" << endl;
      while (cin >> uploadTime >> photoId) {
        if (uploadTime > 0 && photoId > 0) {
          uploadTimes.push_back(uploadTime);
          photoIds.push_back(photoId);
        } else {
          break;
        }
      }
      Statement sql;
      sql << "insert into " << TABLE_NAME << "_" << userId % TABLE_MOD << "(" << COLUMN_USERID << ", " 
          << COLUMN_PHOTOS << ") values (" << userId << ", '" << xce::friendsphotos::serialize(uploadTimes, photoIds) << "')"
          << " on duplicate key update photos = values(photos)";
      string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(userId % TABLE_MOD);
      QueryRunner(DB_SOURCE, CDbWServer, tableName).store(sql);
    } else {
      break;
    }
  } 
}

int main(int argc, char* argv[]) {
  ConnectionPoolManager::instance().initialize(); 
  if (argc < 2 || (strcmp(argv[1], "getUserPhotosInfo") != 0 && strcmp(argv[1], "insertUserPhotosInfo") != 0)) {
    printUsage();
    return 0;
  }
  if (strcmp(argv[1], "getUserPhotosInfo") == 0) {
    handleGetUserPhotosInfo();
  } else {
    handleInsertUserPhotosInfo();
  }
  return 0;
}
