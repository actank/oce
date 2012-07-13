#include "HotWordAdapter.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
//using namespace mop::hi::svc::adapter;
using namespace std;
using namespace xce::hotwords;
using namespace MyUtil;
using namespace com::xiaonei::search5::logicserver;

//---------------------------------------------------------------------------
HotWordManagerPrx HotWordAdapter::getHotWordManager(int id) {
	return locate<HotWordManagerPrx>(_managers, "M", id, TWO_WAY);
}
void HotWordAdapter::updateDoings(const IndexUpdateDataSeq& infos){
	try{
		MCE_INFO("[HotWordAdapter] update info size "<< infos.size());
		DoingItemSeq items;
		items.reserve(infos.size());
		for(IndexUpdateDataSeq::const_iterator it=infos.begin();it!=infos.end();++it){
			if((*it).operationType == 2)
				continue;
			DoingItem dp;
			map<string,string>::const_iterator itt;

			itt = it->index.find("id");
			if(itt!=it->index.end())
				dp.id = boost::lexical_cast<long>(itt->second);

			itt = it->index.find("userid");
			if(itt!=it->index.end())
				dp.uid= boost::lexical_cast<long>(itt->second);

			itt = it->index.find("content");
			if(itt!=it->index.end())
				dp.content = itt->second;

			itt = it->index.find("rootContent");
			if(itt!=it->index.end())  {
				dp.content += ";";
				dp.content += itt->second;
			}
			dp.time = time(NULL)*1000L;
			items.push_back(dp);
			MCE_INFO("[HotWordAdapter] update "<<dp.id<<" "<<dp.uid<<" "<<dp.content);
		}
	getHotWordManager(0)->addDoings(items);
	}catch(std::exception& ex){
		MCE_WARN("[HotWordAdapter::updateDoings] Exception : "<<ex.what());
	}catch(...){
		MCE_WARN("[HotWordAdapter::updateDoings] Exception : Unknown Exception");
	}
}

