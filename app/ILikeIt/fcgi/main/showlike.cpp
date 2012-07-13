#include "fcgiapp.h"
#include "fcgi_config.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ILikeItCacheAdapter.h"
#include <boost/lexical_cast.hpp>
#include "ILikeFCGIUtil.h"
namespace xce {
	namespace ilikeit {
		string err = "Content-type: text/html\r\n\r\n{code:1,msg:\"服务器繁忙\"}";
		string yes = "Content-type: text/html\r\n\r\n{code:0,msg:\"\",";
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
						std::string ret;
						try {
							map<string, string> mapParam;
							string query_string = ILikeFCGIUtil::GetParam(request);
							//string http_cookie = FCGX_GetParam("HTTP_COOKIE", request.envp);
							ILikeFCGIUtil::ParseParam(query_string, mapParam);
							map<string, string>::iterator itU = mapParam.find("uid");
							map<string, string>::iterator itG = mapParam.find("gid");
							if ( (itU == mapParam.end()) || (itG == mapParam.end()) ) {
								FCGX_FPrintF(request.out, err.c_str());
								FCGX_Finish_r(&request);
								continue;
							} 
							
							int uid = boost::lexical_cast<int>((*itU).second);
							LikeInfo2Ptr lip = ILikeItCacheAdapter::instance().getLikeDetail2( itG->second, uid, 0);
							ret = ILikeFCGIUtil::LikeInfo2JSON(uid, lip );
						}catch(Ice::Exception& e) {
							//string exInfo = string("Content-type: text/html\r\n\r\n loop ice::ex : ")+e.what();
							FCGX_FPrintF(request.out, err.c_str());
							FCGX_Finish_r(&request);
							continue;
						}catch(std::exception& e) {
							//string exInfo = string("Content-type: text/html\r\n\r\n loop ice::ex : ")+e.what();
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
						txt << yes << ret ;
						FCGX_FPrintF(request.out,txt.str().c_str());
						FCGX_Finish_r(&request);

					}
				};

			private:
				FCGX_Request request;
		};
	}
}

using namespace xce::ilikeit;
int main(int argc, char**argv) {

	FCGX_Init();
	ILikeItCacheAdapter::instance();

	std::vector<IceUtil::ThreadPtr> threads;
	for (int i=0;i<50;++i) {
		threads.push_back(new WorkerThread( ));
	}
	for (int i=0;i<50;++i) {
		threads.at(i)->start(128*1024);
	}

	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);
	while(FCGX_Accept_r(&request) >=0 )
	{
		std::string ret;
		try {
			map<string, string> mapParam;
			string query_string = ILikeFCGIUtil::GetParam(request);
			ILikeFCGIUtil::ParseParam( query_string, mapParam);
			map<string, string>::iterator itU = mapParam.find("uid");
			map<string, string>::iterator itG = mapParam.find("gid");
			if ( (itU == mapParam.end()) || (itG == mapParam.end() )) {
				//cerr << "loop illegal param" << endl;
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);
				continue;
			}
			int uid = boost::lexical_cast<int>((*itU).second);
			
			LikeInfo2Ptr lip = ILikeItCacheAdapter::instance().getLikeDetail2( itG->second, uid , 0);
			ret = ILikeFCGIUtil::LikeInfo2JSON(uid, lip );
		}catch(Ice::Exception& e) {
			//string exInfo = string("Content-type: text/html\r\n\r\n loop ice::ex : ")+e.what();
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			continue;
		}catch(std::exception& e) {
			//string exInfo = string("Content-type: text/html\r\n\r\n loop ice::ex : ")+e.what();
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			continue;
		}catch(...){
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);
			// std::cerr << "loop unknown ex" << std::endl;;
			continue;
		}

		std::ostringstream txt;
		txt << yes << ret ;
		FCGX_FPrintF(request.out, txt.str().c_str());
		FCGX_Finish_r(&request);
	}
	FCGX_Free(&request, 0);
};

