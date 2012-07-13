
#include "search/HotWord/HotWordManagerI.h"
#include "search/HotWord/ProcessThread.h"

using namespace xce::hotwords;

void MyUtil::initialize() {
    ServiceI& service = ServiceI::instance();
    service.getAdapter()->add(&HotWordManagerI::instance(), service.createIdentity("M",""));
  //  ServerStateSubscriber::instance().initialize("ControllerHotWord",
  //          &HotWordManagerI::instance(), 10, 5);

}
HotWordManagerI::HotWordManagerI(){
	init();
}
void HotWordManagerI::init(){
//	setlocale( LC_CTYPE, "" );
//	locale::global(locale("zh_CN.UTF-8"));
//	TimeUnit* unit_day= new TimeUnit("day",0,7,10,NULL);

	//name carry minOccur limit next
	unit_day_ = new TimeUnit("day",1,20,-1,NULL); 
	unit_hour_ = new TimeUnit("hour",48,6,-1,unit_day_);
	unit_min_ = new TimeUnit("min",6,1,-1,unit_hour_);
	ifstream fin(file.c_str());
	int total;
	fin>>total;
	string key;
	int value;
	unit_min_.history_ = hash_map<string,int>(total);
	while(fin>>key>>value && total--){
		unit_min_.history_.insert(make_pair<string,int>(key,value));
	}
//	TimeUnit* unit_min= new TimeUnit("min",0,60*60/interval,10,1000,unit_hour);
//	TimeUnit(string name,size_t size,size_t carry,size_t minOccur,TimeUnit* next):
	thread_ = new SortedThread(this, unit_min_);
	thread_->start().detach();
}

void HotWordManagerI::addDoings(const DoingItemSeq& items,const Ice::Current& current){
	thread_->putDoings(items);
}

DoingItemSeq HotWordManagerI::searchDoings(const string& unit,
						const string& key,
						const Ice::Current&){
	return unit_min_->search(key);
}

HotWordSeq HotWordManagerI::lastHotWords(const string& unit,
						const Ice::Current&){
	return unit_min_->get();
}


/*

HotWordSeq HotWordManagerI::showHotWords(::Ice::Int start,
				Ice::Int end,
				const Ice::Current& current)
{
		if(end - start == 60*5){
			HotWordSeq res = unit_min_->get();
			MCE_INFO("get hot  words" << res.size());
			res.resize(min((int)res.size(),100));
			return res; 
		}else if(end - start == 60*30){
			HotWordSeq res = unit_hour_->get();
			res.resize(min((int)res.size(),500));
			return res; 
		}else{
			return HotWordSeq();
		}
}

DoingItemSeq HotWordManagerI::searchHotWord(const HotWord& word,
				Ice::Int begin,
				Ice::Int limit,
				const Ice::Current& current)
{
		IceUtil::Mutex::Lock lock(mutex_);
		MCE_INFO("Word "<<word.key<<" Size "<<word.ids.size());
		vector<DoingItem> res;
		for(size_t i=0;i<word.ids.size();++i){
			DoingItem item;
			item.id = word.ids[i];
			vector<DoingItem>::iterator it = lower_bound(items_.begin(),items_.end(),item,DoingItemCmp);	
			if(it!=items_.end()&&it->id == item.id){
				res.push_back(*it);	
			}
		}
		return res;
}

HotWordSeq HotWordManagerI::showHistory(const HotWord& word,
				Ice::Int start,
				Ice::Int end,
				const Ice::Current& current)
{
		return HotWordSeq();
}
*/

