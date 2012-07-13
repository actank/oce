/*
 * Util.h
 *
 *  Created on: 2011-6-22
 *      Author: sunzz
 */

#ifndef AD_QUALITY_UTIL_H_
#define AD_QUALITY_UTIL_H_

#include <Ice/Ice.h>
#include "Date.h"
#include "Singleton.h"
#include <string>
#include <set>
namespace xce{
namespace ad{
using namespace std;
using namespace MyUtil;
using namespace std;
const string pos_721 = "1000000000721";
const string pos_722 = "1000000000722";
const string pos_723 = "1000000000723";
const string pos_701 = "1000000000701";
const string pos_702 = "1000000000702";
const string pos_703 = "1000000000703";
const string pos_631 = "1000000000631";
const string pos_632 = "1000000000632";
const string pos_633 = "1000000000633";
const string pos_751 = "1000000000751";
const string pos_752 = "1000000000752";
const string pos_753 = "1000000000753";
const string pos_011 = "1000000000011";
const string pos_012 = "1000000000012";
const string pos_013 = "1000000000013";
Ice::Long TimeForm2Long(string time);
string GetKey(Ice::Long zone_id, int pos);
Ice::Long GetTime(int year, int mon, int day, int hour);
Ice::Long GetTime(MyUtil::Date & day);
Ice::Long GetDay(int year, int mon, int day);
Ice::Long GetDay(MyUtil::Date & day);
string TransformForSql(set<Ice::Long>& gids);
}
}
#endif /* UTIL_H_ */
