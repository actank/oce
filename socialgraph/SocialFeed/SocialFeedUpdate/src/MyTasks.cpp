#include "MyTasks.h"
//using namespace xce::offerfriends::adapter;
//using namespace xce::offerfriends;
using namespace xce::socialgraph;

int CreateSocailFeedTask::SetSocialFeedCache(int uid, const vector<SocialFeed>& feeds) 
{
	const size_t MAX_FEED_NUM = 2;
	vector<Feed> feedlist;
	feedlist.reserve(MAX_FEED_NUM);
	for (vector<SocialFeed>::const_iterator iter = feeds.begin();
			iter != feeds.end(); ++iter)
	{
		Feed feed;
		iter->Transfer(feed);

		feedlist.push_back(feed);
		if (feedlist.size() >= MAX_FEED_NUM) {
	    	break;
		}
	}

	try {
		SocialFeedCacheAdapter::instance().set(uid, feedlist);
	} catch (Ice::Exception& e) {
		cout << "CreateSocailFeedTask SetSocialFeedCache ice exception " << e.what() << endl;
	} catch (std::exception& e) {
		cout << "CreateSocailFeedTask SetSocialFeedCache std exception " << e.what() << endl;
	} catch (...) {
		cout << "CreateSocailFeedTask SetSocialFeedCache unknow exception" << endl;
	}
	
	return 0;
} 

int CreateSocailFeedTask::CreateSocailFeed(vector<SocialFeed>& feeds, 
		USERID uid, const vector<USERID>& changeFriends,
		const map<USERID,set<USERID> >& secondLayer)
{
	map<USERID,set<USERID> >::const_iterator secondIter;
	map<USERID,vector<USERID> > degree2; // map<3_layer_user, vector<my friends> >
	
	//struct timeval start[6];

	// 计算二度好友（此时包含好友）排序
	//gettimeofday(&start[0], NULL);
	for (vector<USERID>::const_iterator cfIter = changeFriends.begin();
			cfIter != changeFriends.end(); ++cfIter)
	{
		secondIter = secondLayer.find(*cfIter);
		assert(secondIter != secondLayer.end());
		for (set<USERID>::const_iterator thirdIter = secondIter->second.begin();
				thirdIter != secondIter->second.end(); ++thirdIter)
		{
			map<USERID,vector<USERID> >::iterator iter = degree2.find(*thirdIter);
			if (iter == degree2.end())
			{
				pair<map<USERID,vector<USERID> >::iterator, bool> result =
					degree2.insert(make_pair<USERID,vector<USERID> >(*thirdIter, vector<USERID>()));
				iter = result.first;
			}
			iter->second.push_back(*cfIter);
		}
	}

	// 过滤好友及自己，合并
	//gettimeofday(&start[1], NULL);
	vector<USERID> friendlist;
	friendFetcher->Read(uid, friendlist);
	set<USERID> setFriends(friendlist.begin(), friendlist.end());

	//gettimeofday(&start[2], NULL);
	map<vector<USERID>,vector<USERID> > mapfeeds;
	for (map<USERID,vector<USERID> >::const_iterator iter = degree2.begin();
			iter != degree2.end(); ++iter)
	{
		if (iter->first == uid || setFriends.find(iter->first) != setFriends.end())
			continue;

		map<vector<USERID>,vector<USERID> >::iterator revIter = mapfeeds.find(iter->second);
		if (revIter == mapfeeds.end())
		{
			pair<map<vector<USERID>,vector<USERID> >::iterator, bool> result =
				mapfeeds.insert(make_pair<vector<USERID>,vector<USERID> >(
							iter->second, vector<USERID>()));
			revIter = result.first;
		}
		revIter->second.push_back(iter->first);
	}

	// 去除block feed, 按照offerfriend排序, 计算TopN
	//gettimeofday(&start[3], NULL);
	set<long> blockfeed;
	socialFeedBlockFetcher->Read(uid, blockfeed);
	//fprintf(stderr, "%d: ", uid);
	//for (set<long>::const_iterator iter = blockfeed.begin();
	//		iter != blockfeed.end(); ++iter)
	//{
	//	fprintf(stderr, "%ld,", *iter);
	//}
	//fprintf(stderr, "\n");
	
	//gettimeofday(&start[4], NULL);
	map<int,float> offerfriend_sort;
	GetOfferFriendSort(uid, 100, offerfriend_sort);

	//gettimeofday(&start[5], NULL);
	for (map<vector<USERID>,vector<USERID> >::const_iterator iter = mapfeeds.begin();
			iter != mapfeeds.end(); ++iter)
	{
		SocialFeed feed(iter->first, iter->second, offerfriend_sort);
		long finger = feed.Finger();
		//fprintf(stderr, "<<%ld>>", finger);
		if (blockfeed.find(finger) != blockfeed.end())
			continue;
		feeds.push_back(feed);
	}
	sort(feeds.begin(), feeds.end(), SocialFeedComp());

	//for (int i=0; i<5; ++i)
	//{
	//	fprintf(stdout, "[%d:%ld]", i,
	//			(start[i+1].tv_sec-start[i].tv_sec)*1000000+start[i+1].tv_usec-start[i].tv_usec);
	//}
	//fprintf(stderr, "\n");
	return 0;
} 

int CreateSocailFeedTask::GetOfferFriendSort(int userId, int limit, map<int,float>& offerfriend_sort) 
{
	//vector<Offer> list;
	//try {
	//	//list = OfferFriendsReplicaAdapter::instance().getOffer(userId, limit);
	//}catch( Ice::Exception e){
	//	cout << "[CreateSocailFeedTask] GetOfferFriendSort Ice-> " << e.what() << endl;
	//}catch( std::exception e){
	//	cout << "[CreateSocailFeedTask] GetOfferFriendSort std-> " << e.what() << endl;
	//}catch( ... ){
	//	cout << "[CreateSocailFeedTask] GetOfferFriendSort unknown " << endl;
	//}

	//for (vector<Offer>::const_iterator iter = list.begin();
	//		iter != list.end(); ++iter)
	//{
	//	offerfriend_sort.insert(make_pair<int,float>(iter->userId, iter->weight));
	//}
	return 0;
} 

