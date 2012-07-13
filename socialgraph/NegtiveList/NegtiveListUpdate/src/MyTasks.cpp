#include "MyTasks.h"

using namespace tasks;
using namespace MyUtil;
using namespace xce::socialgraph;

void AppendSingleRelationAndSetCacheTask::handle() {
	IntSeq resultList;
	HotFeedSeq feedSeq;

    //try {
	//	feedSeq = HotFeedRecommendCacheAdapter::instance().Get(_ownerId, -1);
    //} catch (Ice::Exception& e) {
    //	cout << "AppendSingleRelationAndSetCacheTask HotFeedRecommendCacheAdapter Ice exception " << e.what() << endl;
    //} catch (std::exception& e) {
    //    cout << "AppendSingleRelationAndSetCacheTask HotFeedRecommendCacheAdapter std exception " << e.what() << endl;
    //} catch (...) {
    //    cout << "AppendSingleRelationAndSetCacheTask HotFeedRecommendCacheAdapter unknow exception" << endl;
    //}
	cout << "feedSeq size = " << feedSeq.size() << endl;

	for (HotFeedSeq::iterator iter = feedSeq.begin();
			iter != feedSeq.end(); ++iter) {
		resultList.push_back(iter->userId);
	}

	//resultList.push_back(285340707);
	//resultList.push_back(1331);
	//resultList.push_back(2055206602);

	set<int> checkSet(resultList.begin(), resultList.end());

	//cout << "AppendSingleRelationAndSetCacheTask" << endl;
	//for (vector<int>::iterator iter = _negtiveFriendList.begin();
	//		iter != _negtiveFriendList.end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;


	for (vector<int>::iterator iter = _negtiveFriendList.begin();
			iter != _negtiveFriendList.end(); ++iter) {
		if (checkSet.find(*iter) == checkSet.end()) {
			resultList.push_back(*iter);
		}
	}
	
	//cout << "iijiojonojh fuck you" << endl;
	//for (vector<int>::iterator iter = resultList.begin();
	//		iter != resultList.end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;
	cout << "AppendSingleRelationAndSetCacheTask " << _ownerId << " : " << resultList.size() << endl;

    try {
		NegtiveListCacheAdapter::instance().Set(_ownerId, resultList);
    } catch (Ice::Exception& e) {
    	cout << "AppendSingleRelationAndSetCacheTask NegtiveListCacheAdapter Ice exception " << e.what() << endl;
    } catch (std::exception& e) {
        cout << "AppendSingleRelationAndSetCacheTask NegtiveListCacheAdapter std exception " << e.what() << endl;
    } catch (...) {
        cout << "AppendSingleRelationAndSetCacheTask NegtiveListCacheAdapter unknow exception" << endl;
    }
}
