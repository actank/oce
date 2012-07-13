/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "Db.h"
#include "FeedNaiveBayesI.h"
#include "ScoreCache.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "TaskManager.h"
#include "FreqCache.h"
#include <boost/foreach.hpp>
#include <FeedFocusInvertReplicaAdapter.h>
using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace boost;

class PrintHelper {
public:
//  template<typename T>
//  static string Print(const T & container) {
//    ostringstream ios;
//    ios << "(";
//    bool first_item = true;
//    int i = 0;
//    for (typename T::const_iterator itr = container.begin(); itr
//        != container.end(); ++itr, ++i) {
//      if (first_item == true) first_item = false;
//      else ios << ",";
//      ios << *itr;
//    }
//    ios << ")";
//    return ios.str();
//  }

  template<typename T>
  static string Print(const T & container,int limit = 30) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";

    	ios << v;

      if(idx >= limit) break;
    }
    ios << ")";

    return ios.str();
  }
};
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedNaiveBayesI::instance(),
      service.createIdentity("M", ""));
  FeedNaiveBayesI::instance().Init();
	TaskManager::instance().execute(&ReloadTask::instance());
  //TaskManager::instance().scheduleRepeated(&QueueStatTimer::instance());
}

float ChangeScore(float orig, int ratio, float cut){
   float res = orig * ratio;

   return res> cut ?cut : res;
}

//map<int,int>  FeedNaiveBayesI::GetScoreDict(int fromid,const vector<int> & toids, int stype, const Ice::Current& ){
//	TimeStat ts;
//	map<int,int> dict;
//	if(!ScoreCache::instance().ready()){
//		MCE_INFO("FeedNaiveBayesI::GetScoreDict. not ready. fromid:" << fromid  );
//		for(unsigned i= 0; i != toids.size(); ++i){
//			dict[toids.at(i)] = 10;
//		}
//	}
//	ostringstream os;
//	int total_score = 0;
//	for(unsigned i = 0;  i != toids.size();++i){
//
//		int idx_not_test = toids.at(i)%10;
//		if(idx_not_test >=7){
//			continue;
//		}
//
//		int idx = toids.at(i) / 100 % 20;
//
//		float from_score = ScoreCache::instance().GetScore(toids.at(i),fromid);
//		float stype_score = ScoreCache::instance().GetScore(toids.at(i),-stype);
//
//
//		int score,test_score1,test_score2;
////		if(toids.at(i)% 10 ==1){
////		test_score1 = int(pow((double)(ChangeScore(from_score) * ChangeScore(stype_score)),(double) 0.8 ));
////		test_score2 = int(ChangeScore(from_score) * ChangeScore(stype_score));
//
////    int test_score_cut_1 = int(ChangeScore(from_score, 1, 1) * ChangeScore(stype_score, 1, 1));
////    int test_score_20_cut_10 = int(ChangeScore(from_score, 20, 10) * ChangeScore(stype_score, 20, 10));
////    int test_score_200_cut_10 = int(ChangeScore(from_score, 200, 10) * ChangeScore(stype_score, 200, 10));
//    int test_score_400_cut_100 = int(ChangeScore(from_score, 400, 100) * ChangeScore(stype_score, 400, 100));
//
//    switch (idx) {
//		case 5:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.2 )); //数据好
//			break;
//		case 0:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.22 ));
//			break;
//		case 1:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.24 ));
//			break;
//		case 9:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.18));
//			break;
//
//		case 2:
//			score = int(from_score * stype_score * 12000);
//			break;
//		case 7:
//			score = int(from_score * stype_score * 11800);
//			break;
//		case 8:
//			score = int(from_score * stype_score * 12200);
//			break;
//		case 3:
//			score = int(from_score * stype_score * 12400);
//			break;
//
//		case 6:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.3 ));
//			break;
//		case 10:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.32 ));
//			break;
//		case 11:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.28 ));
//			break;
//		case 12:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.26 ));
//			break;
//
//		case 4:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.1 ));
//			break;
//		case 13:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.08 ));
//			break;
//		case 14:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.12 ));
//			break;
//		case 15:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.14 ));
//			break;
//
//		case 19:
//			score = int(pow(double(test_score_400_cut_100), (double)0.6));
//			break;
//		case 16:
//			score = int(pow(double(test_score_400_cut_100), (double)0.58));
//			break;
//		case 17:
//			score = int(pow(double(test_score_400_cut_100), (double)0.62));
//			break;
//		case 18:
//			score = int(pow(double(test_score_400_cut_100), (double)0.64));
//			break;
//
//
///*
//		case 0:
//			score = int(pow(double(from_score * 400) , (double)1.1 ));
//			break;
//		case 1:
//			score = int(from_score * stype_score * 10000);
//			break;
//		case 3:
//			score = int(from_score * stype_score * 13000);
//			break;
//		case 7:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.4 ));
//			break;
//		case 10:
//			score = test_score_200_cut_10 * 1.2;
//			break;
//		case 11:
//			score = test_score_200_cut_10 * 1.3; //数据好, seems to be the best by June 22.
//			break;
//		case 12:
//			score = test_score_200_cut_10 * 1.4;
//			break;
//		case 13:
//			score = test_score_200_cut_10 * 1.5;
//			break;
//    case 14:
//			score = test_score_200_cut_10 * 1.6; // might be good
//			break;
//		case 15:
//			score = test_score_20_cut_10 * 1.3; // result not good
//			break;
//		case 16:
//			score = test_score_20_cut_10 * 1.5;
//			break;
//		case 17:
//			score = test_score_20_cut_10 * 1.7;
//			break;
//		case 18:
//			score = int(pow(double(test_score_400_cut_100), (double)0.5));
//			break;
//*/
//
//		default:
//			;
//		}
//
//    /*
//    if(idx == 0){
//			score = test_score_200_cut_10 * 1.6; // might be good
//    }
//
//		if(idx ==1){
////			score = test_score2;//数据差，
////			score = test_score2 * 1.6;
//			score = test_score_200_cut_10 * 1.6; // might be good
//		} else	if (idx ==2){
////			score = test_score2 * 1.4;
//			score = test_score_20_cut_10 * 1.3; // result not good
//		} else if(idx == 3){
//			score = test_score_200_cut_10 * 1.3; //数据好, seems to be the best by June 22.
//			//score = int(pow(double(from_score * stype_score * 10000) , (double)0.8 ) * 1.2);
//		} else if(idx == 4){
////		score = int(pow(double(from_score * stype_score), double(1)/7) * 100);//数据不好
////			score = int(pow(double(from_score * stype_score), double(1)/2) * 200);
//			score = int(pow(double(test_score_400_cut_100), (double)0.5));
//		} else if(idx == 5){
////			score = int(pow(double(from_score * stype_score * 10000) , (double)1.3 ));
//			score = int(pow(double(from_score * 400) , (double)1.1 ));
//		} else if(idx ==6){
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.3 ));
//		} else {
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.2 )); //数据好 0尾号
//		}
//
//		*/
//
//
//		score = score > 100? 100:score;
////		os << " " << toids.at(i) << ":" << score;
//		total_score += score;
//
//		dict[toids.at(i)] = score;
//	}
////	int total_score = 0;
//
////	for (map<int, int>::iterator it = dict.begin(); it != dict.end(); ++it) {
////		os << " " << it->first << ":" << it->second;
//////		total_score += it->second;
////	}
//	MCE_INFO("FeedNaiveBayesI::GetScoreDict. toids.size:" << toids.size()
//			<< " fromid:" << fromid << " stype:" << stype
//			<< " dictsize:" << dict.size()
//			<< " average:" << float(total_score) / dict.size()
////			<< " scores:" << os.str()
//			<< " cost:" << ts.getTime()
//	);
//	return dict;
//}


//老的
//map<int,int>  FeedNaiveBayesI::GetScoreDict(int fromid,const vector<int> & toids, int stype, const Ice::Current& ){
//	TimeStat ts;
//	map<int,int> dict;
//	if(!ScoreCache::instance().ready()){
//		MCE_INFO("FeedNaiveBayesI::GetScoreDict. not ready. fromid:" << fromid  );
//		for(unsigned i= 0; i != toids.size(); ++i){
//			dict[toids.at(i)] = 10;
//		}
//	}
//	ostringstream os;
//	float total_score = 0.0;
//	float exist_total_score = 0.0;
//	int exist_cnt = 0;
//	float stype_score = 0.0;
//	map<float,int> exist_scores;
//	for(unsigned i = 0;  i != toids.size();++i){
//
//		int idx_not_test = toids.at(i)%20;
//		int idx_h = toids.at(i) / 100 % 10 ;
//		if(idx_not_test != 3 && idx_not_test != 12
//				&& idx_not_test != 5 && idx_not_test!=6
//				&& idx_h != 7 //特殊的
//		){
//			continue;
//		}
//
//		int idx = toids.at(i) % 100 ;
//
//		float from_score = ScoreCache::instance().GetScore(toids.at(i),fromid);
//		stype_score = ScoreCache::instance().GetScore(toids.at(i),-stype);
//
/*/*//*/*/
//		float score = 0;
////		test_score1,test_score2;
////    int test_score_400_cut_100 = int(ChangeScore(from_score, 400, 100) * ChangeScore(stype_score, 400, 100));
//
//		idx = idx / 20;
//    switch (idx) {
//		case 0:
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.18));//原来的9
//			break;
//		case 1:
//			score = int(double(from_score * stype_score * 1000));
//			break;
//		case 2:
//			score = int(double(from_score * stype_score * 3000));
//			break;
//		case 3:
//			score = int(double(from_score * stype_score * 6000));
//			break;
//		case 4:
//			score = int(double(from_score * stype_score * 10000));
//			break;
//		default:
//			;
//		}
//    if (idx_not_test == 5 || idx_not_test == 6){
//    	score = int(pow(double(from_score * stype_score * 10000) , (double)1.18));//原来的9
//    }
//
//
//		if(idx_h == 7){
//			score = int(pow(double(from_score * stype_score * 10000) , (double)1.18));//原来的9
//		}
////		score = float(from_score * stype_score * 1000);
//		score = score > 100.0? 100.0:score;
//		if(score != 0){
//			exist_total_score += float(score);
//			++exist_cnt;
//			exist_scores[score] = toids.at(i);
//		}
////		os << " " << toids.at(i) << ":" << score;
//		total_score += score;
//		dict[toids.at(i)] = int(score);
//	}
//
//	for(map<float,int>::reverse_iterator ri = exist_scores.rbegin(); ri != exist_scores.rend(); ++ri){
//			os << " " << ri->first << ":" << ri->second;
//	}
////	for (map<int, int>::iterator it = dict.begin(); it != dict.end(); ++it) {
////		os << " " << it->first << ":" << it->second;
////	}
//
//	MCE_INFO("FeedNaiveBayesI::GetScoreDict. toids.size:" << toids.size()
//			<< " fromid:" << fromid << " stype:" << stype << " stype_score:" << stype_score
//			<< " dictsize:" << dict.size()
//			<< " average:" << float(total_score) / dict.size()
//			<< " exist cnt:" << exist_cnt
//			<< " exist average:" << exist_total_score/exist_cnt
//			<< " exist scores:" << os.str()
//			<< " cost:" << ts.getTime()
//	);
//	return dict;
//}

//110725新改
//void CutScore(float & old,float up,float low){
//	 old = old > up ? up:old;
//	 old =old < low ? low:old;
//}
//map<int,int>  FeedNaiveBayesI::GetScoreDict(int fromid,const vector<int> & toids, int stype, const Ice::Current& ){
//	TimeStat ts;
//	map<int,int> dict;
//	if(!ScoreCache::instance().ready()){
//		MCE_INFO("FeedNaiveBayesI::GetScoreDict. not ready. fromid:" << fromid  );
//		for(unsigned i= 0; i != toids.size(); ++i){
//			dict[toids.at(i)] = 10;
//		}
//	}
//	ostringstream os;
//	float total_score = 0.0;
//	float exist_total_score = 0.0;
//	int exist_cnt = 0;
//
//	map<float,int> exist_scores;
//	for(unsigned i = 0;  i != toids.size();++i){
//		int idx_h = toids.at(i) / 100 % 10 ;
//		float from_score = ScoreCache::instance().GetScore(toids.at(i),fromid);
//		float stype_score = ScoreCache::instance().GetScore(toids.at(i),-stype);
//
//
//		float score = 0.0f;
//		switch(idx_h){
//		case 0:
//			if(toids.at(i)%100 == 44){
//				score = 0.0f;
//				break;
//			}else{
//				score = pow(float(from_score * stype_score * 8000) , (float)1.13);
//				break;
//			}
//		case 1:
//			CutScore(from_score,1.0f,0.02f);
//			CutScore(stype_score,1.0f,0.02f);
//			score = pow(float(from_score * stype_score * 9000) , (float)1.18);
//			break;
//		case 2:
//			score = pow(float(from_score * stype_score * 8000) , (float)1.18);
//			break;
//		case 3:
//			CutScore(from_score,0.8f,0.02f);
//			CutScore(stype_score,0.8f,0.02f);
//			score = pow(float(from_score * stype_score * 8000) , (float)1.18);
//			break;
//		case 4:
//			score = pow(float(from_score * stype_score * 9000) , (float)1.18);
//			break;
//
//		case 5:
//			score = pow(float(from_score * stype_score * 10000) , (float)1.25);
//			break;
//		case 6:
//			score = pow(float(from_score * stype_score * 10000) , (float)1.13);
//			break;
//		case 7:
//			continue;
//		case 8:
//			continue;
//		case 9:
//			score = pow(float(from_score * stype_score * 10000) , (float)1.18);//原来的9,基准
//			break;
//		}
//
//		score = score > 100.0? 100.0:score;
//		if(score > 1.0f){
//		//if(idx_h== 1){
//			exist_total_score += float(score);
//			++exist_cnt;
//			exist_scores[score] = toids.at(i);
//		}
////		os << " " << toids.at(i) << ":" << score;
//		total_score += score;
//		dict[toids.at(i)] = int(score);
//	}
//
//	for(map<float,int>::reverse_iterator ri = exist_scores.rbegin(); ri != exist_scores.rend(); ++ri){
//			os << " " << ri->first << ":" << ri->second;
//	}
//
//	MCE_INFO("FeedNaiveBayesI::GetScoreDict. toids.size:" << toids.size()
//			<< " fromid:" << fromid << " stype:" << stype
//			<< " dictsize:" << dict.size()
//			<< " average:" << float(total_score) / dict.size()
//			<< " exist cnt:" << exist_cnt
//			<< " exist average:" << exist_total_score/exist_cnt
//			<< " exist scores:" << os.str()
//			<< " cost:" << ts.getTime()
//	);
//	return dict;
//}

void CutScore(float & old,float up,float low){
	 old = old > up ? up:old;
	 old =old < low ? low:old;
}
set<int> FeedNaiveBayesI::GetFocusSource(int target){
	try{
		IntSeq res = FeedFocusInvertReplicaAdapter::instance().GetSources(target);
		return set<int>(res.begin(),res.end());
	}catch (Ice::Exception& e) {
		MCE_WARN("FeedNaiveBayesI::GetFocusSource. target:" << target  << " error:  "<< e);
	}catch(...){
		MCE_WARN("FeedNaiveBayesI::GetFocusSource. target:" << target  << " unknown error" );
	}
	return set<int>();
}
map<int,int>  FeedNaiveBayesI::GetScoreDict(int fromid,const vector<int> & toids, int stype, const Ice::Current& ){
	TimeStat ts;
	map<int,int> dict;
	if(!ScoreCache::instance().ready()){
		MCE_INFO("FeedNaiveBayesI::GetScoreDict. not ready. fromid:" << fromid  );
		for(unsigned i= 0; i != toids.size(); ++i){
			dict[toids.at(i)] = 10;
		}
	}
	ostringstream os;
	float total_score = 0.0;
	float exist_total_score = 0.0;
	int exist_cnt = 0;

	set<int> focusers = GetFocusSource(fromid);
	set<int> usedfocusers;
	map<float,int> exist_scores;
//	for(unsigned i = 0;  i != toids.size();++i){
	BOOST_FOREACH(int toid,toids){
		int idx_h = toid / 100 % 10 ;
		float from_score = ScoreCache::instance().GetScore(toid,fromid);
		float stype_score = ScoreCache::instance().GetScore(toid,-stype);
		if(focusers.count(toid)){
			from_score = 1.0f;
			usedfocusers.insert(toid);
		}

		float score = 0.0f;
		if(idx_h == 7){
			continue;
		}else{
			score = float(from_score * stype_score * 250);
		}

		score = score > 99.0? 99.0:score;
		if(score > 1.0f){
		//if(idx_h== 1){
			exist_total_score += float(score);
			++exist_cnt;
			exist_scores[score] = toid;
		}
//		os << " " << toids.at(i) << ":" << score;
		total_score += score;
		dict[toid] = int(score);
	}

	for(map<float,int>::reverse_iterator ri = exist_scores.rbegin(); ri != exist_scores.rend(); ++ri){
			os << " " << ri->first << ":" << ri->second;
	}

	MCE_INFO("FeedNaiveBayesI::GetScoreDict. toids.size:" << toids.size()
			<< " fromid:" << fromid << " stype:" << stype
			<< " dictsize:" << dict.size()
			<< " average:" << float(total_score) / dict.size()
			<< " exist cnt:" << exist_cnt
			<< " exist average:" << exist_total_score/exist_cnt
			<< " exist scores:" << os.str()
			<< " cost:" << ts.getTime()
			<< " focusers:" << PrintHelper::Print(focusers)
			<< " usedfocusers:" << PrintHelper::Print(usedfocusers)
	);
	return dict;
}


map<int,int>  FeedNaiveBayesI::GetScoreDictReverse(int to,const vector<int> & fromids, int stype, const Ice::Current& ){
	TimeStat ts;
	map<int,int> dict;
	if(!ScoreCache::instance().ready()){
		MCE_INFO("FeedNaiveBayesI::GetScoreDict. not ready. toid:" << to  );
		for(unsigned i= 0; i != fromids.size(); ++i){
			dict[fromids.at(i)] = 10.0f;
		}
	}
	ostringstream os;
	float total_score = 0.0f;
	float exist_total_score = 0.0f;
	int exist_cnt = 0;


	map<float,int> exist_scores;
	for(unsigned i = 0;  i != fromids.size();++i){
		int fromid = fromids.at(i);
		int idx_h = to / 100 % 10 ;
		float from_score = ScoreCache::instance().GetScore(to,fromid);
		float stype_score = ScoreCache::instance().GetScore(to,-stype);

		if(to == 239556923 || to == 220678672){
			idx_h = 1;
		}

		float score = 0.0f;
		switch(idx_h){
		case 0:
			//score = int(double(from_score * stype_score * 1000));
			score = 0.0f;
			break;
		case 1:
			CutScore(from_score,1.0f,0.01f);
			CutScore(stype_score,1.0f,0.01f);
			score = from_score * stype_score * 1000;
			break;
		case 2:
			score = from_score * stype_score * 5000;
			break;
		case 3:
			if(stype % 100 == 5 && stype % 100 == 6 && stype % 100 == 7 ){
				stype_score *= 3.0f;
			}
			score = from_score * stype_score * 5000;
			break;
		case 4:
			score = from_score * stype_score * 10000;
			break;
		case 5:
			score = from_score * stype_score * 50000;
			break;
		case 6:
			score = pow(float(from_score * stype_score * 10000) , (float)1.18);//原来的9
			break;
		case 7:
			continue;
			//老数据
			break;
		case 8:
			continue;
			break;
		case 9:
			continue;
			break;
		}

		score = score > 100.0? 100.0:score;
		if(score != 0){
			exist_total_score += float(score);
			++exist_cnt;
			exist_scores[score] = fromid;
		}
//		os << " " << toids.at(i) << ":" << score;
		total_score += score;
		dict[fromid] = int(score);
	}

	for(map<float,int>::reverse_iterator ri = exist_scores.rbegin(); ri != exist_scores.rend(); ++ri){
			os << " " << ri->first << ":" << ri->second;
	}

//	MCE_INFO("FeedNaiveBayesI::GetScoreDict. toids.size:" << toids.size()
//			<< " fromid:" << fromid << " stype:" << stype
//			<< " dictsize:" << dict.size()
//			<< " average:" << float(total_score) / dict.size()
//			<< " exist cnt:" << exist_cnt
//			<< " exist average:" << exist_total_score/exist_cnt
//			<< " exist scores:" << os.str()
//			<< " cost:" << ts.getTime()
//	);
	return dict;
}

int  FeedNaiveBayesI::GetSendFrequency(int fromid, const Ice::Current&){
	int res = FrequencyCache::instance().GetFrequency(fromid);
	MCE_INFO("FeedNaiveBayesI::GetSendFrequency. fromid:" << fromid << " res:" << res);
	return res;
}

void FeedNaiveBayesI::RegisterController(){
	if(registered_){
		MCE_WARN("FeedNaiveBayesI::RegisterController. all ready registered");
		return;
	}
  ServiceI& service = ServiceI::instance();
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt(
      "FeedNaiveBayes.Mod");
  int interval =
      service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
          "FeedNaiveBayes.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize(
      "ControllerFeedNaiveBayesR", &FeedNaiveBayesI::instance(), mod, interval,
      new XceFeedChannel());
  MCE_INFO("FeedNaiveBayesI::RegisterController. mod:" << mod << " interval:" << interval);
  registered_ = true;
}

void FeedNaiveBayesI::Reload(){
	ReloadTask::instance().handle();
}


void FeedNaiveBayesI::MergeNew(const string & new_score,const string & dump,const Ice::Current&){
	ScoreCache::instance().MergeNew(new_score,dump);
}

