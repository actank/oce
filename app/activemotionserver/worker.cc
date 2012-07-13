#include "worker.h"
#include "hadooplogger.h"
//#include <locallogger.h>
#include "socketlogger.h"
#include <ostream>

using namespace std;

/*****************************************************************************/

void WorkMessage::Run(){
	SetDataHelper::GetInstance().InvokeDelay(type_,time_,param_);
}

/*****************************************************************************/

void NotifyMessage::Run(){
	for( size_t i=0; i<datas_.size(); ++i ){
		Json::Value value;
		Json::Reader reader;
		try{
			const std::string& paramJ = datas_[i]->param_;
			if(reader.parse(paramJ,value) && value.isObject()) {
				int id = boost::lexical_cast<int>( value.get(DjParamID, 0).asString() );
				std::string r_url = value.get(DjParamR, "NULL").asString();
				std::string u_url = value.get(DjParamU, "").asString();
				std::string t_url = value.get(DjParamT, "").asString();
				if( datas_[i]->type_ == 1 ){
					if( u_url == "" ){
						HADOOP_INFO( datas_[i]->time_ << " " << id << " 1 " << r_url );
					} else{
						HADOOP_INFO( datas_[i]->time_ << " " << id << " 0 " << r_url << " " << u_url << " " <<t_url);
					}
					try{
						ostringstream o;
						o << ((long)datas_[i]->time_)*1000;
						value["time"] = o.str();
						SOCKET_WARN( value.toStyledString() );
					} catch(...){
						//TODO: nothing;
					}
				} else if( datas_[i]->type_ == 6){
					std::string o_info = value.get(DjParamO, "NULL").asString();
					HADOOP_INFO( datas_[i]->time_ << " " << id << " " << datas_[i]->type_ << " " << r_url << " " << o_info );
				} else {
					HADOOP_INFO( datas_[i]->time_ << " " << id << " " << datas_[i]->type_ << " " << r_url );

				}
			}
		} catch(...){
			//TODO: nothing;
		}
	}
}

/*****************************************************************************/

static void InitSetDataHelper() {
	LOG(INFO) << "==> SetDataHelper inited !";
	//FCGI_INFO( "==> SetDataHelper inited !");
}

SetDataHelper& SetDataHelper::GetInstance() {
	static XCE_DECLARE_ONCE(once_guard_);
	static SetDataHelper sdh_;
	xce::XceOnceInit(&once_guard_, &InitSetDataHelper);
	return sdh_;
}

/*****************************************************************************/

void SetDataHelper::InvokeDelay( int type, int time, const std::string& param ){
	{
		boost::mutex::scoped_lock alock(mutex_);
		datas_.push_back( boost::shared_ptr<NotifyData>( new NotifyData(type,time,param) ) );
		if (datas_.size() >= MAX_DATA_SIZE ){
			LOG(WARNING) << "==> size: " << datas_.size() << " max_size_notify_now";
			//FCGI_WARN( "==> size: " << types_.size() << " max_size_notify_now");
			datas_full_.notify_all();
		}
	}
}
void SetDataHelper::InvokeNow( int type, int time, const std::string& param ){
	{
		boost::mutex::scoped_lock alock(mutex_);
		datas_.push_back( boost::shared_ptr<NotifyData>( new NotifyData(type,time,param) ) );
		datas_full_.notify_all();
	}
}

void SetDataHelper::Swap(std::vector<boost::shared_ptr<NotifyData> >& datas){
	{
		boost::mutex::scoped_lock alock(mutex_);
		if (datas_.size() < MAX_DATA_SIZE ){
			boost::system_time tAbsoluteTime = boost::get_system_time() + boost::posix_time::seconds(MAX_WAIT_TIME);
			datas_full_.timed_wait(alock,tAbsoluteTime );
		}
		datas_.swap( datas );
	}
}

void SetDataHelper::SetDataMethod::Run() {
	while (true){
		std::vector<boost::shared_ptr<NotifyData> > datas;

		helper_->Swap( datas );
		if( datas.empty() ){
			LOG(WARNING) << "==> notify_empty";
			//FCGI_WARN( "==> notify_empty");
			continue;
		}

		LOG(INFO) << "==> notify_size: " << datas.size();
		//FCGI_INFO( "==> notify_size: " << types.size());
		notify_thrdp_.Post( new NotifyMessage(datas) );
	}
}

