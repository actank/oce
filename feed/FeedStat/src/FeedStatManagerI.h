#ifndef __FEEDSTAT_H__
#define __FEEDSTAT_H__

#include <string>
#include <map>
#include <vector>
#include <Ice/Ice.h>
#include "FeedStat.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "Recorder.h"

namespace xce{
namespace feed{
	using namespace std;
	using namespace MyUtil;
	const unsigned int RECORDER_SIZE = 20;
	class FeedStatManagerI :  public FeedStatManager, public Singleton<FeedStatManagerI>{
	public:
		virtual void dispatchRecord(Ice::Int stype, const string& logmsg, const Ice::Current& = Ice::Current());
		virtual void replyRecord(Ice::Int stype, const string& logmsg,  const Ice::Current& = Ice::Current());
		virtual void deliverRecord(Ice::Int stype, const string& logmsg, Ice::Int count, const Ice::Current& = Ice::Current());
		void init(){
			for(size_t i = 0; i < RECORDER_SIZE; ++i){
	        	        RecorderPtr recorderptr = new Recorder;
        	        	recorderPtrSeq.push_back(recorderptr);
		                TaskManager::instance().scheduleRepeated(recorderptr);
        		}
		}
	private:
		vector<RecorderPtr> recorderPtrSeq;
	};
	
};
};
#endif
