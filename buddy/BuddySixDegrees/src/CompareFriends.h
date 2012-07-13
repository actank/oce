#ifndef __BUDDY_SIXDEGREES_COMPARE_I_H__
#define __BUDDY_SIXDEGREES_COMPARE_I_H__
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "../friendindex/FriendsIndex.h"
#include <tcrdb.h>
#include <iostream>
#include <set>
#include "util/cpp/TimeCost.h"

namespace xce {
namespace buddy {
using namespace MyUtil;

/**
 *比较两个人共同好友类
 */
class FriendsCompare
{
public:
	FriendsCompare() {};
	virtual ~FriendsCompare() {};

	/**
	 * 平凡遍历算法
	 * @param id1 id2 两个用户Id
	 * @param friendsA friendsB 两个用户对应好友
	 */
	std::vector<std::vector<int> >   hasCommonFriend( int id1, int id2 ,vector<int>& friendsA, vector<int>& friendsB){

		if (friendsA.empty() || friendsB.empty()) {
			return routList;
		}

		for(vector<int>::iterator it =  friendsA.begin(); it!=friendsA.end();++it)
		{
			vector<int>::iterator iaEnd;
			iaEnd = lower_bound(friendsB.begin(),friendsB.end(),*it);
			if (iaEnd != friendsB.end() && *iaEnd == *it)
			{
				std::vector<int> rout;
				rout.push_back(id1);
				rout.push_back( *iaEnd );
				rout.push_back(id2);
				routList.push_back(rout);
			}
		}

		return routList;
	}

	/**
	 *加速的双向查找算法
	 * @param id1 id2 两个用户Id
	 * @param ndwFriendsA id1对应新好友
	 * @param allFriendsB id2对应全部好友
	 */
	std::vector<std::vector<int> >   hasCommonFriendNew( int id1, int id2 ,vector<int>& newFriendsA, vector<int>& allFriendsB)
	{
		//双向查找
		 vector<int>::iterator iaEnd = newFriendsA.end();
		 vector<int>::iterator ibEnd = allFriendsB.end();
		 if (newFriendsA.back() > allFriendsB.back() ) {
			 iaEnd = lower_bound(newFriendsA.begin(),newFriendsA.end(),allFriendsB.back());
			 if (iaEnd != newFriendsA.end() && (*iaEnd) == (allFriendsB.back())) {
				 routListAdd(id1 , id2 , (*iaEnd)  );  
			 }
		 } else {
			 ibEnd = lower_bound(allFriendsB.begin(),allFriendsB.end(),newFriendsA.back());
			 if (ibEnd != allFriendsB.end() && (*ibEnd) == (newFriendsA.back())) {
				 routListAdd(id1 , id2 , *ibEnd );  
			 }
		 }

		 //计算剩余部分
		 vector<int>::iterator ia = newFriendsA.begin(); //lower_bound(listA.begin(),iaEnd,listB.front());
		 vector<int>::iterator ib = allFriendsB.begin();
		 while(ia != iaEnd){
			 ib = lower_bound(ib,ibEnd,*ia);
			 if(ib != ibEnd && (*ib)== (*ia)){
				 routListAdd(id1 , id2 , (*ia)  );
				 ia++;
				 ib++; 
				 continue ;
			 } else if ( ib == ibEnd  ){
				 return routList;
			 }
			 ia = lower_bound(ia,iaEnd,*ib);
			 if( ia != iaEnd && (*ia) == (*ib)){
				 routListAdd(id1 , id2 , (*ia)  );
				 ia++;
				 ib++;
			 }
		 }
		 return routList;
        }
private:

	std::vector<std::vector<int> >  routList;
	void routListAdd(int id1 , int id2 , int id)
	{
		std::vector<int> rout;
		rout.push_back(id1);
		rout.push_back( id );
		rout.push_back(id2);
		routList.push_back(rout);

	}

};

typedef IceUtil::Handle<FriendsCompare> FriendsComparePtr;

//************************************************************************************//
//
/**
 *计算六度空间关系类
 */
class FriendsCompareSixDegreesNew
{
public:
	FriendsCompareSixDegreesNew() {};
	virtual ~FriendsCompareSixDegreesNew() {};

	std::vector<std::vector<int> >   hasCommonFriendNew( int id1, int id2 ,
			vector<int>& hostFriends , vector<int>& guestFriends,
			vector<IndexInfoIPtr>& newFriendsA,vector<IndexInfoIPtr>& newFriendsB,
			vector<IndexInfoIPtr>& allFriendsA, vector<IndexInfoIPtr>& allFriendsB ){

		ostringstream os;
		os<<"FriendsCompareSixDegreesNew():hasCommonFriend " ;
		os<< "id1:" <<id1 << "id2:" <<id2 ;
		TimeCost tc = TimeCost::create(os.str() , TimeCost::LOG_LEVEL_INFO);

		if (allFriendsA.empty() || allFriendsB.empty() || newFriendsA.empty() ||  newFriendsB.empty()) {
			return routList;
		}

		comparePrivate(id1, id2 , hostFriends , guestFriends, newFriendsA , allFriendsB ) ;
		tc.step("caculate newA index");

		comparePrivate(id1, id2 ,hostFriends , guestFriends, allFriendsA , newFriendsB ) ;
		tc.step("caculate newB index");

		return routList;
	}



private:
	std::vector<std::vector<int> >  routList;

	void routListAdd(int id1 , int id2 , vector<int>& hostFriends , vector<int>& guestFriends, IndexInfoIPtr data1 , IndexInfoIPtr data2 )
	{
	        	
		std::vector<int> rout;
		rout.push_back(id1);
		for( vector<int>::iterator it1= data1->friends.begin(); it1!=data1->friends.end() ; ++it1 )
		{
			vector<int>::iterator  it= lower_bound( guestFriends.begin(), guestFriends.end(), *it1 ) ;
			if(it!=guestFriends.end() && (*it1) == (*it))
			{
				MCE_INFO("FriendsCompareSixDegreesNew::routListAdd the friend : it1" << *it1 << "in the " << *it << "friends ")
				return;
			}

			rout.push_back(*it1);
		}


		vector<int>::iterator  it_g= lower_bound( guestFriends.begin(), guestFriends.end(), data1->id ) ;
		if(it_g!=guestFriends.end() && (data1->id) == (*it_g))
		{
			MCE_INFO("FriendsCompareSixDegreesNew::routListAdd the friend : Node " << "in the " << *(it_g) << "friends ")
			return;
		}


		vector<int>::iterator  it_h= lower_bound( hostFriends.begin(), hostFriends.end(), data1->id ) ;
		if(it_h!=hostFriends.end() && (data1->id) == (*(it_h)))
		{
			MCE_INFO("FriendsCompareSixDegreesNew::routListAdd the friend : Node"  << "in the " << *(it_h) << "friends ")
			return;
		}

		rout.push_back(data1->id);
		if(data2->friends.size()>0)
		{
			for( vector<int>::iterator it2= data2->friends.end()-1; ; --it2 )
			{
				vector<int>::iterator  it= lower_bound( hostFriends.begin(), hostFriends.end(), *it2 ) ;
				if(it!=hostFriends.end() && (*it2) == (*it))
				{
					MCE_INFO("FriendsCompareSixDegreesNew::routListAdd the friend : it2" << *it2 << "in the " << *it << "friends ")
					return;
				}

				rout.push_back(*it2);
				if( it2==data2->friends.begin() )
					break;
			}
		}

		rout.push_back(id2);
		if(checkSameNode(rout)==true)
		{
			MCE_INFO("FriendsCompareSixDegreesNew.checkSameNode == true")
				return;
		}
		routList.push_back(rout);

	}
    
	bool checkSameNode(std::vector<int> rout)
	{
		for( vector<int>::iterator it1= rout.begin(); it1!= rout.end();++it1)
		{
			for( vector<int>::iterator it2=it1+1; it2!= rout.end();++it2)
			{
				if( *it1 == *it2 )
					return true;
			}

		}

		return  false;
	}

	bool checkSameNode(int id1 , int id2 , IndexInfoIPtr data1 , IndexInfoIPtr data2 )
	{
		for( vector<int>::iterator it1= data1->friends.begin(); it1!= data1->friends.end();++it1)
		{
			if(id2 == *it1)
				return true;
			for( vector<int>::iterator it2= data2->friends.begin(); it2!= data2->friends.end();++it2)
			{
				if(id1 == *it2) 
					return true;
				if( *it1 == *it2 )
					return true;
			}

		}

		return  false;
	}

	void comparePrivate(int id1, int id2 ,vector<int>& hostFriends , vector<int>& guestFriends, vector<IndexInfoIPtr>& newFriendsA ,vector<IndexInfoIPtr>& allFriendsB )
	{
		vector<IndexInfoIPtr>::iterator iaEnd = newFriendsA.end();
		vector<IndexInfoIPtr>::iterator ibEnd = allFriendsB.end();
		if (newFriendsA.back()->id > allFriendsB.back()->id ) {
			iaEnd = lower_bound(newFriendsA.begin(),newFriendsA.end(),allFriendsB.back(),less_entry());
			if (iaEnd != newFriendsA.end() && (*iaEnd)->id == (allFriendsB.back())->id) {
				routListAdd(id1 , id2 ,hostFriends , guestFriends, (*iaEnd) , allFriendsB.back() );  
			}
		} else {
			ibEnd = lower_bound(allFriendsB.begin(),allFriendsB.end(),newFriendsA.back(),less_entry());
			if (ibEnd != allFriendsB.end() && (*ibEnd)->id == (newFriendsA.back())->id) {
				routListAdd(id1 , id2 , hostFriends , guestFriends, newFriendsA.back() , *ibEnd );  
			}
		}

		vector<IndexInfoIPtr>::iterator ia = newFriendsA.begin(); //lower_bound(listA.begin(),iaEnd,listB.front());
		vector<IndexInfoIPtr>::iterator ib = allFriendsB.begin();
		while(ia != iaEnd){
			ib = lower_bound(ib,ibEnd,*ia,less_entry());
			if(ib != ibEnd && (*ib)->id == (*ia)->id){
				routListAdd(id1 , id2 ,hostFriends , guestFriends, (*ia) , (*ib) );
				ia++;
				ib++; 
				continue ;
			} else if ( ib == ibEnd  ){
				return ;
			}
			ia = lower_bound(ia,iaEnd,*ib,less_entry());
			if( ia != iaEnd && (*ia)->id == (*ib)->id){
				routListAdd(id1 , id2 ,hostFriends , guestFriends, (*ia) , (*ib) );
				ia++;
				ib++;
			}
		}
	}

};

typedef IceUtil::Handle<FriendsCompareSixDegreesNew> FriendsCompareSixDegreesNewPtr;

}
}

#endif

