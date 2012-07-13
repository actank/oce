#include "common_header.h"
#include "QueryRunner.h"
#include "ConnectionHolder.h"
#include "TaskManager.h"
#include <string>
#include <fstream>
#include "OceCxxAdapter/src/StatusCacheAdapter.h"
#include "util/cpp/TimeCost.h"

using namespace xce::statuscache;

const size_t BATCH = 100000;
const int MAX_SIZE =3000000;
const int READ_BATCH = 10000;
const int BNUM = 1000;

const int TASK_LEVEL_READ= 0;
const int TASK_LEVEL_LOAD = 10;
class KeyComp
{
public:
		KeyComp(){
		}
		KeyComp(const string& field):field_(field){
		}
        bool operator()(const vector<string>& rowA, int key) const
        {
                return boost::lexical_cast<int>(rowA[0]) < key; 
        }
        bool operator()(int key, const vector<string>& rowA) const
        {
						return key < boost::lexical_cast<int>(rowA[0]);
		}
		string field_;
};
class BufferUtil{
	
};
class Buffer{
public:
	Buffer(string id_field,int max_size,void(SearchCacheData::*p_)(const vector<string>& row), int num):id_field_(id_field),max_size_(max_size),num_(num){	
		cmp_.field_ = id_field_;
		cout<<id_field<<" "<<max_size<<" "<<num<<endl;
		max_id_ = -1;
		size_ = 0;
	}
	Buffer(const Buffer& other){
		id_field_ = other.id_field_;
		max_size_ = other.max_size_;
		p_ = other.p_;
		cmp_.field_ = other.cmp_.field_;
	} 
	Buffer& operator=(const Buffer& other){
		id_field_ = other.id_field_;
		max_size_ = other.max_size_;
		p_ = other.p_;
		cmp_.field_ = other.cmp_.field_;
		return *this;
	}

	void fetch(int num,int min_id, int max_id,vector<vector<vector<string> > > & rows, int& fetchNum){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);	
		size_t size = size_;
		MCE_INFO("fetch "<<num<<" min_id "<<min_id<<" max_id "<< max_id <<" size "<<size<<" multimap: "<<max2Rows_.size());

//		MCE_INFO("get range: "<<num<<" min:"<<min_id<<" ~ "<<max_id<<" but real range: "<<max2Rows_.begin()->first<<" ~ "<<max2Rows_.rbegin()->first);
		if(max2Rows_.rbegin()->first <= min_id){
			return;
		}
		multimap<int,vector<vector<string> > >::iterator iterA;
		multimap<int,vector<vector<string> > >::iterator iterB;
		multimap<int,vector<vector<string> > >::iterator iter;

		iterA = max2Rows_.lower_bound(min_id);
		iterB = max2Rows_.lower_bound(max_id);
		
		if(iterA!=iterB){
		vector<vector<string> >::iterator it = lower_bound(iterA->second.begin(), iterA->second.end(),min_id,cmp_);
		rows.push_back(vector<vector<string> >());	
		if(it!=iterA->second.begin()){
			rows.back().assign(it,iterA->second.end());
			iterA->second.erase(it,iterA->second.end());
		}else{
			rows.back().swap(iterA->second);
		}
		fetchNum += rows.back().size();

		iter = iterA;
		while( ++iter!=iterB){
			rows.push_back(vector<vector<string> >());
			rows.back().swap(iter->second);
			fetchNum += rows.back().size();
		}

		if(iterB!=max2Rows_.end()){
			it = lower_bound(iterB->second.begin(), iterB->second.end(),max_id,cmp_);
			rows.push_back(vector<vector<string> >());
			if(it!=iterB->second.end()){
				rows.back().assign(iterB->second.begin(),it);
				iterB->second.erase(iterB->second.begin(),it);
			}else{
				rows.back().swap(iterB->second);
			}
			fetchNum += rows.back().size();
		}
		if(!iterA->second.empty()){
			++iterA;
		}
		if(iterB!=max2Rows_.end() && iterB->second.empty()){
			++iterB;
		}

		if(iterA!=iterB){
			//cout<<num<<" get range: "<<min_id<<" ~ "<<max_id<<" but real range: "<<max2Rows_.begin()->first<<" ~ "<<max2Rows_.rbegin()->first<<endl;
			//cout<<num<<" iterA :"<<iterA->first<<" iterB: "<<iterB->first<<endl;
			//MCE_INFO("remove range: "<< num <<" iterA: "<<iterA->first<<" iterB:"<<iterB->first);
			max2Rows_.erase(iterA,iterB);
		}	
		}else{
			vector<vector<string> >::iterator itA = lower_bound(iterA->second.begin(),iterA->second.end(),min_id,cmp_);	
			vector<vector<string> >::iterator itB = lower_bound(iterA->second.begin(),iterA->second.end(),max_id,cmp_);	
			if(itA==itB){
				return;
			}
			rows.push_back(vector<vector<string> >());
			if(itA==iterA->second.begin()&&itB==iterA->second.end()){
				rows.back().swap(iterA->second);
				max2Rows_.erase(iterA);
			//	MCE_INFO("remove one range: "<<num<<" iterA:" <<iterA->first<<" iterB:"<<iterB->first);
			}else{
				rows.back().assign(itA,itB);
				iterA->second.erase(itA,itB);
			//	MCE_INFO("remove one range: "<<num<<" iterA:" <<iterA->first<<" iterB:"<<iterA->first);
			}
			fetchNum += rows.back().size();	
		}

		
		size_ -= fetchNum; 
		MCE_INFO("remove "<<num<<" size:"<< fetchNum <<" after:"<<size_);
		if(size_ < max_size_)
			mutex_.notify();
	}

	void append(vector<vector<string> >& rows, int maxid){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		if(size_ > max_size_)
			mutex_.wait();
		//mutex_.timedWait(IceUtil::Time::milliSeconds(300));
		size_ += rows.size();
		pair<int,vector<vector<string> > > tmp = make_pair<int,vector<vector<string> > >(maxid,vector<vector<string> >());
		multimap<int,vector<vector<string> > >::iterator it = max2Rows_.insert(tmp);
		it->second.swap(rows);
	}

private:

	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	string id_field_;
	size_t max_size_; 
	multimap<int,vector<vector<string> > >  max2Rows_;
    void(SearchCacheData::*p_)(const vector<string>& row);
	KeyComp cmp_;
	int num_;
	int max_id_;
	int size_;
};

//typedef IceUtil::Handle<BufferManager> BufferManagerPtr;

struct Node{
	Node(int table,string id_field,const vector<vector<vector<string> > >& rows):
		table_(table),id_field_(id_field){
		posIt = rows.begin();
		endIt = rows.end();
		cur = 0;
		size = posIt->size();
		id_ = boost::lexical_cast<int>((*posIt)[cur][0]);
	}
	bool hasNext(){
		++cur;
		if(cur < size){
			id_ = boost::lexical_cast<int>((*posIt)[cur][0]);
			return true;
		}else{
			while(++posIt!=endIt){
				if(!posIt->empty()){
					cur = 0;
					size = posIt->size();
					id_ = boost::lexical_cast<int>((*posIt)[cur][0]);
					break;
				}
			}
			if(posIt == endIt)
				return false;
			else
				return true;
		}
	}

	int table_;
	string id_field_;
	int id_;
	int cur;
	int size;
	vector<vector<vector<string> > >::const_iterator posIt;
	vector<vector<vector<string> > >::const_iterator endIt;
	
	bool operator<(const Node& other) const{
		if(id_ != other.id_)
			return id_ < other.id_;
		else
			return table_ < other.table_;
	}
};
class Bucket{
public:
	Bucket(){
	};
	
	void put(int id,SearchCacheDataPtr data){
			IceUtil::Mutex::Lock lock(mutex_);	
			hash_map<int,SearchCacheDataPtr>::iterator it = id2Data_.find(id);
			id2Data_.insert(make_pair<int,SearchCacheDataPtr>(id,data));
			if(id2Data_.size()%10000==0)
					cout<<"suffix:"<<id%BNUM<<" size:"<<id2Data_.size()<<endl;
	}
private:
	hash_map<int,SearchCacheDataPtr> id2Data_;
	IceUtil::Mutex mutex_;	
};
typedef boost::shared_ptr<Bucket> BucketPtr;
vector<BucketPtr> buckets_;

class BufferManager : public IceUtil::Thread{
public:
	BufferManager():finish_(0),min_id_(0){
	}
	void addBuffer(string id_field,void(SearchCacheData::*p)(const vector<string>& row), int max_size){
		Buffer buffer(id_field, max_size, p, buffers_.size());
		buffers_.push_back(buffer);
		id_fields_.push_back(id_field);
		methods_.push_back(p);
		slow_table_ = -1;
		maxs_.push_back(-1);
		sizes_.push_back(0);
	}
	void fetchAll(){

		max_id_ = 0x7FFFFFFF;
		int min_buffer = -1;

		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(lock_);
		for(size_t i=0;i<maxs_.size();++i){
			if(!hasFinish(i)){
				if(maxs_[i] < max_id_){
					max_id_ = maxs_[i];
					slow_table_ = i;
				}
			}
		}
//		buffers_[0].check(max_id_);	
	}
	void fetchRange(int min_id,int max_id,vector<vector<vector<vector<string> > > >& table2Rows,int& total){
		TimeCost tc = TimeCost::create("fetchRange",1);
		for(size_t i=0;i<buffers_.size();++i){
			if(!hasFinish(i)){
				int fetchNum = 0;
				buffers_[i].fetch(i,min_id,max_id,table2Rows[i], fetchNum);
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(lock_);
				sizes_[i] -= fetchNum;
				if(i==0)
					total = fetchNum;
			}
		}
	}
	void appendBuf(int table,vector<vector<string> >& rows){
//		TimeCost tc = TimeCost::create("appendBuf",1);
		int size = rows.size();
//		MCE_INFO("appendBuf table:"<<table<<" size:"<<rows.size())
		int maxid = boost::lexical_cast<int>(rows.back()[0]);
		buffers_[table].append(rows,maxid);

		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(lock_);
		maxs_[table] = maxid; 
		sizes_[table] += size;
		if(slow_table_ == table){
			lock_.notify();
		}
	}
	void getTotal(){
		ostringstream oss;
		ostringstream oxx;
		ostringstream oyy;
		for(size_t i=0;i<buffers_.size();++i){
			oss<<sizes_[i]<<" ";
			//oxx<<buffers_[i].min()<<" ";
			oyy<<maxs_[i]<<" ";
		}
		MCE_INFO("Manager size: "<<oss.str());
//		MCE_INFO("Manager min: "<<oxx.str());
		MCE_INFO("Manager max: "<<oyy.str());
		MCE_INFO("Manager slow : "<<slow_table_<<" max_id:"<<max_id_);
	}
	void finish(int num){
		IceUtil::Mutex::Lock lock(mutex_);
		finish_ = finish_ | (1<<num);
	}
	bool hasFinish(int num){
		IceUtil::Mutex::Lock lock(mutex_);
		return (finish_ & (1<<num)) > 0 ? true : false;
	}
	virtual void run(){
		while(true){
			MCE_INFO("threading run");
			fetchAll();	

			if(min_id_ < max_id_){
				MCE_INFO("TaskMananger LOAD size:"<<MyUtil::TaskManager::instance().size(TASK_LEVEL_LOAD));
				MCE_INFO("TaskMananger READ size:"<<MyUtil::TaskManager::instance().size(TASK_LEVEL_READ));
				MyUtil::TaskManager::instance().execute(new MergeTask(this,min_id_,max_id_));
				MCE_INFO("TaskMananger start a task");
				min_id_ = max_id_;
				usleep(500000);
			}
			{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(lock_);
			if(slow_table_ < 0 || min_id_ >= maxs_[slow_table_])
				getTotal();
				lock_.timedWait(IceUtil::Time::seconds(1));	
			}
			{	
			IceUtil::Mutex::Lock lock(mutex_);
			if(finish_ == (1<<(buffers_.size())-1))
				break;
			}
		}
		MergeTaskPtr task = new MergeTask(this,min_id_,0x7FFFFFFF);
		MyUtil::TaskManager::instance().execute(task);
		MCE_INFO("All Done");
	}
	int tableSize(){
		return buffers_.size();
	}
private:
		class MergeTask : public MyUtil::Task{
		public:
				MergeTask(BufferManager* manager, int min_id, int max_id):Task(TASK_LEVEL_LOAD),manager_(manager),idFields_(manager_->id_fields_),methods_(manager_->methods_),
					min_id_(min_id),max_id_(max_id){
					table2Rows_.resize(manager->tableSize());
				};
				virtual void handle(){
					MyUtil::TimeCost tc = MyUtil::TimeCost::create("MergeTask");
					int total = 0;
					manager_->fetchRange(min_id_, max_id_, table2Rows_,total);

					tc.step("memcpy");
					MCE_INFO("Task Start Merge Size : "<<total<<" Min: "<<min_id_<<" Max: "<<max_id_);
					if(!total)
						return;
					map<int,SearchCacheDataPtr> id2Ptr;
//					id2Ptr.reserve(total); 
					for(int i=0;i<table2Rows_[0].size();++i){
						for(int j=0;j<table2Rows_[0][i].size();++j){
								SearchCacheDataPtr p(new SearchCacheData);
								int id = boost::lexical_cast<int>(table2Rows_[0][i][j][0]);
								id2Ptr.insert(pair<int,SearchCacheDataPtr>(id,p));
						}
					}
/*
					for(int i=1;i<table2Rows_.size();++i){
						for(int j=0;j<table2Rows_[i].size();++j){
							for(int k=0;k<table2Rows_[i][j].size();++k){
								int id = boost::lexical_cast<int>(table2Rows_[i][j][k][0]);
								map<int,SearchCacheDataPtr>::iterator p;
								if((p=id2Ptr.find(id))!=id2Ptr.end()){
									(((p->second).get())->*methods_[i])(table2Rows_[i][j][k]);
								}
							}
						}
					}
					tc.step("merge");
					*/
					priority_queue<Node> pq;
					map<int,SearchCacheDataPtr>::iterator cur = id2Ptr.begin();

					for(size_t i=1;i<table2Rows_.size();++i){
						if(!table2Rows_[i].empty()){
							Node node(i,idFields_[i],table2Rows_[i]);
							pq.push(node);
						}
					}
					
					while(!pq.empty() && cur!=id2Ptr.end()){
						Node node =	pq.top();	
						if(node.id_ < cur->first){
							pq.pop();
							if(node.hasNext()){
								pq.push(node);
							}
						}else if(node.id_ > cur->first){	
							++cur;
						}else{
							(((cur->second).get())->*methods_[node.table_])((*node.posIt)[node.cur]);
							pq.pop();
							if(node.hasNext()){
								pq.push(node);
							}
						}
					}
					tc.step("merge");
					for(map<int,SearchCacheDataPtr>::iterator it=id2Ptr.begin();it!=id2Ptr.end();++it){
						buckets_[it->first%BNUM]->put(it->first,it->second);	
					}
					tc.step("insert");

					return;
				}
				vector<vector<vector<vector<string> > > > table2Rows_;
		private:
				BufferManager* manager_;
				vector<string> idFields_;
				vector<void(SearchCacheData::*)(const vector<string>& row)> methods_;
				int min_id_;
				int max_id_;
		};
		typedef IceUtil::Handle<MergeTask> MergeTaskPtr;

	vector<Buffer> buffers_;
	vector<string> id_fields_;
	vector<void(SearchCacheData::*)(const vector<string>& row)> methods_;
	IceUtil::Mutex mutex_;
	IceUtil::Monitor<IceUtil::Mutex> lock_;
	int finish_;
	int min_id_;
	int max_id_;
	int slow_table_;
	vector<int> maxs_;
	vector<int> sizes_;
	
};

BufferManager* manager;

struct Entry{
//	Entry(const string& id_field,const mysqlpp::Row& row,void(SearchCacheData::*p)(const mysqlpp::Row& row)):id_field_(id_field),row_(row),p_(p){
	Entry(){
	};
	Entry(const vector<vector<string> >& rows,void(SearchCacheData::*p)(const vector<string>& row)):rows_(rows),p_(p){
		};
    Entry& operator=(const Entry& a) {
		rows_ = a.rows_;
		p_ = a.p_;
		return *this; 
    };
//	string id_field_;
//	mysqlpp::Row row_;
//	void(SearchCacheData::*p_)(const mysqlpp::Row& row);
	vector<vector<string> > rows_;
	void(SearchCacheData::*p_)(const vector<string>& row);
};
//bitset<2147483647> s; 
/*
class Consumer: virtual public IceUtil::Thread {
public:
		Consumer(int num):num_(num){
			lastTime_ = time(NULL);
		};
//		void push(const string& id_field, const mysqlpp::Row& row, void(SearchCacheData::*p)(const mysqlpp::Row& row)){
		void push(const vector<vector<string> >& row, void(SearchCacheData::*p)(const vector<string>& row)){
			Entry entry(row,p);
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			rows_.push(entry);
			MCE_INFO("Push Not Process "<<rows_.size()*100000<<" Suffix:"<<num_);
		};
		virtual void run(){
			while(true){
				//	IceUtil::Mutex::Lock lock(mutex_);
					Entry entry;
					{
							IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
							//Entry entry = rows_.front();
							while(rows_.empty()){
									mutex_.timedWait(IceUtil::Time::seconds(1));
							}
							entry = rows_.front();
							rows_.pop();
					}
					for(vector<vector<string> >::iterator it=entry.rows_.begin();it!=entry.rows_.end();++it){
						if(it->size()<2)
							continue;
						stringstream ss;
						ss<<it->front();
						int id;
						ss>>id; 
						if(s[id] && id%10==0)
							(buckets_[id%buckets_.size()])->put(id,*it,entry.p_);		
					}
	//			for(vector<Entry>::iterator it=rows.begin();it!=rows.end();++it){
//					string x = it->id_field_;
//					int id = (int)((it->row_)[x.c_str()]);
					//(buckets_[id%buckets_.size()])->put(id,it->row_,it->p_);		
	//			}
			}
		}
private:
//		IceUtil::Mutex mutex_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
		queue<Entry> rows_;
		int num_;
		int lastTime_;
};
typedef IceUtil::Handle<Consumer> ConsumerPtr;

vector<ConsumerPtr> threads;
*/
//bitset<STATUS_CACHE_SIZE> s;

class BatchResultHandlerI : public com::xiaonei::xce::ResultHandler {
public:
//		BatchResultHandlerI(std::string table, std::string id_field, int& count, void (SearchCacheData::*p)(const mysqlpp::Row& row)):table_(table),id_field_(id_field),count_(count),p_(p){
		BatchResultHandlerI(int table_id, std::string table, std::string id_field, int& count, vector<vector<string> >& rows):table_id_(table_id),table_(table),id_field_(id_field),count_(count),rows_(rows){
		};
		virtual bool handle(mysqlpp::Row& row) const{
			rows_.push_back(vector<string>());
			rows_.back().reserve(row.size());
			for(mysqlpp::Row::iterator it=row.begin();it!=row.end();++it){
				rows_.back().push_back(it->c_str());
			}
			++count_;
			if(count_>=READ_BATCH){
					count_=0;
					manager->appendBuf(table_id_,rows_);
//					vector<vector<string> >().swap(rows_);
					rows_.reserve(READ_BATCH);
			}
			//buckets_[id%buckets_.size()]->put(id,row,p_);
			return true;
		}
private:
		int table_id_;
		std::string table_;
		std::string id_field_;
		int& count_;
		vector<vector<string> >& rows_;
};

/*
class BasicResultHandlerI : public com::xiaonei::xce::ResultHandler {
public:
        BasicResultHandlerI(std::string table){
        };
        virtual bool handle(mysqlpp::Row& row) const{
            int id = (int) row["id"];
            return true;
        }
private:
        std::string table_;
};
*/

class ReadTask : public MyUtil::Task{
public:
		ReadTask(int level,int id,std::string desc,std::string table,string field,string id_field,string extra=""):Task(level),id_(id),desc_(desc),table_(table),field_(field),id_field_(id_field),extra_(extra) {
		};
		virtual void handle();
private:
		int id_;
		std::string desc_;
		std::string table_;
		std::string field_;
		std::string id_field_;
		std::string extra_;
};

