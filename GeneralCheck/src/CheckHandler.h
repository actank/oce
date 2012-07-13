#ifndef _CHECK_HANDLER_H__
#define _CHECK_HANDLER_H__

#include "ServiceI.h"

namespace xce 
{
namespace generalcheck
{
	class CheckHandler : public Ice::Object
	{
		protected:
		
			CheckHandler( const int sourceCluster, const int desCluster, const int threadsSize = 6, const int getBatchSize = 3000 ) : m_sourceCluster( sourceCluster ), m_desCluster( desCluster ), m_threadsSize( threadsSize ), m_getBatchSize( getBatchSize )
			{
			}

		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod ) = 0;
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod ) = 0;
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData ) = 0;

			virtual void repair( const MyUtil::LongSeq& ids, const int mod ) = 0;

		public:

			int m_sourceCluster;
			int m_desCluster;
			int m_threadsSize;
			int m_getBatchSize;
	};
	typedef IceUtil::Handle<CheckHandler> CheckHandlerPtr;

}
}

#endif
