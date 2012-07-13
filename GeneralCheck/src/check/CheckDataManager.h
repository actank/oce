#ifndef __CHECKDATA_MANAGER_H__
#define __CHECKDATA_MANAGER_H__

#include "ServiceI.h"
#include "AtomicSTL.h"
#include "CheckDataHandler.h"
#include "../IdProducer.h"
#include "../common.h"

namespace xce 
{
namespace generalcheck
{
	class CheckDataManager;
	typedef IceUtil::Handle<CheckDataManager> CheckDataManagerPtr;
	
	class CheckDataThread : public IceUtil::Thread {
		public:
			CheckDataThread(CheckDataHandlerPtr checkDataHandler, xce::atomicstl::BlockingQueue<CheckIdDataPtr>& wrongIds) : m_checkDataHandler( checkDataHandler ), 
				m_wrongIds( wrongIds ) {
				m_ids.setProps( 10000 );
				m_isRunning = false;
			}
		public:
			void invoke ( const MyUtil::IntSeq& ids ) {
				MyUtil::LongSeq lIds( ids.begin(), ids.end() );
				m_ids.pushSeq( lIds );
			}
			bool isRunning() {
				IceUtil::Mutex::Lock lock( m_mutex );
				MCE_INFO("debug CheckDataThread isRunning m_isRunning = " << m_isRunning);
				return m_isRunning;
			}
		protected:
			void run() {
				while(true) {
					MyUtil::LongSeq ids;
					m_ids.popfrontSeq(ids, m_checkDataHandler->m_getBatchSize);
					{
						IceUtil::Mutex::Lock lock(m_mutex);
						m_isRunning = true;
						if( ids.size() > 0 ) {
							MCE_INFO("debug check id " << ids.at( ids.size() -1 ) << " size " << ids.size());
						}
					}

					MyUtil::ObjectMap sourceData = m_checkDataHandler->getSDataHandler_->getData(ids);
					//MyUtil::ObjectMap desData;

					for(vector<GetDataHandlerPtr>::const_iterator it = m_checkDataHandler->getDDataHandler_.begin(); it != m_checkDataHandler->getDDataHandler_.end();
						++it) {
						MyUtil::ObjectMap desData = (*it)->getData(ids);
						MyUtil::LongSeq allWrongIds;
						try {
							allWrongIds = m_checkDataHandler->compareHandler_->compare(sourceData, desData, m_checkDataHandler->getSDataHandler_->name_, (*it)->name_);
						} catch( ... ) {
							MCE_INFO("debug CheckDataThread::run Exception!");
							allWrongIds.swap(ids);
						}
	
						if(allWrongIds.size() > 0) {
							logAndRepair(allWrongIds);
						}
					}

					{
						IceUtil::Mutex::Lock lock(m_mutex);
						m_isRunning = false;
					}
				
				}
			}

		private:
			void logAndRepair( const MyUtil::LongSeq& wrongIds ) {
				CheckIdDataSeq res;

				for( int i = 0; i < m_checkDataHandler->m_desCluster; ++i ) {
					res.push_back( new CheckIdData( MyUtil::LongSeq(), i ) );
				}
				
				for( MyUtil::LongSeq::const_iterator it = wrongIds.begin(); it != wrongIds.end(); ++it ) {
					res.at( (*it) % m_checkDataHandler->m_desCluster )->m_ids.push_back( *it );
				}
				for( CheckIdDataSeq::const_iterator it = res.begin(); it != res.end(); ++it ) {
					if( (*it)->m_ids.size() <= 0 ) {
						continue;
					}

					try {
						//do nothing
						//m_checkDataHandler->repair( ( *it )->m_ids, (*it)->m_mod );
					} catch( ... ) {
						MCE_INFO("debug repair failed");
						continue;
					}
				}
				m_wrongIds.pushSeq( res );
			}

		private:
			CheckDataHandlerPtr m_checkDataHandler;
			xce::atomicstl::BlockingQueue<long> m_ids;
			xce::atomicstl::BlockingQueue<CheckIdDataPtr>& m_wrongIds;
			IceUtil::Mutex m_mutex;
			bool m_isRunning;
	};

	typedef IceUtil::Handle<CheckDataThread> CheckDataThreadPtr;
	
	class CheckDataManager : public Ice::Object
	{
		public:
			CheckDataManager( CheckDataHandlerPtr checkDataHandler , IdProducerPtr producer) : m_checkDataHandler( checkDataHandler ), m_producer( producer ) {
				m_wrongIds.setProps( 10000000 );
				int threadSize = m_checkDataHandler->m_threadsSize < 6 ? m_checkDataHandler->m_threadsSize : 6;
				for( int i = 0; i < threadSize; ++i ) {
					CheckDataThreadPtr checkDataThread = new CheckDataThread( m_checkDataHandler , m_wrongIds );
					checkDataThread->start( 1024 * 1024 ).detach();
					m_threads.push_back( checkDataThread );
				}
			}

		public:
			void check() {
				srand(time(NULL));
				int beginId = 0;
				int maxId = 0;
				while(true) {
					beginId = maxId;
					MyUtil::IntSeq ids;
					try{
						ids = m_producer->produce(beginId, m_checkDataHandler->m_getBatchSize, maxId);
					} catch( ... ){
						//MCE_WARN( "CheckManager::check Exception!");
						MCE_INFO("debug CheckDataManager::check Exception!");
						maxId = beginId + 1;
						continue;
					}

					if( ids.size() != 0 ) {
						m_threads.at( random() % m_threads.size() )->invoke( ids );
					}
				
					if( beginId == maxId ) {
						while( true ) {
							bool waitFlag = false;
							for(std::vector<CheckDataThreadPtr>::const_iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
								if((*it)->isRunning() == true) {
									waitFlag = true;
								}
							}
							if(waitFlag) {
								sleep(1);
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
			void log() {
				CheckIdDataSeq res;
				int wrongNum = 0;
				m_wrongIds.popfrontSeq(res, m_wrongIds.getSize());
				for(CheckIdDataSeq::const_iterator it = res.begin(); it != res.end(); ++it) {
					if((*it)->m_ids.size() <= 0) {
						continue;
					}
					MCE_INFO("warn mod " << (*it)->m_mod);
					for( MyUtil::LongSeq::const_iterator iIt = ( *it )->m_ids.begin(); iIt != (*it)->m_ids.end(); ++iIt) {
						MCE_INFO("warn " << ( *iIt ));
						++wrongNum;
					}
				}
				MCE_INFO("warn Wrong ids size " << wrongNum << "!");
			}

		private:
			CheckDataHandlerPtr m_checkDataHandler;
			IdProducerPtr m_producer;
			std::vector<CheckDataThreadPtr> m_threads;
			xce::atomicstl::BlockingQueue<CheckIdDataPtr> m_wrongIds;
	};
}
}

#endif
