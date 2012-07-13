#ifndef ________UtilSlice_slice_HOTWORD_h__
#define ________UtilSlice_slice_HOTWORD_h__

#pragma once
#include "HotWord.h"
#include "ServiceI.h"
#include <queue>
#include "util/cpp/TimeCost.h"
#include "UtilCxx/src/hash_map_string.h"
#include <ext/hash_set>
#include "search/HotWord/Tools.h"
#include "search/HotWord/ProcessThread.h"
namespace xce
{
namespace hotwords 
{

static const int interval = 60*5;
static string file = "/data/xce/state/black_word";

class ScoreTerm;
class SortedThread;

class HotWordManagerI : virtual public HotWordManager, 
		public MyUtil::Singleton<HotWordManagerI>
{
public:
		HotWordManagerI();
		virtual ~HotWordManagerI(){};
		void init();
		virtual void addDoings(const DoingItemSeq& items,const Ice::Current&);

		virtual DoingItemSeq searchDoings(const string& unit,
						const string& key,
						const Ice::Current&);

		virtual HotWordSeq lastHotWords(const string& unit,
						const Ice::Current&);
	
/*
		void addHistory(TermDoings& old,size_t minOccur){
			for(TermDoings::iterator it=old.begin();it!=old.end();++it){
				if(it->second.size() > minOccur){
					pair<hash_map<string,int>::iterator,bool> p = history_.insert(make_pair<string,int>(it->first,it->second.size()));
					if(!p.second){
						p.first->second+=it->second.size();
					}
				}
			}	
			ofstream fout(file.c_str(),ios::trunc); 
			fout<<history_.size()<<endl;
			for(hash_map<string,int>::const_iterator it=history_.begin();it!=history_.end();++it){
				fout<<it->first<<" "<<it->second<<endl;
			}
		}
*/		
private:
		SortedThread* thread_;
		TimeUnit* unit_min_;
		TimeUnit* unit_hour_;
		TimeUnit* unit_day_;
		__gnu_cxx::hash_map<string,int> history_;
		std::vector<DoingItem> items_;
		IceUtil::Mutex mutex_;
};


};
};

#endif
