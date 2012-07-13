#ifndef GROUPLABEL_GROUPLABEL_H
#define GROUPLABEL_GROUPLABEL_H
#include "StringCountMap.h"
#include "StringInt.h"
#include "GroupCountElement.h"
#include <string>
#include "GroupCountMap.h"
#include "NetworkCount.h"
#include "UserBasicCount.h"
#include "ExistingLabelCount.h"
class GroupLabel{
	public:
	static void dispRes(vector<StringInt> &res, const char *s); 
	static void networkLabel(const vector<int> &userlist, StringInt & res, int host);
	static void userBasicLabel(const vector<int> &userlist, StringInt & res);
	static void existingLabel(const vector<int> &userlist, int host, StringInt & res);
	static void Label(const vector<int> &userlist, int host, vector<StringInt> & res);
};
#endif
