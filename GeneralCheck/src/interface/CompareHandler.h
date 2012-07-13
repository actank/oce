#ifndef __COMPAREHANDLER_H__
#define __COMPAREHANDLER_H__

#include "ServiceI.h"

namespace xce {
namespace generalcheck {
class CompareHandler : public Ice::Object {
public:
	virtual MyUtil::LongSeq compare(const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData, 
									const string& sourceName, const string& desName) = 0;	
};
typedef IceUtil::Handle<CompareHandler> CompareHandlerPtr;
}
}
#endif
