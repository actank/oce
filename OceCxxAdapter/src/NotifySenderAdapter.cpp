#include "NotifySenderAdapter.h"
using namespace xce::notifysender::adapter;
using namespace std;

//---------------------------------------------------------------------------

void NotifySenderAdapter::sendByMessage(int toId, string message){
	getNotifySenderManagerOneway(toId)->sendByMessage(toId,message);
}
void NotifySenderAdapter::sendByAIGuideStage(int toId, AIGuideStage stage){
	getNotifySenderManagerOneway(toId)->sendByAIGuideStage(toId,stage);
}

NotifySenderManagerPrx NotifySenderAdapter::getNotifySenderManagerOneway(int id){
	return locate<NotifySenderManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}
