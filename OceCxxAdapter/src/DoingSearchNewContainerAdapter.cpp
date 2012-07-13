#include "DoingSearchNewContainerAdapter.h"
#include <iostream>
#include <Ice/Ice.h>

using namespace xce::doingsearch;
using namespace std;
using namespace MyUtil;

//---------------------------------------------------------------------------

DoingSearchNewContainerAdapter::DoingSearchNewContainerAdapter(){
}

DoingSearchContainerPrx DoingSearchNewContainerAdapter::getDoingSearchContainer() {	
	return locate<DoingSearchContainerPrx>(name(), endpoints(), TWO_WAY, 300);
}

DoingSearchContainerPrx DoingSearchNewContainerAdapter::getDoingSearchContainerOneway() {
	return locate<DoingSearchContainerPrx>(name(), endpoints(), ONE_WAY, 300);
}

void DoingSearchNewContainerAdapter::addDoing(DoingItem item){
	MCE_DEBUG("DoingSearchNewContainerAdapter::addDoing");
	try{
		getDoingSearchContainerOneway()->addDoing(item);
	}catch(Ice::Exception& e){
		MCE_WARN("DoingSearchNewContainerAdapter::addDoing "<< e);
	}catch(...){
		MCE_WARN("DoingSearchNewContainerAdapter::addDoing exception");
	}
}

string DoingSearchNewContainerAdapter::name(){
	return "M";
}

string DoingSearchNewContainerAdapter::endpoints(){
	return "@DoingSearchNewContainer";
}

size_t DoingSearchNewContainerAdapter::cluster() {
	return 0;
}
