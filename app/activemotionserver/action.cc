#include "action.h"
//#include <locallogger.h>

using namespace cwf::socialgraph;

xce::ThreadPool& GetThreadPool(){
        static xce::ThreadPool _worker_(1,1);
	return _worker_;
}

cwf::HttpStatusCode ClickAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		//WorkerThread::GetInstance().Post( new WorkMessage( TYPE_CLICK, request->get("J") ) );
		GetThreadPool().Post( new WorkMessage( TYPE_CLICK, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> ClickAction create Task for \'J\' failed";
		//FCGI_WARN( "==> ClickAction create Task for \'J\' failed");
	}
	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->OutputHeader();
	response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	return cwf::HC_OK;
}

cwf::HttpStatusCode ScrollBottomAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_SCROLLBOTTOM, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> ScrollBottomAction create Task for \'J\' failed";
		//FCGI_WARN( "==> ScrollBottomAction create Task for \'J\' failed");
	}
	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->OutputHeader();
	//response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	return cwf::HC_OK;
}

cwf::HttpStatusCode FocusAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_FOCUS, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> FocusAction create Task for \'J\' failed";
		//FCGI_WARN( "==> FocusAction create Task for \'J\' failed");
	}
	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->OutputHeader();
	//response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	return cwf::HC_OK;
}

cwf::HttpStatusCode UnfocusAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_UNFOCUS, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> UnfocusAction create Task for \'J\' failed";
		//FCGI_WARN( "==> UnfocusAction create Task for \'J\' failed");
	}
	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->OutputHeader();
	//response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	return cwf::HC_OK;
}

cwf::HttpStatusCode AppHoldAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_APPHOLD, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> AppHoldAction create Task for \'J\' failed";
		//FCGI_WARN( "==> AppHoldAction create Task for \'J\' failed");
	}

	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->WriteRaw("{\"comScore\":\"pageview_candidate\"}");
	response->OutputHeader();
	return cwf::HC_OK;
}

cwf::HttpStatusCode OtherAction::Process(cwf::Request* request, cwf::Response* response)
{
	try{
		GetThreadPool().Post( new WorkMessage( TYPE_OTHER, request->get("J") ) );
	} catch(...){
		LOG(WARNING) << "==> OtherAction create Task for \'J\' failed";
		//FCGI_WARN( "==> OtherAction create Task for \'J\' failed");
	}

	response->header().set_status_code(cwf::HC_OK, "OK");
	response->header().Add(cwf::HH_CONTENT_TYPE, DjDefaultContentType);
	response->OutputHeader();
	return cwf::HC_OK;
}

