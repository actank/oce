#include "ImgUrlTransferAdapter.h"

using namespace talk;
using namespace MyUtil;
using namespace talk::adapter;

TransferManagerPrx ImgUrlTransferAdapter::getManager(int id) {
	return locate<TransferManagerPrx>(_managers, "M", id, TWO_WAY);
}

MyUtil::Int2StrMap ImgUrlTransferAdapter::getTinyUrl(const MyUtil::StrSeq& urls){
	if(urls.empty()){
		return MyUtil::Int2StrMap();
	}
	return _managers.at(urls.size() % _cluster)->getTinyUrl(urls);
}

