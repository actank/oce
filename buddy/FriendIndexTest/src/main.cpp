/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月08日 12时01分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FriendsIndexImpl.h"
#include <iostream>
#if __GNUC__>2
#include <ext/hash_set>
using namespace __gnu_cxx;
#else
#include <hash_set>
using namespace stdext;
#endif

using namespace MyUtil;
using namespace std;
using namespace xce::buddy;

int main(int argc, char* argv[]) {

	//int array[8] = {1, 2, 3, 4, 5, 9, 7, 8};
	int array[1] = {4};
	const size_t array_size=sizeof(array)/sizeof(array[0]);
	hash_set<int> joinSet(array, array+array_size);

	//FriendsIndexPtr fi = new FriendsIndexImpl(joinSet, false);
	//IndexDataPtr index = fi->buildIndex(1);

	FriendsIndexPtr fi = new FriendsIndexImpl();
	fi->asynBuildIndex(1);

	IndexInfoSeq indexs = fi->locateAsynIndex(1)->getFreshers();
	cout << "indexs.size(): " << indexs.size() << endl;
	IndexInfoSeq::const_iterator b = indexs.begin();
	for(; b != indexs.end(); ++b)
        { 
		cout << "id: " << (*b)->id << " degree: " << (*b)->degree << endl;
		for (MyUtil::IntSeq::iterator it = (*b)->friends.begin();
				it != (*b)->friends.end(); ++it) {
			cout<< " => " << (*it) ;	
		}
		cout << endl;
        }

}

