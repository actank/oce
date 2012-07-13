#include "fcgiapp.h"
#include "fcgi_config.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ILikeItLogicAdapter.h"
#include "ILikeItCacheAdapter.h"
#include <boost/lexical_cast.hpp>
#include <DbDescriptor.h>
#include "TicketAdapter.h"
#include "app/ILikeIt/share/ILikeItUtil.h"
#include "ILikeFCGIUtil.h"
#include <boost/detail/atomic_count.hpp>
#include "ActiveTrackAdapter.h"

//boost::detail::atomic_count call_count_(0);
//boost::detail::atomic_count tout_count_(0);

namespace xce {
namespace ilikeit {	
	using namespace passport;

	string err = "Content-type: text/html\r\n\r\n{code:1,msg:\"服务器繁忙\"}";
	string exist = "Content-type: text/html\r\n\r\n{code:1,msg:\"您已经赞过了\"}";
	string yes = "Content-type: text/html\r\n\r\n{code:0,msg:\"\",";
	int count=0;

	void ParseParam(map<string, string>& mapParam, const string& query_string);

	// post信息給api组
	class PostAddTask : public MyUtil::Task {
	public:
		PostAddTask(const map<string, string>& mapParam, int likeCount):
			_mapParam(mapParam), _likeCount(likeCount) {
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
			cmd << "\" http://like.renren.com/platformaddlike";
			system(cmd.str().c_str());
		}
	private:
		map<string, string> _mapParam;
		int _likeCount;
	};

	//string UserDetailSeq2Str(UserDetailSeq likeList);
	//string LikeInfo2JSON(LikeInfoPtr lip);
	//string ParseCookie(const string &http_cookie );
	//string photo = "http://head.xiaonei.com/photos/0/0/men_tiny.gif";

	bool doRequest(FCGX_Request& request){
		std::string ret;
		string m;
		map<string, string> mapParam;
		map<string, string>::iterator itU ;
		map<string, string>::iterator itG ;
		map<string, string>::iterator itO ;
		map<string, string>::iterator itN ;
		map<string, string>::iterator itT ;
		map<string, string>::iterator itGG ;
		LikeInfo2Ptr lip;
		int uid;
		int ownerId;
		int typeId;
		string  query_string = ILikeFCGIUtil::GetParam(request);
		string  http_cookie = FCGX_GetParam("HTTP_COOKIE", request.envp);
		ILikeFCGIUtil::ParseParam(query_string, mapParam);
		itU = mapParam.find("uid");
		itG = mapParam.find("gid");
		itO = mapParam.find("owner");
		itT = mapParam.find("type");
		itN = mapParam.find("name");
		itGG = mapParam.find("Gid");

		if ( (itU == mapParam.end()) || (itG == mapParam.end()) ||(itT == mapParam.end())
				|| (itO == mapParam.end())|| (itN == mapParam.end()) ) {
			return false;
		}
		try {
			uid = boost::lexical_cast<int>((*itU).second);
			ownerId=boost::lexical_cast<int>((*itO).second);
			typeId=boost::lexical_cast<int>((*itT).second);
			if (uid != TicketAdapter::instance().verifyTicket(ILikeFCGIUtil::ParseCookie(http_cookie),m)) 
				return false;
			LikeInfo2Ptr tmp=ILikeItCacheAdapter::instance().getLikeDetail2(itG->second,uid,0);
			if(tmp->selfCount==0){
				Ice::Context ctx;
				ctx["ownerId"]=itO->second;
				ctx["typeId"]=itT->second;
				ctx["name"]=itN->second;
				ctx["gid"]=itG->second;
				ostringstream oss;
				oss<<itG->second<<"_"<<uid;
				xce::log::ActiveTrack::instance().action(ownerId, "ILIKEIT", "ADD_FOOTPRINT", oss.str());
				lip = ILikeItLogicAdapter::instance().addLikeAndGet2(itG->second, uid);
				ret = ILikeFCGIUtil::LikeInfo2JSON(uid, lip );

				// Post这个Add的信息
				// TaskManager::instance().execute(new PostAddTask(itG->second, (*itU).second, (*itO).second, lip->totalCount));
				TaskManager::instance().execute(new PostAddTask(mapParam, lip->totalCount));

			}else{
				FCGX_FPrintF(request.out, exist.c_str());
				FCGX_Finish_r(&request);
				return true;
			}
		} catch(...){
			return false;
		}
		ostringstream txt;
		txt <<yes<<ret;



		if(uid!=ownerId && itGG!=mapParam.end()) {
			ILikeItUtil::instance().sendWebPager(ownerId,uid,lip->totalCount,itGG->second,itN->second);	
		}
		FCGX_FPrintF(request.out,ILikeFCGIUtil::convert(txt.str()).c_str());
		FCGX_Finish_r(&request);

		return true;
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
					try{
						if(!doRequest(request)){
							FCGX_FPrintF(request.out, err.c_str());
							FCGX_Finish_r(&request);	  
						}
					}catch(...){
						FCGX_FPrintF(request.out, err.c_str());
						FCGX_Finish_r(&request);	  
					}
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
	xce::log::ActiveTrack::instance().initialize();
	ILikeItUtil::instance().init();
	

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
		try{
			if(!doRequest(request)){
				FCGX_FPrintF(request.out, err.c_str());
				FCGX_Finish_r(&request);	  
			}
		}catch(...){
			FCGX_FPrintF(request.out, err.c_str());
			FCGX_Finish_r(&request);	  
		}
	}
	FCGX_Free(&request, 0);
};

