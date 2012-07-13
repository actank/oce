/*
 * Common.h
 *
 *  Created on: Jan 14, 2011
 *      Author: yejingwei
 */

#ifndef COMMON_H_
#define COMMON_H_
//#include "RFeed.h"
#include <vector>
#include <string>
#include "RFeed.h"
#include "QueryRunner.h"
using namespace std;
using namespace com::xiaonei::xce;

/*class FeedConfig;
 class ReplyData;
 class FeedReply;
 typedef IceUtil::Handle<FeedConfig> FeedConfigPtr;
 typedef IceUtil::Handle<FeedReply> FeedReplyPtr;
 typedef IceUtil::Handle<ReplyData> ReplyDataPtr;*/

namespace xce {
namespace feed {
class SqlConverter: public MyUtil::Singleton<SqlConverter> {
public:
  string Convert(const Statement & statement) {
    IceUtil::Mutex::Lock lock(query_mutex_);
    statement.query(*query_);
    string sqlstr = query_->str();
    query_->reset();
    return sqlstr;
  }
  bool Init() {
    query_ready_ = false;
    vector<pair<string, string> > v;
    v.push_back(make_pair("feed_db", "feed_content_0"));
    v.push_back(make_pair("feed_db", "feed_index_0"));
    v.push_back(make_pair("feed_db", "feed_mini_0"));
    for (unsigned i = 0; i != v.size(); ++i) {
      try {
        ConnectionHolder conn(v.at(i).first, 3, v.at(i).second);//TODO 有么有更好的办法
        query_ = new mysqlpp::Query(conn.query());
        query_ready_ = true;
        break;
      } catch (Ice::Exception& e) {
        MCE_WARN("DbHelper::Init --> err:" << e);
      } catch (std::exception& e) {
        MCE_WARN("DbHelper::Init --> err:" << e.what());
      }
    }
    MCE_INFO("DbHelper::Init. query_ready_" << query_ready_);
    return query_ready_;
  }
  ;
private:
  mysqlpp::Query *query_;
  IceUtil::Mutex query_mutex_;
  bool query_ready_;
};
}
}
#endif /* COMMON_H_ */
