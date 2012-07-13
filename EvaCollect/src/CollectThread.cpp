#include <sstream>
#include <map>
#include <ctime>
#include "CollectThread.h"

using namespace xce::eva::collect;
using namespace com::xiaonei::service;

const static int TIME_INTERVAL = 15;
const static int timeout = 300000;
IceUtil::Handle<EvaCollector> xce::eva::collect::EvaCollector::collector_;
IceUtil::Mutex xce::eva::collect::EvaCollector::mutex_;
IceUtil::Mutex xce::eva::collect::StatTopPart::_mapMutex;
IceUtil::Mutex xce::eva::collect::StatTopPart::_timeMutex;

void EvaCollector::initialize(string loggerEndpoints) {	


	// 打一行提示性能评价系统布置成功的log
	std::cout << "[EvaCollector::initialize] start to use ice_evolution system\n";
	// 设置黑名单词
//	blackwords_.push_back("SubscriberRandom_");

	logger_ =logAdapter_.getLogger(loggerEndpoints);
	logger_->initialize();

	int bucketSize = (BUCKET_SIZE>0)?BUCKET_SIZE:1;
	for (int i=0;i<bucketSize;++i) {
		LogHolderPtr holder = new LogHolder(logger_);
		holder->initialize(i);
		holders_.push_back( holder );
	}
	loggerCategoriesThread.push_back("IceLog");      //设置threadPool的log的路径
	loggerCategoriesThread.push_back("threadPoolNum");

}

EvaCollector& EvaCollector::instance() {
	if ( !collector_ ) {
		IceUtil::Mutex::Lock lock(mutex_);
		if ( ! collector_ ) {
			collector_ = new EvaCollector;
			collector_->initialize();
		}
	}
	return *collector_;
}

LogHolderPtr EvaCollector::findHolder() {	
	int seed = rand();
	int size = holders_.size();
	if ( size > 0 ) {
		return holders_[seed % size];
	} else {
		return NULL;
	}
}

void EvaCollector::collect(const std::string& serviceName, const std::string& operaName, const long time, const int responseTime, bool isException){
	// 过滤无效内容
	/* for ( vector<string>::const_iterator it = blackwords_.begin(); it != blackwords_.end(); ++it) {
		if ( serviceName.find(*it) == 0 ) {
			return;
		}
	} */
	if((serviceName.find("SubscriberRandom") != string::npos) || ( operaName.find("-") != string::npos )){
		return;
	}

	LogTail tail(serviceName+"-"+operaName, time, responseTime, isException);
	LogHolderPtr holder = EvaCollector::instance().findHolder();
	if ( holder ) {
		holder->add(tail);
	}
}
void EvaCollector::printThreadPoolLog(const std::string& prefix, int threadPoolNum){

        //std::vector<std::string> loggerCategories;
        //loggerCategories.push_back("IceLog");
        //loggerCategories.push_back("threadPoolNum");

        if( prefix.find("-") != string::npos){
                return;
        }

        std::ostringstream os; 
        os<< time(NULL) << " " << prefix << " " << threadPoolNum;
        //os<< time(NULL) << " " << prefix << " " << rand()%10;

        logger_->addLog(loggerCategoriesThread, os.str());

}

void LogHolder::initialize(int suffix) {
	output_ = new OutputThread(*this, suffix, mutex_);
	output_->start();
  	buffer_.reserve(300000);
}

/* static std::string time2Str(time_t t) {
	struct tm tmCur = *localtime(&t);
	char timeCurStr[40];
	sprintf(timeCurStr, "%04d-%02d-%02d_%02d:%02d:%02d", tmCur.tm_year+1900, tmCur.tm_mon+1, tmCur.tm_mday, tmCur.tm_hour, tmCur.tm_min, tmCur.tm_sec);
	return timeCurStr;
} */


void Stat::merge() {

	//cout<<"add Log size is "<<tails_.size()<<endl;
	for(std::vector<LogTail>::const_iterator i=tails_.begin(); i!=tails_.end(); ++i){
		string keyValue = i->name;
		std::map<std::string, std::vector<LogCount> >::iterator it = logMap_.find(keyValue);
		if(it != logMap_.end()){
			(*it).second.push_back(LogCount(*i));
		} else {
			std::vector<LogCount> logVector;
			logVector.push_back(LogCount(*i));
			logMap_.insert(make_pair(keyValue, logVector));
		}   
	}
}

void Stat::output(time_t timeDur, com::xiaonei::service::XceLoggerPtr logger, const vector<string>& loggerCategories) {

	for(std::map<std::string, std::vector<LogCount> >::const_iterator map_it = logMap_.begin();
			map_it!=logMap_.end(); ++map_it){
		std::string message;
		std::vector<LogCount> logVector = map_it->second;

		std::string keyValue = map_it->first;
		std::string serverName = keyValue;
	        std::string operaName = "";
		size_t pos = keyValue.find("-");
		if ( pos != string::npos ) {
			serverName = keyValue.substr(0, pos);
			operaName = keyValue.substr(pos+1);
		}

		// 依次执行各次统计
		for (vector<StatObjectPtr>::iterator itStat = statObjects_.begin();
				itStat != statObjects_.end(); ++itStat ) {
			(*itStat)->output(timeDur, serverName, operaName, logVector, logger, loggerCategories);
		}
	}
};

void StatObject::output(time_t timeDur, const string& serviceName, const string& operaName, const vector<LogCount>& logVector, com::xiaonei::service::XceLoggerPtr logger, const std::vector<std::string>& loggerCategories) {

	string statMsg = serviceName + "-" + operaName;
	std::map<string, std::vector<int> > recordMap;
	
	if ( stat(statMsg, recordMap, logVector)) {	
		int timeCur = time(NULL);
		
		
		if(statMsg.find("record_99") != string::npos){	//统计99％的stat，并且到达指定之间则打印log
			
/*		 for(std::map<string, std::vector<int> >::iterator ittest = recordMap.begin(); ittest != recordMap.end(); ++ittest){
                 	 std::string keyValuetest = ittest->first;
	                  std::vector<int> recordSeqtest = ittest->second;
        	          int i = 0;
                	  for(std::vector<int>::iterator itSeqtest = recordSeqtest.begin(); itSeqtest != recordSeqtest.end(); ++itSeqtest){
                        	  MCE_INFO("mapTestOut " << keyValuetest << " " << i << " " << *itSeqtest);
	                          i++;
        	          }
                	  MCE_INFO("test");
	          }*/
		


			for(std::map<string, std::vector<int> >::iterator it = recordMap.begin(); it!= recordMap.end(); ++it){
				std::ostringstream os;
				string key = it->first;
		
				string serviceNameTmp;
				string operaNameTmp;
		
				size_t pos = key.find("-");
		                if ( pos != string::npos ) {
                	        	 serviceNameTmp = key.substr(0, pos);
	                        	 operaNameTmp = key.substr(pos+1);
        		        }

				os << timeCur << " " << serviceNameTmp << " " << operaNameTmp;	

				std::vector<int> recordSeq = it->second;
				for(std::vector<int>::iterator itSeq = recordSeq.begin(); itSeq != recordSeq.end(); ++itSeq){
					os << " " << (*itSeq);
				}
			//	os << " " <<recordMap.size() << " " << statMsg;		
		
				std::vector<std::string> categories(loggerCategories);
				categories.pop_back();
				categories.push_back("record_99");
				try {
		                         logger->addLog(categories, os.str());
		                 }catch(Ice::Exception& e){
                	         //MCE_WARN("[IceThread]:  Ice::Exception " << e.what());
		                 }catch(std::exception& e){
                	         //MCE_WARN("[IceThread]:  td::exception ");
		                 }catch(...){
                	         //MCE_WARN("[IceThread]:  Exception ");
		                 }				
			}
		 }else {

			std::ostringstream os;
			os<< timeCur << " " << timeDur << " " << serviceName << " " << operaName << " " << statMsg;
			// string message=os.str();

			try {
				logger->addLog(loggerCategories, os.str());
			}catch(Ice::Exception& e){
				//MCE_WARN("[IceThread]:  Ice::Exception " << e.what());
			}catch(std::exception& e){
				//MCE_WARN("[IceThread]:  td::exception ");
			}catch(...){
				//MCE_WARN("[IceThread]:  Exception ");
			}
		}
	}
}

bool StatAvgByTime::stat(string& statMsg, std::map<std::string, std::vector<int> >& statMsgMap, const vector<LogCount>& logVector) {
	statMsg = "";

	long sumTime = 0;
	int averageTime = 0;
	int maxTime = 0;
	int minTime = numeric_limits<int>::max();
	int timeoutNum=0;		
	ostringstream os;
	for(std::vector<LogCount>::const_iterator vector_it = logVector.begin(); vector_it != logVector.end();vector_it++){
		sumTime += (*vector_it).responseTime;  
		maxTime = (*vector_it).responseTime > maxTime ? (*vector_it).responseTime : maxTime;
		minTime = (*vector_it).responseTime < minTime ? (*vector_it).responseTime : minTime;
		if((*vector_it).responseTime > timeout)
			++timeoutNum;
	} 
	int size = logVector.size();
	if (size > 0 ) {
		averageTime = sumTime/size;
	} else {
		averageTime = 0;
	}
	os << size << " " << averageTime << " " << maxTime << " " << minTime << " " << timeoutNum;
	statMsg = os.str();
	return true;
}

bool StatTopPart::stat(string& statMsg, std::map<std::string, std::vector<int> >& statMsgMap, const vector<LogCount>& logVector) {

	ostringstream os;
	//统计log，将logVector中的数据收集到_recordSeq中
	{
		IceUtil::Mutex::Lock lock(_mapMutex);
		std::map<string, std::vector<int> >::iterator itKey = _recordMap.find(statMsg);
		if(itKey == _recordMap.end()){
			std::vector<int> recordSeq(34, 0);
			_recordMap.insert(make_pair(statMsg, recordSeq));
			itKey = _recordMap.find(statMsg);
		} 

		for(vector<LogCount>::const_iterator it = logVector.begin(); it!=logVector.end(); ++it){
			int pos = (*it).responseTime/1000;
	
			int index = getIndex(pos);
			++(itKey->second)[index];
		
		}
	
		(itKey->second)[33] += logVector.size();
	}

	//如果到达时间间隔则输出，返回statMsgMap，否则返回statMsg为" "

	statMsg = os.str();
	time_t nowTime = time(NULL);
	{
		IceUtil::Mutex::Lock lock(_timeMutex);
		if ( nowTime - _timeLast < 300 ) {
			return false;
		} 
		_timeLast = nowTime;
	}
		
        std::map<string, std::vector<int> > recordMapTemp;

        {
                IceUtil::Mutex::Lock lock(_mapMutex);
                recordMapTemp.swap(_recordMap);
        }

        for(std::map<string, std::vector<int> >::iterator it = recordMapTemp.begin(); it != recordMapTemp.end(); ++it){
               std::string key = it->first;
               std::vector<int> recordSeq = it->second;

               //统计服务名+方法名的key的log的99%，99.9%，99.99%的记录
               int logSize = recordSeq[33];
               int size99 = logSize/100;
               bool flag99 = true;
               int result99 = 0;

               int size99_9 = logSize/1000;
               bool flag99_9 = true;
               int result99_9 = 0;

               int size99_99 = logSize/10000;
               bool flag99_99 = true;
               int result99_99 = 0;

               int count = 0;
               int index = 32;

               for(; index >= 0 ; --index){
                       count += recordSeq[index];
                       if(count > size99_99 && flag99_99){
                               result99_99 = getRecord(index);
                               flag99_99 = false;
                       }

                       if(count > size99_9 && flag99_9){
                               result99_9 = getRecord(index);
                               flag99_9 = false;
                       }

                       if(count > size99 && flag99){
                               result99 = getIndex(index);
                               flag99 = false;
                               break;
                       }
               }

               std::vector<int> resultSeq;
               resultSeq.push_back(result99);
               resultSeq.push_back(result99_9);
               resultSeq.push_back(result99_99);

               statMsgMap.insert(make_pair(key, resultSeq));

               _timeLast = time(NULL);

        }
         
       os << " record_99";
       statMsg = os.str();

       return true;

}

int StatTopPart::getIndex(int record){
	int result = 0;
	if((record<=20)&&(record>=0)) {
		result = record;
	} else if((record<=100)&&(record>=21)) {
		result = record/10+18;
	} else if((record<=300)&&(record>=101)) {
		result = (record - 100)/50 + 28;
	} else {
		result = 32;
	}

	return result;
}

int StatTopPart::getRecord(int index){
	int result = 0;
	if(index == 32) {
		result = 300;
	} else if((index<=31)&&(index>=28)) {
		result = (index-28)*50+100;
	} else if((index<=27)&&(index>=21)) {
		result = (index-18)*10;
	} else if((index<=20)&&(index>=0)) {
		result = index;
	}

	return result;
}


/* void StatThreadPool::output(const string& serviceName, const std::vector<LogCount>& logVector, com::xiaonei::service::XceLoggerPtr logger, const std::vector<std::string>& loggerCategories){
        string statMsg;
        if(stat(statMsg, logVector)){
                std::ostringstream os;
                int timeCur = time(NULL);
                os << "ThreadPool" << " " << timeCur << " " << serviceName << " " << statMsg;
                logger->addLog(loggerCategories, os.str());
        }
} */

/*bool StatThreadPool::stat(string& statMsg, const vector<LogCount>& logVector){
        ostringstream os;
        int maxInUseNum = 0;
        for(std::vector<LogCount>::const_iterator vector_it = logVector.begin();vector_it!=logVector.end();++vector_it){
                maxInUseNum = (*vector_it).threadpool > maxInUseNum ? (*vector_it).threadpool : maxInUseNum;
        }
        os << "ThreadPool" << " " << maxInUseNum;
        statMsg = os.str();
        return true;
}*/



void LogHolder::OutputThread::run() {
	// while(1)
	// 1 got LogHolder::Buffer
	// 2 send
	std::vector<std::string> loggerCategories;
	ostringstream bucket;
	bucket << "bucket_" << suffix_;
	loggerCategories.push_back("IceLog");
	loggerCategories.push_back(bucket.str());
	// time_t timeBegin = time(NULL);
	// time_t timeLast = time(NULL);

	int sleep_time = TIME_INTERVAL;
	time_t timeLast = time(NULL);
	while (true) {		
		time_t timeDur = time(NULL) - timeLast;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			if ( (holder_.size() < size_t(BATCH_SIZE)) && (timeDur < sleep_time) ) {
				mutex_.wait();
				continue;
			}
		}
		std::vector<LogTail> t;
		t.reserve(300000);
		holder_.swap(t);
		Stat stat(t);
		stat.merge();
	//	std::cout<<"printLog"<<time(NULL)<<endl;
		stat.output(timeDur, holder_.logger_, loggerCategories);
		timeLast = time(NULL);

		// sleep(sleep_time);
		sleep(1);
	}
}


void LogHolder::add(const LogTail& tail) {
	// IceUtil::Mutex::Lock lock(mutex);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	buffer_.push_back(tail);
	mutex_.notify();
}

void LogHolder::swap(vector<LogTail>& logs) {
	// IceUtil::Mutex::Lock lock(mutex);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	buffer_.swap(logs);
}

size_t LogHolder::size() {
	return buffer_.size();
}
