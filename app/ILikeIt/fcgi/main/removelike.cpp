#include "fcgiapp.h"
#include "fcgi_config.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ILikeItLogicAdapter.h"
#include <boost/lexical_cast.hpp>
#include "TicketAdapter.h"
#include "ILikeFCGIUtil.h"

namespace xce {
namespace ilikeit {

using namespace passport;

string err = "Content-type: text/html\r\n\r\n{code:1,msg:\"服务器繁忙\"}";
string yes = "Content-type: text/html\r\n\r\n{code:0,msg:\"\",";

// post信息給api组
class PostRemoveTask : public MyUtil::Task {
public:
	PostRemoveTask(const map<string, string>& mapParam):
		_mapParam(mapParam) {
	}
	virtual void handle() {
		ostringstream cmd;
		cmd << "curl -d \"";
		for (map<string, string>::iterator it = _mapParam.begin(); it != _mapParam.end(); ++it) {
			if ( it != _mapParam.begin() ) {
				cmd << "&";
			}
			cmd << (*it).first << "=" << (*it).second;
		}
		cmd << "\" http://like.renren.com/platformremovelike";
		system(cmd.str().c_str());
	}
private:
	map<string, string> _mapParam;
};

class WorkerThread : virtual public IceUtil::Thread {
public:
	WorkerThread(){
		FCGX_InitRequest(&request, 0, 0);
	}
	~WorkerThread(){
		FCGX_Free(&request, 0);
	}
	virtual void run(){
		while(FCGX_Accept_r(&request) >= 0)
		{
			std::string ret ;
			try {
				map<string, string> mapParam;
				string query_string = ILikeFCGIUtil::GetParam(request);
				string http_cookie = FCGX_GetParam("HTTP_COOKIE", request.envp);
				ILikeFCGIUtil::ParseParam(query_string, mapParam);
				map<string, string>::iterator itU = mapParam.find("uid");
				map<string, string>::iterator itG = mapParam.find("gid");
				//map<string, string>::iterator itO = mapParam.find("owner");
				//map<string, string>::iterator itM = mapParam.find("msg");
				if ( (itU == mapParam.end()) || (itG == mapParam.end())) { 
							//|| (itO == mapParam.end()) || (itM == mapParam.end())) {
					FCGX_FPrintF(request.out, err.c_str());
					FCGX_Finish_r(&request);
					continue;
				}
				int uid = boost::lexical_cast<int>((*itU).second);
				string m;
				if( uid != TicketAdapter::instance().verifyTicket(ILikeFCGIUtil::ParseCookie( http_cookie), m ) ) {
					FCGX_FPrintF(request.out, err.c_str());
					FCGX_Finish_r(&request);
					continue;	
				}
				LikeInfo2Ptr lip = ILikeItLogicAdapter::instance().removeLikeAndGet2( itG->second, uid );
				//ILikeFCGIUtil::sendWebPager(itO->second,lip->totalCount,itM->second);
				ret = ILikeFCGIUtil::LikeInfo2JSON(uid, lip );

				// Post这个Remove的信息	
				// TaskManager::instance().execute(new PostRemoveTask((*itG).second, (*itU).second));
				TaskManager::instance().execute(new PostRemoveTask(mapParam));
			}catch(Ice::Exception& e) {
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);
				continue;
			}catch(std::exception& e) {
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);
				continue;
			}catch(...){
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);
				continue;
				// std::cerr << "loop unknown ex" << std::endl;;
			}
			ostringstream txt;
			txt<<yes<<ret;
			FCGX_FPrintF(request.out, ILikeFCGIUtil::convert(txt.str()).c_str());
			FCGX_Finish_r(&request);
		}
	};

private:
	FCGX_Request request;
};
}
}

using namespace xce::ilikeit;
using namespace passport;
int main(int argc, char**argv) {

	FCGX_Init();

	TaskManager::instance().config(TASK_LEVEL_POST, MyUtil::ThreadPoolConfig(1, 100));
//	MyUtil::init_logger("Mce","oce_log_remove","INFO");

	std::vector<IceUtil::ThreadPtr> threads;
	for (int i=0;i<20;++i) {
		threads.push_back(new WorkerThread( ));
	}
	for (int i=0;i<20;++i) {
		threads.at(i)->start(128*1024);
	}

	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);
	while(FCGX_Accept_r(&request) >=0 )
	{
		std::string ret ;
		try {
			map<string, string> mapParam;
			string query_string = ILikeFCGIUtil::GetParam(request);
			string  http_cookie = FCGX_GetParam("HTTP_COOKIE", request.envp);
			ILikeFCGIUtil::ParseParam(query_string, mapParam);
			map<string, string>::iterator itU = mapParam.find("uid");
			map<string, string>::iterator itG = mapParam.find("gid");
			//map<string, string>::iterator itO = mapParam.find("owner");
                        //map<string, string>::iterator itM = mapParam.find("msg");
                        if ( (itU == mapParam.end()) || (itG == mapParam.end())) { 
                                              // || (itO == mapParam.end()) || (itM == mapParam.end())) {

				//cerr << "loop illegal param" << endl;
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);
				continue;
			}
			int uid = boost::lexical_cast<int>((*itU).second);
			string m;
			if( uid != TicketAdapter::instance().verifyTicket(ILikeFCGIUtil::ParseCookie( http_cookie) ,m )) {
				FCGX_FPrintF(request.out, err.c_str());						
				FCGX_Finish_r(&request);
				continue;
			}
			LikeInfo2Ptr lip = ILikeItLogicAdapter::instance().removeLikeAndGet2( itG->second, uid);
			//ILikeFCGIUtil::sendWebPager(itO->second,lip->totalCount,itM->second);
			ret = ILikeFCGIUtil::LikeInfo2JSON( uid,lip );
		}catch(Ice::Exception& e) {
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			continue;
		}catch(std::exception& e) {
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			continue;
		}catch(...){
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			continue;
			// std::cerr << "loop unknown ex" << std::endl;;
		}
		ostringstream txt;
		txt<<yes<<ret;
		FCGX_FPrintF(request.out, ILikeFCGIUtil::convert(txt.str()).c_str());
		FCGX_Finish_r(&request);
	}
	FCGX_Free(&request, 0);
};

