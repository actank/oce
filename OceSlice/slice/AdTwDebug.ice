#ifndef __AD_TW_DEBUG_ICE__
#define __AD_TW_DEBUG_ICE__

#include <Util.ice>

module xce{
module ad{ 

interface AdTwDebug
{
    MyUtil::LongSeq UrlMaping(string referer, int stage);
};

};
};

#endif


