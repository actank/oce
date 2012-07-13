#include "ZoieUpdater.h"
#include "util/cpp/TimeCost.h"
using namespace search::update;

ZoieUpdater::ZoieUpdater():last_update_(time(NULL)){
	start();
}
void ZoieUpdater::insert(int id, int oper){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	buffer_.push_back(make_pair<int,int>(id,oper));
}
void ZoieUpdater::insertBatch(const vector<long>& ids,int oper){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	for(vector<long>::const_iterator it=ids.begin();it!=ids.end();++it)
		buffer_.push_back(make_pair<int,int>((int)(*it),oper));
}
void ZoieUpdater::run(){
	while(true){
		try{
			vector<pair<int,int> > ids;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				MCE_INFO("buffer "<< buffer_.size());
				if(buffer_.size() > 1000 || time(NULL) - last_update_ >= 5){
					ids.swap(buffer_);	
				}else{
					mutex_.timedWait(IceUtil::Time::seconds(1));
				}
			}

			if(!ids.empty()){

				last_update_ = time(NULL);
				MCE_INFO("update last_registry_user swap size : " << ids.size());
	      MyUtil::TimeCost tc = MyUtil::TimeCost::create("update last_registry_user",1);

				map<int,int> uniq_ids;
				for(vector<pair<int,int> >::iterator it=ids.begin();it!=ids.end();++it){
					uniq_ids[it->first] = it->second;	
				}
				MCE_INFO("update last_registry_user uniq size : " << uniq_ids.size());
				tc.step("unique");

    		com::xiaonei::xce::Statement sql;
    		sql<<"insert into last_registry_user (user_id, operation) values ";
				for(map<int,int>::iterator it=uniq_ids.begin();it!=uniq_ids.end();++it){
					if(it!=uniq_ids.begin())				
						sql << ",";
					sql << "(" << (int)it->first << "," << it->second << ")" ;
				}
				tc.step("prepare");

				try {
					com::xiaonei::xce::QueryRunner("search", com::xiaonei::xce::CDbWServer).execute(sql);
				}catch(std::exception& e){
					MCE_WARN("update last_registry_user exception " << e.what());		
				}catch(...){
					MCE_WARN("update last_registry_user unknown exception");	
				}
				tc.step("execute");
			}
		}catch(std::exception& e){
			MCE_WARN("ZoieUpdater::Thread Exception : " << e.what());
		}catch(...){
			MCE_WARN("ZoieUpdater::Thread Unknown Exception ");
		}
	}	
}
int main(){
  MyUtil::init_logger("Mce", "tbk.log", "DEBUG");
	com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
	while(true){
		int id = 220840375;
		int oper = 1;
		MCE_INFO("insert");
		ZoieUpdater::instance().insert(id,oper);
		sleep(1);
	}
}

