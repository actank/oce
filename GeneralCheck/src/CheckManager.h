#ifndef _CHECK_MANAGER_H__
#define _CHECK_MANAGER_H__

#include "common.h"
#include "ServiceI.h"
#include "CheckHandler.h"
#include "IdProducer.h"
#include "AtomicSTL.h"

namespace xce 
{
namespace generalcheck
{

	class CheckManager;
	typedef IceUtil::Handle<CheckManager> CheckManagerPtr;
	
	class CheckThread : public IceUtil::Thread
	{
		public:

			;
			CheckThread( CheckHandlerPtr checkHandler , xce::atomicstl::BlockingQueue<CheckIdDataPtr>& wrongIds) : m_checkHandler( checkHandler ), m_wrongIds( wrongIds )
			{
				m_ids.setProps( 10000 );
				m_isRunning = false;
			}
	
		public:

			void invoke ( const MyUtil::IntSeq& ids )
			{
				MyUtil::LongSeq lIds( ids.begin(), ids.end() );
				m_ids.pushSeq( lIds );
			}

			bool isRunning()
			{
				IceUtil::Mutex::Lock lock( m_mutex );
				std::cout << "debug CheckThread isRunning m_isRunning = " << m_isRunning << std::endl;
				return m_isRunning;
			}
		
		protected:
			
			void run()
			{
				while( true )
				{
					MyUtil::LongSeq ids;

					m_ids.popfrontSeq( ids, m_checkHandler->m_getBatchSize );
					{
						IceUtil::Mutex::Lock lock( m_mutex );
						m_isRunning = true;
						if( ids.size() > 0 )
						{
							std::cout<<"debug check id "<<ids.at( ids.size() -1 )<<" size "<<ids.size()<<std::endl;
						}
					}

					CheckIdDataSeq distIds[2];
					distributeIds( ids , distIds );

					MyUtil::ObjectMap sourceData;
					MyUtil::ObjectMap desData;

					for( CheckIdDataSeq::const_iterator it = distIds[0].begin(); it != distIds[0].end(); ++it )
					{
						try
						{
							MyUtil::ObjectMap tempData = m_checkHandler->getSource( (*it)->m_ids, (*it)->m_mod );
							sourceData.insert( tempData.begin(), tempData.end() );
						}catch( ... )
						{
							std::cout << "debug CheckThread::run getSource Exception!" << std::endl;
						}
					}
					
					for( CheckIdDataSeq::const_iterator it = distIds[1].begin(); it != distIds[1].end(); ++it )
					{
						try
						{
							MyUtil::ObjectMap tempData = m_checkHandler->getDes( (*it)->m_ids, (*it)->m_mod );
							desData.insert( tempData.begin(), tempData.end() );
						}catch( ... )
						{
							std::cout << "debug CheckThread::run getDes Exception!" << std::endl;
						}
					}

					MyUtil::LongSeq allWrongIds;
					
					try
					{
						allWrongIds = m_checkHandler->compare( sourceData, desData );
					}catch( ... )
					{
						//MCE_WARN("CheckThread::run Exception!");
						std::cout<<"debug CheckThread::run Exception!"<<std::endl;
						allWrongIds.swap( ids );
					}

					if( allWrongIds.size() > 0 )
					{
						logAndRepair( allWrongIds );
					}

					{
						IceUtil::Mutex::Lock lock( m_mutex );
						m_isRunning = false;
					}
				
				}
			}

		private:

			void distributeIds( const MyUtil::LongSeq& ids , CheckIdDataSeq* res )
			{
				
				for( int i = 0; i < m_checkHandler->m_sourceCluster; ++i )
				{
					res[0].push_back( new CheckIdData( MyUtil::LongSeq(), i ) );
				}
				
				for( int i = 0; i < m_checkHandler->m_desCluster; ++i )
				{
					res[1].push_back( new CheckIdData( MyUtil::LongSeq(), i ) );
				}
				
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					res[0].at( ( *it ) % m_checkHandler->m_sourceCluster )->m_ids.push_back( *it );
					res[1].at( ( *it ) % m_checkHandler->m_desCluster )->m_ids.push_back( *it );
				}
			}

			void logAndRepair( const MyUtil::LongSeq& wrongIds )
			{
				CheckIdDataSeq res;

				for( int i = 0; i < m_checkHandler->m_desCluster; ++i )
				{
					res.push_back( new CheckIdData( MyUtil::LongSeq(), i ) );
				}
				
				for( MyUtil::LongSeq::const_iterator it = wrongIds.begin(); it != wrongIds.end(); ++it )
				{
					res.at( (*it) % m_checkHandler->m_desCluster )->m_ids.push_back( *it );
				}
				for( CheckIdDataSeq::const_iterator it = res.begin(); it != res.end(); ++it )
				{
					if( (*it)->m_ids.size() <= 0 )
					{
						continue;
					}

					try
					{
						m_checkHandler->repair( ( *it )->m_ids, (*it)->m_mod );
					}catch( ... )
					{
						std::cout << "debug repair failed" << std::endl;
						continue;
					}
				}
				
				m_wrongIds.pushSeq( res );
			
			}

		private:

			CheckHandlerPtr m_checkHandler;
			
			xce::atomicstl::BlockingQueue<long> m_ids;
			
			xce::atomicstl::BlockingQueue<CheckIdDataPtr>& m_wrongIds;
			
			IceUtil::Mutex m_mutex;

			bool m_isRunning;

	};
	typedef IceUtil::Handle<CheckThread> CheckThreadPtr;
	
	class CheckManager : public Ice::Object
	{
		public:

			CheckManager( CheckHandlerPtr checkHandler , IdProducerPtr producer) : m_checkHandler( checkHandler ), m_producer( producer )
			{
				m_wrongIds.setProps( 1000000000 );
				int threadSize = m_checkHandler->m_threadsSize < 6 ? m_checkHandler->m_threadsSize : 6;
				for( int i = 0; i < threadSize; ++i )
				{
					CheckThreadPtr checkThread = new CheckThread( m_checkHandler , m_wrongIds );
					checkThread->start( 1024 * 1024 ).detach();
					m_threads.push_back( checkThread );
				}
			}

		public:

			void check()
			{
				srand( time( NULL ) );
				int beginId = 0;
				int maxId = 0;
				while( true )
				{
					beginId = maxId;
					MyUtil::IntSeq ids;
					try{
						ids = m_producer->produce(beginId,m_checkHandler->m_getBatchSize,maxId);
					} catch( ... ){
						//MCE_WARN( "CheckManager::check Exception!");
						std::cout << "debug CheckManager::check Exception!" << std::endl;
						maxId = beginId +1;
						continue;
					}

					if( ids.size() != 0 )
					{
						m_threads.at( random() % m_threads.size() )->invoke( ids );
					}
				
					if( beginId == maxId )
					{
						while( true )
						{
							bool waitFlag = false;
							for( std::vector<CheckThreadPtr>::const_iterator it = m_threads.begin(); it != m_threads.end(); ++it )
							{
								if( (*it)->isRunning() == true )
								{
									waitFlag = true;
								}
							}
							if( waitFlag )
							{
								sleep( 1 );
								continue;
							}
							break;
						}
						log();
						break;
					}
				}
			}

		private:

			void log()
			{
				CheckIdDataSeq res;

				int wrongNum = 0;

				if(m_wrongIds.isEmpty())
				{
					std::cout << "warn Wrong ids size " << wrongNum << "!" << std::endl;
					return;
				}
				m_wrongIds.popfrontSeq( res, m_wrongIds.getSize() );

				for( CheckIdDataSeq::const_iterator it = res.begin(); it != res.end(); ++it )
				{
					if( (*it)->m_ids.size() <= 0 )
					{
						continue;
					}

					std::cout << "warn mod " << ( *it )->m_mod << std::endl;
					for( MyUtil::LongSeq::const_iterator iIt = ( *it )->m_ids.begin(); iIt != ( *it )->m_ids.end(); ++iIt )
					{
						std::cout << "warn " << ( *iIt ) << std::endl;
						++wrongNum;
					}
				}

				std::cout << "warn Wrong ids size " << wrongNum << "!" << std::endl;
				
			}

		private:

			CheckHandlerPtr m_checkHandler;
			
			IdProducerPtr m_producer;

			std::vector<CheckThreadPtr> m_threads;
			
			xce::atomicstl::BlockingQueue<CheckIdDataPtr> m_wrongIds;

	};

}
}

#endif
