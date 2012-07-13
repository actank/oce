/*
 * Restore.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: yejingwei
 */

#include "Restore.h"
#include <fstream>
#include <iostream>
#include <istream>
#include <dirent.h>

#include <cstdlib>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "MyQueue.h"
#include "LogWrapper.h"
#include "FeedQueueI.h"
#include "Date.h"
#include "Common.h"

#include <fstream>
#include <algorithm>
#include <iterator>

using namespace xce::feed;
using namespace boost;
using namespace MyUtil;
using namespace std;
using MyUtil::Date;
//using namespace com::xiaonei::xce;

void Restorer::Init() {
  qm_ = new QueueManager();
  qm_->Init(1, 1000);
  ready_ = true;
}

void Restorer::Restore(const string & begintime, const string & endtime,
    int thishour) {
  if (!ready_) {
    MCE_INFO("Restorer::Restore. not ready");
    return;
  }

  ready_ = false;
  try {
    MCE_INFO("Restorer::Restore. begin:" << begintime << " end:" << endtime);
    if (SelectLogs(begintime, endtime, thishour)) {
      ParseOuttags();
      ParseIntags();
      MCE_INFO("Restorer::Restore. suc_ids size:" << outtag_ids_.size() << " " << Print(outtag_ids_));
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("Restorer::Restore: error" << e );
  } catch (std::exception& e) {
    MCE_WARN("Restorer::Restore:" << e.what());
  }
  Clear();
  ready_ = true;
}
void Restorer::Clear() {
  inlogs_.clear();
  outlogs_.clear();
  outtag_ids_.clear();
}

//输入时间格式　"%Y-%m-%d %H:%M:%S"
//文件名格式　"oce_log.2011-02-19-21"
bool Restorer::SelectLogs(const string & begintime, const string & endtime,int this_hour) {
  MyUtil::Date OneHour(60 * 60);

  MyUtil::Date begin, end;
  if (begintime.empty()) {//输入为空的，只恢复前一小时当前的
    MCE_INFO("empty");
    long t = time(NULL);
    end = MyUtil::Date::seconds(t);
    begin = end - OneHour;
  } else {
    MCE_INFO("no empty");
    begin = MyUtil::Date::datetime(begintime);
    end = MyUtil::Date::datetime(endtime);
  }
  if (begin > end) {
    MCE_WARN("Restorer::SelectLogs. begin larger than end.begin:" << begintime << " end:" <<endtime);
    return false;
  }
  MCE_INFO("begin:" << begin << " end:" << end);
  while (begin < end + OneHour) {
    string filename = DateToLogName(begin);
    MCE_INFO("Restorer::SelectLogs. inlog filename:" << path_ + filename);
    inlogs_.push_back(path_ + filename);
    if (inlogs_.size() > (unsigned)MAX_LOG) {
      MCE_WARN("Restorer::SelectLogs. to much logs. size:" <<inlogs_.size());
      return false;
    }
    begin += OneHour;
  }
  if (this_hour) {
    if (CopyOceLog()) {
      MCE_INFO("Restorer::CopyOceLog. copy success");
      inlogs_.push_back(path_ + "oce_log.bak");//最新的log
    } else {
      MCE_INFO("Restorer::CopyOceLog. copy fail");
    }
  }
  outlogs_ = inlogs_;
  return true;
}

bool Restorer::CopyOceLog() {
  string orig = path_ + "oce_log";
  string bak = path_ + "oce_log.bak";
  string cmd = "cp " + orig + " " + bak;
  int i = system(cmd.c_str());
  return i == 0;
}

string Restorer::DateToLogName(MyUtil::Date & date) {
  ostringstream oss;
  oss << "oce_log." << date.year() << "-" << AddZero(date.month()) << "-"
      << AddZero(date.day()) << "-" << AddZero(date.hour());
  return oss.str();
}

void Restorer::SetServiceIndex(int i) {
  ostringstream oss;
  if (opendir("/data/xce/FeedTest/")) {
    oss << "/data/xce/FeedTest/log/FeedQueue";//TODO
  } else {
    oss << "/data/xce/XFeed/log/FeedQueue";//TODO
  }
  oss << i << "/";
  path_ = oss.str();
  MCE_INFO("Restorer::SetServiceIndex. index:" << i << " path:" << path_);
}

string Restorer::AddZero(int i) {
  string s;
  if (i < 10) {
    s = "0";
  }
  s += lexical_cast<string> (i);
  return s;
}

void Restorer::ParseIntags() {
  for (unsigned i = 0; i != inlogs_.size(); ++i) {
    ParseIntagsFromFile(inlogs_.at(i));
  }
  MCE_INFO("Restorer::ParseIntags. done. outtag_ids_ size:" << outtag_ids_.size());
}

void Restorer::ParseOuttags() {
  for (unsigned i = 0; i != outlogs_.size(); ++i) {
    ParseOuttagsFromFile(outlogs_.at(i));
  }
  MCE_INFO("Restorer::ParseOuttags. done.outtag_ids_ size:" << outtag_ids_.size());
}

void Restorer::ParseOuttagsFromFile(string & name) {
  string s;
  ifstream file(name.c_str());

  if (file.good()) {
    MCE_INFO("Restorer::ParseOuttagsFromFile. open file good, name:" << name);

  } else {
    MCE_INFO("Restorer::ParseOuttagsFromFile. open file fail, name:" << name);
    return;
  }

  int suc_count = 0;
  while (getline(file, s)) {
    size_t begin = 0;
    size_t found;
    Ice::Long id;
    found = s.find(kOutTag, begin);
    if (found == string::npos) {
      continue;
    }
    if (found > 30) {
      MCE_INFO(__LINE__ << ",ParseOuttagsFromFile,found > 30, found:" << found);
      continue;
    }
    try {
      begin = found + 1;
      found = s.find('\t', begin);
      string idstr = s.substr(found + 1);
      id = lexical_cast<Ice::Long> (idstr.c_str());
      outtag_ids_.insert(id);
      ++suc_count;
    } catch (Ice::Exception& e) {
      MCE_WARN("Restorer::ParseOuttagsFromFile:" << e << ",str:" << s);
      continue;
    } catch (std::exception& e) {
      MCE_WARN("Restorer::ParseOuttagsFromFile:" << e.what()<< ",str:" << s);
      continue;
    }
  }
  file.close();
  MCE_INFO("Restorer::ParseOuttagsFromFile. parse file:" << name << " suc_count:"
      << suc_count << " total_count:" << outtag_ids_.size());
}

void Restorer::ParseIntagsFromFile(string & name) {

  string s;
  ifstream file(name.c_str());

  if (file.good()) {
    MCE_INFO("Restorer::ParseOuttagsFromFile. open file good,name:" << name);
  } else {
    MCE_INFO("Restorer::ParseOuttagsFromFile. open file fail, name:" << name);
    return;
  }
  while (getline(file, s)) {
    string id, instance, wrflag, pattern, statement;
    size_t found = 0, begin = 0;
    found = s.find(kInTag, begin);
    if (found == string::npos) {//##PUSH##位置比较靠前
      continue;
    }
    if (found > 30) {
      continue;
    }
    UnitPtr unit;
    try {
      unit = ExtractUnitFromLine(s);
    } catch (Ice::Exception& e) {
      MCE_WARN("Restorer::ParseIntagsFromFile:" << e);
      continue;
    } catch (std::exception& e) {
      MCE_WARN("Restorer::ParseIntagsFromFile:" << e.what());
      continue;
    }

    if (unit) {
      //      MCE_INFO("Restorer::ParseIntagsFromFile. Parse one line:" << unit->DumpToString());
      MCE_INFO("Restorer::ParseIntagsFromFile. Push one Unit");
      qm_->PushUnit(unit);
    } else {
      //      MCE_INFO("Restorer::ParseIntagsFromFile. ExtractUnitFromLine error. str:" << s );
    }
  }
  file.close();
}

UnitPtr Restorer::ExtractUnitFromLine(string & str) {
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("\t");
  tokenizer tokens(str, sep);

  int size = 0;
  for (tokenizer::iterator tit = tokens.begin(); tit != tokens.end(); ++tit) {
    ++size;
  }

  if (size < 6) {//至少有六段
    MCE_INFO("Restorer::ExtractUnitFromLine. wrong size:" << size << " str:" << str);
    return NULL;
  }

  tokenizer::iterator itr = tokens.begin();
  ++itr;
  Ice::Long id = lexical_cast<Ice::Long> (*itr++);
  set<Ice::Long>::iterator it = outtag_ids_.find(id);
  if (it != outtag_ids_.end()) {
    MCE_INFO("Restorer::ExtractUnitFromLine. matched. id:" << id);
    return NULL;
  }
  //  if (it != outtag_ids_.end()) {
  //    --(it->second);
  //    if (it->second == 0) {
  //      outtag_ids_.erase(it);
  //    }
  //    MCE_INFO("Restorer::ExtractUnitFromLine. matched. id:" << id);
  //    return NULL;
  //  }
  int retry = lexical_cast<int> (*itr++);
  string instance = lexical_cast<string> (*itr++);
  int wrflag = lexical_cast<int> (*itr++);
  string pattern = lexical_cast<string> (*itr++);
  string statement;
  for (; itr != tokens.end(); ++itr) {
    statement += lexical_cast<string> (*itr);
    statement += "\t";
  }

  UnitPtr unit = new Unit(id, retry + 1, instance, wrflag, pattern, statement);
  if (retry > MAX_RETRY) {
    MCE_INFO("Restorer::ExtractUnitFromLine. retry exceed max_retry. " << unit->DumpToString());
    return NULL;
  }
  return unit;
}

string Restorer::DumpToString() {
  ostringstream oss;
  oss << qm_->DumpToString() << " outtag_ids:" << outtag_ids_.size();
  return oss.str();
}
