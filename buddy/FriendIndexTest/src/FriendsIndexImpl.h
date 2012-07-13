/*
 * =====================================================================================
 *
 *       Filename:  FriendsIndexImpl.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年08月18日 18时45分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_INDEX_IMPL_H__
#define __FRIENDS_INDEX_IMPL_H__

#include "OceCommon/src/FriendsIndex.h"
#if __GNUC__>2
#include <ext/hash_set>
using namespace __gnu_cxx;
#else
#include <hash_set>
using namespace stdext;
#endif

class FriendsIndexImpl : public xce::buddy::FriendsIndex {
public:
	FriendsIndexImpl(const hash_set<int>& set, bool useOptimalPath = false) : xce::buddy::FriendsIndex(set, useOptimalPath) {
	};	

	FriendsIndexImpl() : xce::buddy::FriendsIndex() {
		test = new xce::buddy::AsynIndexData();
	};	
	virtual ~FriendsIndexImpl() {}

	virtual xce::buddy::BuddyDataPtr locateFriend(Ice::Int id) {
		xce::buddy::BuddyDataPtr demo = new xce::buddy::BuddyData();
		/*
		for (int i = 10; i< 110; i++) {
			demo->friends.push_back(i);
		}
		*/
		
		switch(id) {
			case 1:
				demo->friends.push_back(2);
				demo->friends.push_back(5);
				break;
			/*
			case 2:
				demo->friends.push_back(3);
				demo->friends.push_back(1);
				break;
			case 3:
				demo->friends.push_back(4);
				demo->friends.push_back(2);
				break;
			case 4:
				demo->friends.push_back(5);
				demo->friends.push_back(3);
				break;
			*/
			default:
				break;

		}
		return demo;
	}

	virtual xce::buddy::BuddyDataPtr locateJoinFriend(Ice::Int id) {
		xce::buddy::BuddyDataPtr demo = new xce::buddy::BuddyData();
		/*
		for (int i = 10; i< 110; i++) {
			demo->friends.push_back(i);
		}
		*/
		
		switch(id) {
			case 1:
				demo->friends.push_back(5);
				break;
			case 2:
				demo->friends.push_back(7);
				break;
			case 3:
				demo->friends.push_back(5);
				demo->friends.push_back(6);
				break;
			case 4:
				demo->friends.push_back(7);
				break;
			default:
				break;

		}
		return demo;
	}
	
	virtual xce::buddy::AsynIndexDataPtr locateAsynIndex(Ice::Int id) {
		return test;
	}

	virtual MyUtil::ObjectResultPtr locateFriends(const MyUtil::LongSeq& ids) {
		MyUtil::ObjectResultPtr demo = new MyUtil::ObjectResult();
		for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
			xce::buddy::BuddyDataPtr data = new xce::buddy::BuddyData();
			/*
			for (int i = 1; i< 100; i++) {
				data->friends.push_back(i);
			}
			*/
			
			switch(*it) {
				case 2:
					data->friends.push_back(3);
					data->friends.push_back(4);
					data->friends.push_back(7);
					break;
				/*
				case 3:
					data->friends.push_back(2);
					data->friends.push_back(4);
					break;
				*/
				default:
					break;

			}
			demo->data.insert(std::make_pair((*it), data));
		}
		return demo;
	}
private:
	xce::buddy::AsynIndexDataPtr test;

};

typedef IceUtil::Handle<FriendsIndexImpl> FriendsIndexImplPtr;

#endif

