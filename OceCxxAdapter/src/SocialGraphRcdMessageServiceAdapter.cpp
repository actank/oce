#include "SocialGraphRcdMessageServiceAdapter.h"

namespace xce {
namespace socialgraph {

void RcdMessageServiceAdapter::AddSgBlock(Ice::Long source, Ice::Long block, const std::string& biz) {
  try {
    getProxy(source % 10)->addSgBlock(source, block, biz);
  } catch (...) {
    //Nothing to do
  }
}

void RcdMessageServiceAdapter::RemoveRcdCache(Ice::Long source, Ice::Long target, const::string& biz) {
  if (biz != "NON") {
    try {
      getProxy(source % 10)->removeBizRcd(source, target, biz);
    } catch(...) {
     //nothing to do
    }
  }
  else {
    try {
      getProxy(source % 10)->removeRcd(source, target);
    } catch (...) {
     //nothing to do
    } 
  }
}

void RcdMessageServiceAdapter::AddSgBlockAndRemoveRcdCache(Ice::Long source, Ice::Long block, const std::string& biz) {
  AddSgBlock(source, block, biz);
  RemoveRcdCache(source, block, biz);
}

void RcdMessageServiceAdapter::BecomeFriends(int host, int guest) {
  RemoveRcdCache(host, guest);
  RemoveRcdCache(guest, host);
}

void RcdMessageServiceAdapter::Block(int host, int guest) {
  AddSgBlockAndRemoveRcdCache(host, guest);
}

void RcdMessageServiceAdapter::RemoveBizCache(Ice::Long source, Ice::Long block, const std::string& biz) {
	try {
	  getProxy(source % 10)->removeBizCache(source, block, biz);
	} catch(...) {
	 //nothing to do
	}
}

}}



