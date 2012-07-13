#ifndef _SOCIALGRAPH_RCD_MESSAGE_SERVICE_H_
#define _SOCIALGRAPH_RCD_MESSAGE_SERVICE_H_

#include <Util.ice>

module xce {
module socialgraph {

interface RcdMessageService {
  void addSgBlock(long source, long block, string biz);
  // remove by target
  void removeRcd(long source, long target);
  //remove by target and biz 
  void removeBizRcd(long source, long target, string biz);
	//remove target of biz type in the biz cache and add into block table
	void removeBizCache(long source, long target, string biz);
};

};
};

#endif


