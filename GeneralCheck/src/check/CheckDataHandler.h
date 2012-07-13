#ifndef _CHECKDATA_HANDLER_H__
#define _CHECKDATA_HANDLER_H__

#include "ServiceI.h"
#include "../interface/GetDataHandler.h"
#include "../interface/CompareHandler.h"

namespace xce 
{
namespace generalcheck
{
	class CheckDataHandler : public Ice::Object
	{
		public:
			CheckDataHandler(const GetDataHandlerPtr getSDataHandler, const std::vector<GetDataHandlerPtr>& getDDataHandler, const CompareHandlerPtr compareHandler, 
							const int sourceCluster, const int desCluster, const int threadsSize = 6, const int getBatchSize = 3000 ) :
							m_sourceCluster( sourceCluster ), m_desCluster( desCluster ), m_threadsSize( threadsSize ), m_getBatchSize( getBatchSize ),
							getSDataHandler_(getSDataHandler), getDDataHandler_(getDDataHandler), compareHandler_(compareHandler) {
			}

			int m_sourceCluster;
			int m_desCluster;
			int m_threadsSize;
			int m_getBatchSize;
			GetDataHandlerPtr getSDataHandler_;
			std::vector<GetDataHandlerPtr> getDDataHandler_;
			CompareHandlerPtr compareHandler_;
	};
	typedef IceUtil::Handle<CheckDataHandler> CheckDataHandlerPtr;

}
}

#endif
