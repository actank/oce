#ifndef __Tracks_h__
#define __Tracks_h__

#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace activetrack {

const static int TRACK = 0;

class Track: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	MyUtil::Str2StrMap get();
	void set(std::string, std::string);
	void remove(std::string&);
private:
	MyUtil::Str2StrMap _track;
};
typedef IceUtil::Handle<Track> TrackPtr;

}
}
#endif
