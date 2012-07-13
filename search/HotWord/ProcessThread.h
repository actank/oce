

#pragma once
#include "search/HotWord/HotWordManagerI.h"
#include "DbCxxPool/src/QueryRunner.h"
//#include "HotWord.h"

using namespace __gnu_cxx;
using namespace com::xiaonei::xce;

namespace xce {
namespace hotwords {

class TimeUnit;
//#pragma once
class HotWordManagerI;
//#pragma once
class SplitWord;

bool DoingItemCompare(const DoingItem& it1,const DoingItem& it2){
	return it1.id < it2.id;
};
bool HotWordCompare(const HotWord& word1,const HotWord& word2){
	return word1.score > word2.score;
}
struct Doings{
	Doings(DoingItemSeq& items,time_t now):process_(false),update_(now){
		items_.swap(items);	
		sort(items_.begin(),items_.end(),DoingItemCompare);
	}
	void done(){
		process_ = true;
	}
	bool hasOut(time_t now){
		return process_ && ( now - update_ >= 5*60 );
	}
	DoingItemSeq items_;
	bool process_;
	time_t update_;
};
struct Doing{
	long id_;
	int uid_;
	Doing(long id, int uid):id_(id),uid_(uid){
	}
	bool operator<(const Doing& doing) const{
		if(uid_!=doing.uid_){
			return uid_ < doing.uid_;
		}else{
			return id_ < doing.id_;
		}
	}
};
struct ScoreTerm{
	ScoreTerm(hash_map<string,vector<Doing> >::const_iterator it):key(it->first),docFreq(it->second.size()),uidFreq(0),history(0),iter(it){
	}
	string key;
	string alias;
	string detail;
	float score;
	int docFreq;
	int uidFreq;
	int history;
	hash_map<string,vector<Doing> >::const_iterator iter;
	bool operator<(const ScoreTerm& term) const{
		return score< term.score;	
	}
	void calc(){
		score = docFreq/(history+1.0f)*(uidFreq/docFreq);
	}

};

struct TimeUnit{
	TimeUnit(string name,size_t carry,size_t minOccur,size_t limit,TimeUnit* next):
		name_(name),carry_(carry),minOccur_(minOccur),limit_(limit),next_(next){
	}
	vector<ScoreTerm> top_; 
	vector<HotWord> res_;
	hash_map<string,DoingItemSeq> cache_; 
	hash_map<string,int> history_;

	string name_;
	size_t carry_;
	size_t minOccur_;
	size_t limit_;
	struct TimeUnit* next_;
	IceUtil::Mutex mutex_;
	
	float similarity(const ScoreTerm& term1, const ScoreTerm& term2){
		int common = 0;
		vector<Doing>::const_iterator itA=term1.iter->second.begin();
		vector<Doing>::const_iterator itB=term2.iter->second.begin();
		while(itA!=term1.iter->second.end()&&itB!=term2.iter->second.end()){
			if(*itA < *itB){
				++itA;
			}else if(*itB < *itA){
				++itB;
			}else{
				++common;
				++itA;
				++itB;
			}
		}
		return (float)common/term1.iter->second.size();
	}
	void swap(const vector<ScoreTerm>& terms,const deque<Doings> q){
		hash_map<string,DoingItemSeq> cache;
		for(size_t i=0;i<terms.size();++i){
			string key = terms[i].key;
			for(size_t j=0;j<terms[i].iter->second.size();++j){
				DoingItem item;
				item.id = terms[i].iter->second[j].id_;
				for(deque<Doings>::const_reverse_iterator itQ =q.rbegin();itQ!=q.rend();++itQ){
					DoingItemSeq::const_iterator it = lower_bound(itQ->items_.begin(),itQ->items_.end(),item);
					if(*it == item){
						cache[key].push_back(*it);
					}
				}
			}
		}
		int mark[terms.size()];
		memset(mark,0,sizeof(mark));
		HotWordSeq words;
		for(size_t i=0;i<terms.size();++i){
			if(mark[i]!=0)
				continue;
			HotWord word;
			word.key = terms[i].key;
			word.alias = terms[i].alias;
			word.docFreq = terms[i].docFreq;
			word.uidFreq = terms[i].uidFreq;
			word.history = terms[i].history;
			word.score = terms[i].score;
			for(size_t j=i+1;j<terms.size();++j){
				if(!mark[j] && similarity(terms[i],terms[j])>0.75F){
					MCE_INFO(terms[i].key<<" like "<<terms[j].key);
					word.likes.push_back(terms[j].key+":"+terms[j].alias);
					word.score = terms[j].score;
					mark[j] = 1;
				}
			}
			mark[i] = 1;
			words.push_back(word);
		}
		sort(words.begin(),words.end(),HotWordCompare);
		IceUtil::Mutex::Lock lock(mutex_);
		res_.swap(words);
		cache_.swap(cache);
//		top_.swap(top);
	}
	vector<HotWord> get(){
		IceUtil::Mutex::Lock lock(mutex_);
		return res_;
	}
	DoingItemSeq search(const string& key){
		IceUtil::Mutex::Lock lock(mutex_);
		return cache_[key];
	}
	static	string getTime(){
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time ( &rawtime );
		timeinfo = localtime ( &rawtime );

		strftime (buffer,80,"%Y-%m-%d-%H-%M",timeinfo);
		return string(buffer);
	}
	int history(const string& key){
		hash_map<string,int>::iterator it = history_.find(key);
		if(it!=history_.end()){
			return it->second; 
		}
		return 0;
	}
};
class PinYinResultHandler : virtual public com::xiaonei::xce::ResultHandler{
	public:
		PinYinResultHandler(std::set<std::string>& ubb_set):ubb_set_(ubb_set){}
	protected:
		virtual bool handle(mysqlpp::Row& res) const{
#ifndef NEWARCH
			string name=res["ubb"].get_string();
#else
			string name=res["ubb"].c_str();
#endif
			ubb_set_.insert(name);
						return true;
		}
	private:
		std::set<std::string>& ubb_set_;
};

class SortedThread : virtual public IceUtil::Thread {
	public:
		SortedThread(HotWordManagerI* manager,TimeUnit* unit):manager_(manager),unit_(unit){
			init();
		}
		void init(){
			try{
				Statement sql;
				sql << "SELECT ubb FROM doing_ubb_cut";
				ubb_set_.clear();
				PinYinResultHandler handler(ubb_set_);
				QueryRunner("biz_friend_doing", CDbRServer).query(sql, handler);
			}catch(std::exception& ex){
				MCE_WARN(ex.what());	
			}
			start_ = time(NULL);
			last_ = time(NULL);
		}
		void putDoings(const DoingItemSeq& doing){
			MCE_INFO("put doings "<< doing.size());
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			buffer_.insert(buffer_.end(),doing.begin(),doing.end());
		}
		virtual void run(){
			while(true){
				MyUtil::TimeCost tc = MyUtil::TimeCost::create("[SortedThread]",1);
				DoingItemSeq doings;
				now_ = time(NULL);
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
					MCE_INFO("sleep "<< (60 - now_ + last_));
					if(now_ - last_ < 60){
						mutex_.timedWait(IceUtil::Time::seconds(60 - (now_ - last_)));
					}
					doings.swap(buffer_);
					if(doings.empty()){
						last_ = time(NULL);
						continue;
					}
					MCE_INFO("[SortedThread] swap doings size "<<doings.size());

					q_.push_back(Doings(doings,now_));
					tc.step("SwapAndAdd");
				}
				process();
				tc.step("Process");
				last_ = time(NULL);
			}
		}

		void process(){
				hash_map<string, vector<Doing> > cache;
				deque<Doings>::iterator itS = q_.begin();
				for(deque<Doings>::iterator itQ=q_.begin();itQ!=q_.end();++itQ){
					if(itQ->hasOut(now_)){
						itS=itQ;
						continue;
					}
					
					for(DoingItemSeq::iterator itV=itQ->items_.begin();itV!=itQ->items_.end();++itV){
						string content = filter(itV->content);
						for(size_t len=2;len<=4 && !content.empty();++len){
							vector<string> hzs;
							SplitWord::splitHZ(content,len,hzs);
							addTerms(hzs,*itV,cache);
						}
					}
					itQ->done();
				}
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				q_.erase(q_.begin(),itS);
				
				hash_map<int,vector<string> > relation;
				hash_map<string,vector<int> > word2Group;
				vector<ScoreTerm> scores;
				build(cache,relation,word2Group);
				top(unit_,cache,scores);
				print(unit_,scores,relation,word2Group,cache.size());

//				cache_.swap(cache);
		}
		void build(hash_map<string, vector<Doing> >& cache,hash_map<int,vector<string> >& relation,hash_map<string,vector<int> >& word2Group){
			hash_map<string,vector<string> > subMap;

			for(hash_map<string,vector<Doing> >::iterator it=cache.begin();it!=cache.end();++it){
				wstring key =SplitWord::str2wstr(it->first);
				if(key.size() > 2){
					string k1 = SplitWord::wstr2str(key.substr(0,key.size()-1));
					string k2 = SplitWord::wstr2str(key.substr(1));
					subMap[k1].push_back(it->first);
					subMap[k2].push_back(it->first);
				}else{
					subMap[it->first].push_back(it->first);
				}
			}
			int group = 0;
			for(hash_map<string,vector<string> >::iterator it=subMap.begin();it!=subMap.end();++it,group++){
				float docFreq=cache[it->first].size()*0.75F;
				vector<string> vec;
				vec.push_back(it->first);
				for(vector<string>::iterator itV=it->second.begin();itV!=it->second.end();++itV){
					if(*itV == it->first)
						continue;
					if(docFreq<=cache[*itV].size()){
						vec.push_back(*itV);
					}
				}	
				for(vector<string>::iterator itV=vec.begin();itV!=vec.end();++itV){
					word2Group[*itV].push_back(group);		
				}
				relation.insert(make_pair<int,vector<string> >(group,vec));
			}
		}
		int uniq(hash_map<string,vector<Doing> >::const_iterator term){
			int uidFreq=0;
			int pre = -1;
			for(vector<Doing>::const_iterator it=term->second.begin();it!=term->second.end();++it){
				if(it->uid_!=pre){
					if(pre!=-1){    
						++uidFreq;
					}
					pre = it->uid_;
				}
			}
			if(pre > 0){
				++uidFreq;
			}
			return uidFreq;
		}

		void score(TimeUnit* unit,hash_map<string,vector<Doing> >::const_iterator term,ScoreTerm& termScore){
			termScore.uidFreq = uniq(term);
			termScore.history+=unit_->history(term->first);
			termScore.calc();
		}
		void top(TimeUnit* unit,const hash_map<string,vector<Doing> >& cache,vector<ScoreTerm>& scores){
			scores.clear();
			priority_queue<ScoreTerm> pq;
			for(hash_map<string,vector<Doing> >::const_iterator it=cache.begin();it!=cache.end();++it){
				ScoreTerm termScore(it);
				score(unit->next_,it,termScore);	
				if(unit->limit_ < 0 || pq.size() < unit->limit_){
					pq.push(termScore);
				}else{
					ScoreTerm topTerm = pq.top();
					if(topTerm.score < termScore.score) {
						pq.pop();
						pq.push(termScore);
					}
				}
			}
			scores.reserve(pq.size());
			while(!pq.empty()){
				scores.push_back(pq.top());
				pq.pop();
			}
		}

		void addQueue(const string& key,const hash_map<string,vector<int> >& word2Group,set<int>& groups,queue<pair<int,int> >& q,int deep){
			hash_map<string,vector<int> >::const_iterator it=word2Group.find(key);	
			if(it!=word2Group.end()){
				for(size_t i=0;i<it->second.size();++i){
					if(groups.find(it->second[i])==groups.end()){
						groups.insert(it->second[i]);
						q.push(make_pair<int,int>(deep+1,it->second[i]));
					}
				}
			}
		}
		bool bfs(const string& key, const hash_map<int,vector<string> > & relation, const hash_map<string,vector<int> >& word2Group,set<int>& groups,ostringstream& oss){
			queue<pair<int,int> > q;	
			addQueue(key,word2Group,groups,q,0);	
			oss<<"[";
			if(q.empty()){
				return false;
			}
			while(!q.empty()){
				int deep = q.front().first;
				int group = q.front().second;
				q.pop();
				hash_map<int,vector<string> >::const_iterator it=relation.find(group);
				oss<<group<<":";
				for(vector<string>::const_iterator itV=it->second.begin();itV!=it->second.end();++itV){
					if(itV!=it->second.begin())
						oss<<",";
					oss<<*itV;
					addQueue(*itV,word2Group,groups,q,deep+1);
				}
				oss<<";";
			}
			oss<<"]";
			return true;
		}

		void print(TimeUnit* unit, vector<ScoreTerm>& scores,const hash_map<int,vector<string> > & relation, const hash_map<string,vector<int> >& word2Group,int total){
			vector<HotWord> words;
			ostringstream path;
			path<< "../log/"<<unit->name_<<"/"<<unit->name_<<"."<<TimeUnit::getTime();
			ofstream out(path.str().c_str());
			out<<scores.size()<<" "<<total<<endl;
			//std::wcout<<endl;
			set<int> groups;
			int rank = 0;
			vector<ScoreTerm> res;
			for(vector<ScoreTerm>::iterator it=scores.begin();it!=scores.end();++it){
				hash_map<string,vector<int> >::const_iterator itW = word2Group.find(it->key);	
				if(itW==word2Group.end()){
					continue;
				}
				ostringstream oss;
				if(bfs(it->key,relation,word2Group,groups,oss)){
					MCE_INFO(rank++<<" "<<it->key<<" "<<SplitWord::getAlias(oss.str())<<" "<<it->score<<" "<<it->uidFreq<<" "<<" "<<it->docFreq<<" "<<it->history<<" "<<oss.str());
					it->detail = oss.str();
					it->alias = SplitWord::getAlias(it->detail);
					res.push_back(*it);
					if(rank > 100 ){
						break;
					}
				}
			}	
			unit->swap(res,q_);
			out.close();
		}
		
		string filter(const string& content){
			string noubb = filterUbb(content);
			string nozz = filterZZ(noubb);
			std::transform(nozz.begin(), nozz.end(), nozz.begin(), ::tolower);
			return nozz;	
		}
		string filterUbb(const string& content){
			ostringstream oss;
			size_t start = 0;
			size_t end = 0;
			while(end < content.size()){
				if(content[end] == '('){
					oss<<content.substr(start,end-start);
					start = end;
				}else if(content[end]==')'){
					string sub = content.substr(start,end - start + 1);
					if(ubb_set_.find(sub)==ubb_set_.end()){
						oss<<sub;
					}
					start = end + 1;
				}
				++end;
			}				
			oss<<content.substr(start,end-start);
			return oss.str();
		}
		string filterZZ(const string& content){
			ostringstream oss;
			string str = "转自";
			size_t i=0,j=0;
			while(j<content.size()){
				i = content.find(str,j);	
				if(i!=string::npos){
					oss<<content.substr(j,i-j);
					j = content.find(":",i+str.size());
					if(j!=string::npos){
						i=++j;
					}else{
						j=content.size();
					}
				}else{
					oss<<content.substr(j);
					break;
				}
						
			}
			return oss.str();
		}

		void addTerms(vector<string>& hzs,const DoingItem& item, hash_map<string, vector<Doing> >& cache){
			Doing doing(item.id,item.uid);
			sort(hzs.begin(),hzs.end());
			string pre;
			for(vector<string>::const_iterator it=hzs.begin();it!=hzs.end();++it){
				if(*it!=pre){
					if(!pre.empty()){
						MCE_INFO("add term "<< pre);
						cache[pre].push_back(doing);
					}
					pre = *it;
				}
			}
			if(!pre.empty()){
				MCE_INFO("add term "<< pre);
				cache[pre].push_back(doing);
			}
		} 
	private:
		DoingItemSeq buffer_;
		deque<Doings> q_;
//		std::map<string,std::vector<Doing> > cache_;
		std::vector<DoingItem> items_;
		std::set<string> ubb_set_;
//				__gnu_cxx::hash_set<string> black_;
//		hash_map<string, vector<Doing> > cache_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
		HotWordManagerI* manager_;
    TimeUnit* unit_;
	  int start_;
    int last_;
    int now_;

};

};
};

