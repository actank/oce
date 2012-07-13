#ifndef IMGURLTRANSFERADAPTER_H_
#define IMGURLTRANSFERADAPTER_H_

#include "AdapterI.h"
#include "ImgUrlTransfer.h"
#include "Singleton.h"

namespace talk
{
namespace adapter
{
using namespace ::talk;
using namespace MyUtil;
class ImgUrlTransferAdapter : public MyUtil::AdapterI, public AdapterISingleton<MyUtil::TalkChannel, ImgUrlTransferAdapter>
{
public:
	
	MyUtil::Int2StrMap getTinyUrl(const MyUtil::StrSeq& urls);
	
protected:
	virtual string name() {
		return "ImgUrlTransfer";
	};
	virtual string endpoints() {
		return "@ImgUrlTransfer";
	};
	virtual size_t cluster() {
		return 1;
	};

	TransferManagerPrx getManager(int id);
	

	vector<TransferManagerPrx> _managers;
	
};

};
};

#endif /*IMGURLTRANSFERADAPTER_H_*/
