#include "action.h"
#include "base/logging.h"
#include <boost/lexical_cast.hpp>
#include "OceCxxAdapter/src/TipsAdapter.h"

using namespace xce::tps;
using namespace std;

/*
xce::ThreadPool& GetThreadPool(){
        static xce::ThreadPool _worker_(1,1);
	return _worker_;
}
*/

cwf::HttpStatusCode ShowAction::Process(cwf::Request* request, cwf::Response* response)
{
	try {
		string strU = request->get("uid");
		string strS = request->get("site");
		LOG(INFO) << "==> show uid=" << strU << " site=" << strS;

		if ( (strU == "") || (strS == "") ) {
			response->header().set_status_code(cwf::HC_OK, "OK");
			response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
			response->OutputHeader();
			return cwf::HC_OK;
		}

		int userId = boost::lexical_cast<int>(strU);
		int website = strS=="renren"? 1:0;

		TipsPtr ptr;
		try{
			ptr = TipsAdapter::instance().show(userId, website);
		}catch(...){
			LOG(WARNING) << "==> show uid=" << strU << " site=" << strS << " TipsAdapter error";
		}

		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		if(ptr){
			string id = boost::lexical_cast<string>(ptr->tipsid);
			if(id.size()<=11){
				id = id.append(11-id.size(), ' ');
				response->WriteRaw(id+ptr->text);
				LOG(INFO) << "==> show uid=" << strU << " site=" << strS << " done: " << id+ptr->text;
			}
		}
		return cwf::HC_OK;
	}catch(...){
		LOG(WARNING) << "==> show FCGI error";
		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		return cwf::HC_OK;
	} 

}

cwf::HttpStatusCode ClickAction::Process(cwf::Request* request, cwf::Response* response)
{
	try {
		string strU = request->get("uid");
		string strI = request->get("tid");
		LOG(INFO) << "==> click uid=" << strU << " tid=" << strI;

		if ( (strU == "") || (strI == "") ) {
			response->header().set_status_code(cwf::HC_OK, "OK");
			response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
			response->OutputHeader();
			return cwf::HC_OK;
		}

		int userId = boost::lexical_cast<int>(strU);
		int tipsId = boost::lexical_cast<int>(strI);
		try{
			TipsAdapter::instance().click(userId, tipsId, false);
		}catch(...){
			LOG(WARNING) << "==> click uid=" << strU << " tid=" << strI << " TipsAdapter error";
		}
		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		LOG(INFO) << "==> click uid=" << strU << " tid=" << strI << " done";
		return cwf::HC_OK;

	}catch(...){
		LOG(WARNING) << "==> click FCGI error";
		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		return cwf::HC_OK;
	} 
}

cwf::HttpStatusCode CloseAction::Process(cwf::Request* request, cwf::Response* response)
{
	try {
		string strU = request->get("uid");
		string strI = request->get("tid");
		LOG(INFO) << "==> close uid=" << strU << " tid=" << strI;

		if ( (strU == "") || (strI == "") ) {
			response->header().set_status_code(cwf::HC_OK, "OK");
			response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
			response->OutputHeader();
			return cwf::HC_OK;
		}

		int userId = boost::lexical_cast<int>(strU);
		int tipsId = boost::lexical_cast<int>(strI);
		try{
			TipsAdapter::instance().click(userId, tipsId, true);
		}catch(...){
			LOG(WARNING) << "==> close uid=" << strU << " tid=" << strI << " TipsAdapter error";
		}
		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		LOG(INFO) << "==> close uid=" << strU << " tid=" << strI << " done";
		return cwf::HC_OK;
	}catch(...){
		LOG(WARNING) << "==> close FCGI error";
		response->header().set_status_code(cwf::HC_OK, "OK");
		response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
		response->OutputHeader();
		return cwf::HC_OK;
	} 

}

/*
cwf::HttpStatusCode XXXAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_XXX, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> XXXAction create Task for \'J\' failed";
	}

	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, TipsDefaultContentType);
	response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	response->OutputHeader();
	return cwf::HC_OK;
}
*/

