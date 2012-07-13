/*
 * Util.cpp
 *
 *  Created on: 2011-6-22
 *      Author: sunzz
 */
#include "AdQualityUtil.h"
namespace xce{
namespace ad{
using namespace std;
using namespace MyUtil;
Ice::Long TimeForm2Long(string time){
  MyUtil::Date t(time);
  return (((t.year() * 100 + t.month()) * 100 + t.day()) * 100 + t.hour());
}


string GetKey(Ice::Long zone_id, int pos) {
  Ice::Long key = zone_id * 10 + pos;
  ostringstream sin;
  sin << key;
  return sin.str();
}

Ice::Long GetTime(int year, int mon, int day, int hour) {
  return (((year * 100 + mon) * 100 + day) * 100 + hour);
}
Ice::Long GetTime(MyUtil::Date & day){
  return (((day.year() * 100 + day.month()) * 100 + day.day()) * 100 + day.hour());
}
Ice::Long GetDay(int year, int mon, int day) {
  return (year * 100 + mon) * 100 + day;
}
Ice::Long GetDay(MyUtil::Date & day){
  return (day.year() * 100 + day.month()) * 100 + day.day();
}

string TransformForSql(set<Ice::Long>& gids) {
  ostringstream result;
  if (gids.empty()) {
    return result.str();
  }
  for(set<Ice::Long>::iterator itg = gids.begin(); itg!= gids.end(); ++itg) {
    if(itg == gids.begin()) {
      result << " ( " << *itg;
    }else {
      result <<" , " << *itg;
    }
  }
  result << " ) ";
  return result.str();
}
}
}
