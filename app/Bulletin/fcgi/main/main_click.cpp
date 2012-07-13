
#include "fcgi_config.h"
#include "fcgiapp.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "BulletinSearch.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include "OceCxxAdapter/src/BulletinAdapter.h"
#include <boost/detail/atomic_count.hpp>

//#include "Print.h"
using namespace std;

boost::detail::atomic_count call_count(0);
boost::detail::atomic_count timeout_count(0);

namespace xce {
namespace blt {
	using namespace xce::blt;
	void ParseParam(map<string, string>& mapParam, const string& query_string);
	
	class Worker4ClickThread : virtual public IceUtil::Thread {
	public:
		Worker4ClickThread() {
			FCGX_InitRequest(&request, 0, 0);
		}
		~Worker4ClickThread(){
			FCGX_Free(&request, 0);
		}
		virtual void run(){
			for (;;) {
				int rc = FCGX_Accept_r(&request);
				if (rc < 0) break;
				try {
					map<string, string> mapParam;
					string query_string = FCGX_GetParam("QUERY_STRING", request.envp);
					ParseParam(mapParam, query_string);
					map<string, string>::iterator itU = mapParam.find("uid");
					map<string, string>::iterator itI = mapParam.find("bid");

					if ( (itU == mapParam.end()) || (itI == mapParam.end()) ) {
						FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
						FCGX_Finish_r(&request);
						continue;
					}
					int userId = boost::lexical_cast<int>((*itU).second);
					int bulletinId = boost::lexical_cast<int>((*itI).second);
					++call_count;
					try{
						BulletinAdapter::instance().click(userId, bulletinId, false);
					}catch(Ice::TimeoutException& ex){
						++timeout_count;
					}
				//	Ice::Context ctx;
				//	ctx.insert( pair<string, string>("type", "click") );
				//	_prx->userClicked(userId, bulletinId, ctx);
						
				}catch(...){
						FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
						FCGX_Finish_r(&request);
						continue;

				} 
				string txt = "Content-type: text/html\r\n\r\n";
				FCGX_FPrintF(request.out, txt.c_str());
				FCGX_Finish_r(&request);
			}
		};
			
	private:
		FCGX_Request request;
		com::xiaonei::bulletinsearch::BulletinManagerPrx _prx;
	};

	void ParseParam(map<string, string>& mapParam, const string& query_string) {
		vector<string> vecParam;
		size_t pos0 = 0;
		size_t pos1 = query_string.find('&', pos0);
		while ( pos1 != string::npos) {
			string strSeg = query_string.substr(pos0, pos1-pos0);
			vecParam.push_back(strSeg);
			pos0 = pos1+1;
			pos1 = query_string.find('&', pos0);
		}
		if ( pos0 < query_string.length() ) {
			string strSeg = query_string.substr(pos0, query_string.length()-pos0);
			vecParam.push_back(strSeg);
		}
		for ( vector<string>::iterator it = vecParam.begin(); it != vecParam.end(); it++ ) {
			size_t pos = (*it).find('=');
			if ( pos != string::npos ) {
				string key = (*it).substr(0, pos);
				string value = (*it).substr(pos+1, (*it).length()-pos-1);
				if ( key != "" ) {
					mapParam.insert(pair<string, string>(key, value));
				}
			}
		}
	}
}
}

using namespace xce::blt;

int main(int argc, char**argv) {

	FCGX_Init();
	int start = 0;
	vector<IceUtil::ThreadPtr> threads;
	for (int i=0;i<20;++i) {
		threads.push_back(new Worker4ClickThread());
	}
	for (int i=0;i<20;++i) {
		threads.at(i)->start(128*1024);
	}

	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);
	
	for (;;) {
		int rc = FCGX_Accept_r(&request);
		if (rc < 0) break;
		try {
			int end = time(NULL);
			if(end-start > 60*60){
					start = end;
//					printResult(call_count, timeout_count, "click");
			}
			map<string, string> mapParam;
			string query_string = FCGX_GetParam("QUERY_STRING", request.envp);
			ParseParam(mapParam, query_string);
			map<string, string>::iterator itU = mapParam.find("uid");
			map<string, string>::iterator itI = mapParam.find("bid");

			if ( (itU == mapParam.end()) || (itI == mapParam.end()) ) {
				FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
				FCGX_Finish_r(&request);
				continue;
			} 
			int userId = boost::lexical_cast<int>((*itU).second);
			int bulletinId = boost::lexical_cast<int>((*itI).second);
			++call_count;
			try{
				BulletinAdapter::instance().click(userId, bulletinId, false);		
			}catch(Ice::Exception& ex){
				++timeout_count;
			}
		}catch(...){
				FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
				FCGX_Finish_r(&request);
				continue;
		} 
		string txt = "Content-type: text/html\r\n\r\n";
		FCGX_FPrintF(request.out, txt.c_str());
		FCGX_Finish_r(&request);
	}

	FCGX_Free(&request, 0);
};
