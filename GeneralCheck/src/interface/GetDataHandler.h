#ifndef __GET_DATA_HANDLER_H__
#define __GET_DATA_HANDLER_H__
#include "ServiceI.h"

namespace xce {
	namespace generalcheck {
		class GetDataHandler : public Ice::Object {
			protected:
				GetDataHandler(const std::string& name) : name_(name) { }
			public:
				virtual MyUtil::ObjectMap getData(const MyUtil::LongSeq& ids) = 0;
				std::string name_;	
		};
		
		typedef IceUtil::Handle<GetDataHandler> GetDataHandlerPtr;
	}
}

#endif
