
#include "fcgi_config.h"
#include "fcgiapp.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include "OceCxxAdapter/src/QuestCacheAdapter.h"

using namespace std;

namespace xce {
namespace quest {	
	void ParseParam(map<string, string>& mapParam, const string& query_string);
	bool doRequest(FCGX_Request& request){
		map<string, string> mapParam;
		string query_string = FCGX_GetParam("QUERY_STRING", request.envp);
		ParseParam(mapParam, query_string);
		map<string, string>::iterator itU = mapParam.find("uid");
		map<string, string>::iterator itT = mapParam.find("tid");
		if(itU == mapParam.end()||itT == mapParam.end()){
			return false;
		}
		int userId,tipId;
		try{
			userId = boost::lexical_cast<int>((*itU).second);
			tipId = boost::lexical_cast<int>((*itT).second);
		}catch(...){
			return false;
		}
		try{
			QuestCacheAdapter::instance().click(tipId, userId);
		}catch(...){
			return false;
		}
		FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
		FCGX_Finish_r(&request);
		return true;

	}	
	class Worker4CloseThread : virtual public IceUtil::Thread {
	public:
		Worker4CloseThread() {
			FCGX_InitRequest(&request, 0, 0);
		}
		~Worker4CloseThread(){
			FCGX_Free(&request, 0);
		}
		virtual void run(){
			for (;;) {
				int rc = FCGX_Accept_r(&request);
				if (rc < 0) break;
				if(!doRequest(request)){
					FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
					FCGX_Finish_r(&request);
				}
			}
		};
			
	private:
		FCGX_Request request;
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
using namespace xce::quest;

int main(int argc, char**argv) {

	FCGX_Init();

	vector<IceUtil::ThreadPtr> threads;
	for (int i=0;i<20;++i) {
		threads.push_back(new Worker4CloseThread());
	}
	for (int i=0;i<20;++i) {
		threads.at(i)->start(128*1024);
	}

	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);
	
	for (;;) {
		int rc = FCGX_Accept_r(&request);
		if (rc < 0) break;
		if(!doRequest(request)){
			FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
			FCGX_Finish_r(&request);
		}

		
	}
	FCGX_Free(&request, 0);
};
