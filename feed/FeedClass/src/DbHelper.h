/*
 * UserDb.h
 *
 *  Created on: Mar 10, 2011
 *      Author: yejingwei
 */

#ifndef CLASSDB_H_
#define CLASSDB_H_
#include <vector>
#include "Singleton.h"

using namespace MyUtil;
using namespace std;

namespace xce {
namespace feed {


class GroupDb: public Singleton<GroupDb> {
public:
  bool GetMembers(int gid, vector<int>& mids);
  string GetMinigroupTableName(int gid) ;

};
}
}
#endif /* CLASSDB_H_ */
