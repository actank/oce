#include "TalkDeliverAdapter.h"
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"

using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::util;


DeliverManagerPrx TalkDeliverAdapter::getManager(int id) {
	return locate<DeliverManagerPrx>(_managers, "M", id, TWO_WAY);
}

DeliverManagerPrx TalkDeliverAdapter::getManagerOneway(int id) {
	return locate<DeliverManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//----------------------------------------

void TalkDeliverAdapter::deliver(const MessageSeq& seq) {
	if(seq.empty()){
		return;
	}
	MessageSeq split_seq;
	int len_msgs = seq.size();
	int total_size = 800000;
	int now_size = 0;
//	MCE_INFO("judge TalkDeliverAdapter::deliver len_msgs-->" << len_msgs);
	for(size_t i = 1;i <= len_msgs; ++i)
	{
		split_seq.push_back(seq[i-1]);
		if(now_size + 60 + seq[i-1]->msg.size() < total_size)
		{
			now_size += 60 + seq[i-1]->msg.size();
			if(i==len_msgs){
//				MCE_INFO("judge TalkDeliverAdapter::deliver when i == len_msgs split_seq-->" << split_seq.size() << "now_size" << now_size);
				getManagerOneway(seq.at(0)->to->userId)->deliver(split_seq);
			}
		}
		else{
//			MCE_INFO("judge TalkDeliverAdapter::deliver  send split_seq-->" << split_seq.size() << "now_size" << now_size);
			getManagerOneway(seq.at(0)->to->userId)->deliver(split_seq);
			now_size = 0;
			split_seq.clear();
		}
	}
}

void TalkDeliverAdapter::deliver(const MessagePtr& msg) {
	if(!msg){
		return;
	}
	MessageSeq seq;
	seq.push_back(msg);
	getManagerOneway(seq.at(0)->to->userId)->deliver(seq);
}

//OfflineMessageSeq TalkDeliverAdapter::queryOfflineMessageSeq(Ice::Int userId) {
//	return getManager(userId)->queryOfflineMessageSeq(userId);


