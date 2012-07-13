#include "UGCShareLogicAdapter.h"

using namespace com::ugcshare;

UGCShareLogicPrx UGCShareLogicAdapter::getManager(int id){
	return locate<UGCShareLogicPrx>(_managers, "W", id, TWO_WAY);
}
void UGCShareLogicAdapter::createShareToShare(Ice::Long id,int ownerid,int hostid, const string& ip, const string& comment){
	getManager()->createShareToShare(id, ownerid, hostid, ip, comment);	
}
void UGCShareLogicAdapter::createInternalShare(Ice::Long id, int ownerid, int type, int hostid, const string& ip, const string& comment){
	getManager()->createInternalShare(id, ownerid, type, hostid, ip, comment);
}
void UGCShareLogicAdapter::createExternalShare(int hostid, const string& link, const string& ip, const string& comment){
	getManager()->createExternalShare(hostid, link, ip, comment);
}


