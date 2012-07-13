#ifndef __IMG_URL_TRANSFER_ICE__
#define __IMG_URL_TRANSFER_ICE__

#include <Util.ice>

module talk
{
	class TransferManager{
		MyUtil::Int2StrMap getTinyUrl(MyUtil::StrSeq urls);
	};
};

#endif
