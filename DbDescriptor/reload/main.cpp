
#include <iostream>
#include "DbDescriptor.h"
#include "Channel.h"

int main(){
	MyUtil::OceChannel channel;
	Ice::CommunicatorPtr ic = channel.getCommunicator();
	com::xiaonei::xce::DbDescriptorPrx proxy = com::xiaonei::xce::DbDescriptorPrx::checkedCast(ic->stringToProxy("DCS@DbDescriptor"));
	std::cout << "reloading...";
	proxy->reload();
	std::cout << "done." << std::endl;
	ic->destroy();
	return 1;
}
