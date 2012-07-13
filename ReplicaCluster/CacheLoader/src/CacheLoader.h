#ifndef __LOADER_H__
#define __LOADER_H__

#include <iostream>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>
#include <deque>
#include "UtilCxx/src/AtomicSTL.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClusterStateSubscriber.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClusterClient.h"
#include "CacheLoaderProducer.h"
#include "CacheLoaderFactory.h"
#include "RedoMonitor.h"

namespace xce {
namespace cacheloader {

using namespace xce::cacheloader;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::redomonitor;

//服务代理，负责往服务中写入数据，带有数据队列和写线程
template<class T> class PrxAgent : virtual public IceUtil::Thread {
private:
	void setValid(){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_statusMutex);
		if( !_is_valid ){
			return;
		}
		//else 如果已经写完所有数据，并且可setValid，那么将prx设置valid
 		std::ostringstream tmp;
		tmp<< "[PrxAgent "<<_mod<<" ::setValid] setValid ";
		MCE_INFO( tmp.str() );
		SetValidRedoMonitorPtr setvalidptr = new SetValidRedoMonitor<T>(_prx,false,tmp.str(),true);
		setvalidptr->monitorHandle();
	}
	T _prx;			//直接代理
	std::string _endpoint;	////////////////////////////////////////////////////////////////////////
	int _cluster;		//这三个值在上面直接代理中都有，将其单独取出的好处是读取时少一次远程调用
	int _mod;		////////////////////////////////////////////////////////////////////////
	int _write_batch_size;	//批量写入的个数，由CacheLoader的纯虚函数定义

	int _target_id;		//开始id,也就是下一轮的结束id
	bool _next_loop;	//是否已经到了最后一轮循环
	bool _is_valid;		//是否允许设置valid(设置后不再做setData操作)
        IceUtil::Monitor<IceUtil::Mutex> _statusMutex;				//以上三个变量的读写锁

        std::vector< pair<long,Ice::ObjectPtr> > datas_;
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
public:
	typedef IceUtil::Handle<SetDataRedoMonitor<T> > SetDataRedoMonitorPtr;
        typedef IceUtil::Handle<SetValidRedoMonitor<T> > SetValidRedoMonitorPtr;

	PrxAgent(T& prx, const std::string& endpoint, int cluster, int mod, int write_batch_size) :
		_prx(prx), _endpoint(endpoint), _cluster(cluster), _mod(mod), _write_batch_size(write_batch_size), _target_id(0), _next_loop(false), _is_valid(true) {
	}
	T getPrx(){
		return _prx;
	}
	std::string getEndpoint(){
		return _endpoint;
	}
	int getCluster(){
		return _cluster;
	}
	int getMod(){
		return _mod;
	}
	void setTarget( int target_id ){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _statusMutex );
		_target_id = target_id;
		_next_loop = true;
		_is_valid = false;
	}
	bool isValid(){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _statusMutex );
		return _is_valid;
	}
	//此函数会使以后的add在id>_target_id并且没有下一轮写入时，不再接受新加载数据(_is_valid=true);
	void setNextLoop( bool next_loop ){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _statusMutex );
		_next_loop = next_loop;
	}
	void load( MyUtil::ObjectResultPtr newDatas ){
		if( !newDatas->data.empty() ){
			_prx->setData(newDatas);
		}
        }
        void load( long id, Ice::ObjectPtr newData ){
		MyUtil::ObjectResultPtr data = new MyUtil::ObjectResult;
		if( ( id%_cluster==_mod ) && ( newData!=0 ) ){
			data->data.insert( make_pair<long, Ice::ObjectPtr>( id, newData ) );
			MCE_DEBUG( "[PrxAgent "<<_mod<<" ::load] load" << id );
		}
		if( !data->data.empty() ){
			_prx->setData( data );
		}
        }

	void swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		if (datas_.size() < (size_t)_write_batch_size) mutex_.timedWait(IceUtil::Time::seconds(5));
		datas_.swap(empty_vector);
		mutex_.notifyAll();
	}
	void invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
                if(datas_.size()+1 == (size_t)_write_batch_size){
                	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
			mutex_.notify();
		} else{
			while(datas_.size()+1 > (size_t)(_write_batch_size*2))
			{
				mutex_.wait();
			}
                	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
		}
	}
	//批量版加
	void add( MyUtil::ObjectResultPtr newDatas ){
		for ( std::map<long, Ice::ObjectPtr>::const_iterator it = newDatas->data.begin();
			it!= newDatas->data.end(); ++it ){
			add( it->first, it->second );
		}
        }
	//往队列中添加待写数据
        void add( long id, Ice::ObjectPtr newData ){
		bool toSetValid = false;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_statusMutex);
			//如果valid! 什么都不做
			if( _is_valid ){
				return;
			}
			//如果inValid
				//如果不再有下一轮的数据，且当前加入的id超过了目标id，则不再接受数据，且允许setValid
				//否则维持
			if( ( !_next_loop ) && ( id>=_target_id ) ){
				_is_valid = true;
				toSetValid = true;
				MCE_INFO( "[PrxAgent "<<_mod<<" ::add] final data: " << id << ", to set valid" );
			}
		}

		if( newData==0 || id%_cluster!=_mod ){
			if( toSetValid ){
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
				if( datas_.empty() ){//有可能当前加的对象为空，此时队列并未增加,但是该id可能使得当前prx可以设为valid
					setValid();
				}
			}
		} else{
			invokeDelay( id, newData );
		}
        }

	//写入线程
	virtual void run(){
		MCE_INFO( "[PrxAgent "<<_mod<<" ::run] start" );
		while (true) {
			std::vector< pair<long,Ice::ObjectPtr> > vecTmp;
			swap( vecTmp );

			if( vecTmp.empty() ){
				continue;
			}

			MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult();
			for ( std::vector< pair<long, Ice::ObjectPtr> >::const_iterator it = vecTmp.begin();
					it != vecTmp.end(); ++it ){
				result->data.insert( make_pair<long,Ice::ObjectPtr>(it->first,it->second) );
			}

 			std::ostringstream tmp;
			tmp<< "[PrxAgent "<<_mod<<" ::run] setData: " << vecTmp.size() << " objs";
			MCE_INFO( tmp.str() );
			SetDataRedoMonitorPtr setdataptr = new SetDataRedoMonitor<T>(_prx,false,tmp.str(),result);
                        setdataptr->monitorHandle();
			//_prx->setData( result );

			setValid();
		}
	}
};

//各种Loader服务的基类，集成了预加载功能
template<class T> class CacheLoader : virtual public xce::clusterstate::ClusterClient, public IceUtil::Mutex, virtual public MyUtil::Singleton<CacheLoader<T> > {
protected:
        friend class MyUtil::Singleton<CacheLoader<T> >;
	typedef IceUtil::Handle< PrxAgent<T> > PrxAgentPtr;
	typedef xce::atomicstl::AtomicMap<std::string,PrxAgentPtr> PrxAgentMap;

        PrxAgentMap  _mapPrx;						//服务代理列表
        xce::clusterstate::ClusterStateSubscriberPtr _subscriber;	//注册组件
	ProducerPtr _producer;						//用户定义的id生成器
	CacheLoaderFactoryPtr _cacheloader_factory;			//用户定义的产生对象用的CacheLoaderFactory

        std::string _ctlEndpoint;		//Controller的endpoints
        int _subInterval;			//向Controller注册的间隔
	int _timeout;				//服务直接代理的调用timeout
	int _produce_batch_size;		//批量生成id的个数
	int _consume_batch_size;		//批量生成ObjectResults的个数
	int _write_batch_size;			//批量写入服务的Object个数，将传入PrxAgent
	int _thread_size;			//消费线程数

	bool _valid;		//此Loader服务是否开始提供预加载服务

	class ProduceThread;
	typedef IceUtil::Handle<ProduceThread> ProduceThreadPtr;
        ProduceThreadPtr _produce_thread;				//一个生产线程
	class ConsumeThread;
	typedef IceUtil::Handle<ConsumeThread> ConsumeThreadPtr;
        std::vector<ConsumeThreadPtr> _consume_threads;			//用户自定义数量的消费线程

	CacheLoader():
		_valid(true){
	}
public:
	void setValid(bool valid){
		IceUtil::Mutex::Lock lock( *this );
		_valid = valid;
	}
	void initialize(const ProducerPtr& producer,
			const CacheLoaderFactoryPtr& cacheloader_factory,
			const std::string& ctlEndpoint,
			int subInterval,
			int timeout,
			int produceBatchSize,
			int consumeBatchSize,
			int writeBatchSize,
			int threadSize ){
		MCE_INFO( "[CacheLoader::initialize] initialize start" );
		_producer = producer;
		_cacheloader_factory = cacheloader_factory; 
		_ctlEndpoint = ctlEndpoint;
		_subInterval = subInterval;
		_timeout = timeout;
		_produce_batch_size = produceBatchSize > 0 ? produceBatchSize : 1;
		_consume_batch_size = consumeBatchSize > 0 ? consumeBatchSize : 1;
		_write_batch_size = writeBatchSize > 0 ? writeBatchSize : 1;
		_thread_size = threadSize > 0 ? threadSize : 1;
		MCE_INFO( "[CacheLoader::initialize] initialize: _ctlEndpoint=" << _ctlEndpoint
			<< "  _subInterval=" << _subInterval 
			<< "  _timeout =" << _timeout 
			<< "  _produce_batch_size=" << _produce_batch_size
			<< "  _consume_batch_size=" << _consume_batch_size
			<< "  _write_batch_size=" << _write_batch_size
			<< "  _thread_size=" << _thread_size );
		//生产线程初始化
		_produce_thread = new ProduceThread(
			_mapPrx, _producer, _consume_threads,
			_produce_batch_size, _consume_batch_size, _thread_size );
		_produce_thread->start( 128*1024 ).detach();
		//消费线程初始化
		for( int i = 0; i < _thread_size; ++i ){
			ConsumeThreadPtr consume_thread = new ConsumeThread( 
				i,
				_produce_thread, _mapPrx, _cacheloader_factory,
				_consume_batch_size );
			_consume_threads.push_back(consume_thread);
			consume_thread->start( 128*1024 ).detach();
		}
		//向Controller注册的组件
		_subscriber = new ClusterStateSubscriber( *this, _ctlEndpoint, _subInterval );
		MCE_INFO( "[CacheLoader::initialize] initialize complete" );
	}

	//Controller刷新服务的入口
	virtual void renew( const ServerStateMap& clusterState ){
		MCE_DEBUG( "[CacheLoader::renew] renew " << clusterState.size() << " ServerStateInfoPtr -----" );

		// 获取controller发来的服务信息
		ServerStateMap mapSS = clusterState;
		std::map<std::string,  ServerStateInfoPtr> mapNew;
		for ( ServerStateMap::const_iterator it = mapSS.begin(); it != mapSS.end(); ++it ){
			for( ServerStateSeq::const_iterator itSeq = it->second.begin(); itSeq != it->second.end(); ++itSeq ){
				T prxSvc = T::uncheckedCast( (*itSeq)->prx->ice_timeout( _timeout )->ice_twoway() );
				mapNew.insert( make_pair<std::string,  ServerStateInfoPtr>( prxSvc->ice_toString(), (*itSeq) ) );
				MCE_TRACE( "[CacheLoader::renew] new server:(mod " << (*itSeq)->mod << " )" << prxSvc->ice_toString() );
			}
		}

		std::vector<bool> _loadingMod = _produce_thread->getLoadingMod();
		std::vector<bool> loadingMod( _loadingMod.size(), false );		//记录有多少个需要预加载的mod，其size为cluster;
		bool loading = false;
		// 剔除现有列表中，不在Controller发来的列表中的服务
		std::map<std::string,PrxAgentPtr> mapInfo = _mapPrx.getMap();
		for ( typename std::map<std::string,PrxAgentPtr>::const_iterator it = mapInfo.begin();
				it != mapInfo.end(); ++it ){
			if ( mapNew.find( it->first ) == mapNew.end() ){
				_mapPrx.erase( it->first );
				MCE_WARN( "[CacheLoader::renew] the old server:(mod "
					<< it->second->getMod() << " )" << it->first
					<< " is not in the new list, remove it !!!" );
			} else if( !it->second->isValid() ){	//正在加载
				loading = true;
				loadingMod[it->second->getMod()] = true;
			}
		}
		// 检测需要加载的服务
		mapInfo = _mapPrx.getMap();
		MCE_TRACE( "[CacheLoader::renew] old/new: " << _mapPrx.size() << "/" << mapNew.size() );
		std::vector<PrxAgentPtr> toBeLoad;
		for ( std::map<std::string,ServerStateInfoPtr>::iterator it = mapNew.begin();
				it != mapNew.end(); ++it ){
			typename std::map<std::string,PrxAgentPtr>::const_iterator itPrx = mapInfo.find( it->first );
			if( itPrx != mapInfo.end() ){
				// 当前可用服务列表中有此服务
				if ( it->second->newset ){
					// 服务在注册间隔内瞬间重启，要求重新加载
					MCE_WARN( "[CacheLoader::renew] the old server:(mod "
						<< it->second->mod << " )" << it->first
						<< " has a \"true\" newset, reload it !!!" );
					toBeLoad.push_back( itPrx->second );
				/*setValid的反馈有延时，另外也不应该使用setValid触发服务的预加载，帮此段暂时去掉
					但是注意这样会导致替补的CacheLoader在顶替的时候，也无法自动实现对invalid的服务启动预加载
					需要手动调用执行
				} else if( it->second->status==0 && itPrx->second->isValid() ){
					// 预加载已经完成，但是服务已经被手动设为invalid，要求重新加载
					MCE_WARN( "[CacheLoader::renew] the old server:(mod "
						<< it->second->mod << " )" << it->first
						<< " was set invalid, reload it !!!" );
					toBeLoad.push_back( itPrx->second );
				*/
				} else{
					continue;
				}
			} else{
				// 当前可用服务列表中无此服务，要求进行加载
				if( it->second->cluster != (int)loadingMod.size() ){
					loadingMod.resize( it->second->cluster, false );	//修正Cluster;
				}
				T prxSvc = T::uncheckedCast( it->second->prx->ice_timeout( _timeout )->ice_twoway() );
				PrxAgentPtr pa = new PrxAgent<T>(
					prxSvc,
					prxSvc->ice_toString(),
					it->second->cluster,
					it->second->mod,
					_write_batch_size );
				_mapPrx.put( pa->getEndpoint(), pa );
				_mapPrx[pa->getEndpoint()]->start( 128*1024 );
				if( it->second->status == 0 ){
					MCE_WARN( "[CacheLoader::renew] add new server:(mod "
						<< pa->getMod() << " )" << pa->getEndpoint() << " preload it!" );
					toBeLoad.push_back( pa );
					loadingMod[it->second->mod] = true;
				} else {//else : 当Loader服务或者Controller服务重启了而Cache服务没重启，则不执行加载
					MCE_WARN( "[CacheLoader::renew] add new server:(mod "
						<< pa->getMod() << " )" << pa->getEndpoint() << " but status!=0, do nothing!" );
				}
			}
		}

		IceUtil::Mutex::Lock lock( *this );

		if( _loadingMod.size() != loadingMod.size() ){
			MCE_WARN( "[CacheLoader::renew] loadingMod resize cluster: from " << _loadingMod.size() << " to " << loadingMod.size() );
			_produce_thread->setLoadingMod( loadingMod );
		} else{
			for( size_t it = 0; it < loadingMod.size(); ++it ){
				if( _loadingMod[it] != loadingMod[it] ){
					MCE_WARN( "[CacheLoader::renew] loadingMod found mod " << it << " changed valid status" );
					_produce_thread->setLoadingMod( loadingMod );
				}
			}
		}

		if ( !_valid ){
			MCE_INFO( "[CacheLoader::renew] This CacheLoader is a backup one. Do nothing ..." );
			return;
		}
		if ( !toBeLoad.empty() ){
			_produce_thread->reset( toBeLoad );
		} else if( !loading ){
			// 之前和新加的服务都不再需要加载，则停止加载。（当正在加载的服务被停掉了，则会到达此处）
			_produce_thread->stop();
		}
		MCE_DEBUG( "[CacheLoader::renew] renew complete -----" );
	}

	MyUtil:: ObjectResultPtr reload( const MyUtil::IntSeq& ids ){
		ostringstream idstr;
		MyUtil::IntSeq::const_iterator it = ids.begin();
		if( it != ids.end() ){
			idstr << *it;
			++it;
			for(;it!=ids.end();++it){
				idstr << ", " << *it;
			}
		} else{
			idstr << "size=0, do nothing";
		}

		MCE_INFO( "[CacheLoader::reload] reload : " << idstr.str() );
		std::map<std::string,PrxAgentPtr> mapPrxInfo = _mapPrx.getMap();
		if (mapPrxInfo.size() <=0 ){
			return 0;
		}
		MyUtil::ObjectResultPtr objs = _cacheloader_factory->create(ids);
		int cluster = mapPrxInfo.begin()->second->getCluster();
		vector<MyUtil::ObjectResultPtr> groups;
		for (int i = 0; i < cluster; ++i) {
			groups.push_back(new MyUtil::ObjectResult);
		}

		for (map<long, Ice::ObjectPtr>::iterator itr = objs->data.begin(); itr != objs->data.end(); ++itr) {
			groups[itr->first % cluster]->data.insert(make_pair(itr->first, itr->second));
		}

		for ( typename std::map<std::string,PrxAgentPtr>::iterator it = mapPrxInfo.begin();
				it != mapPrxInfo.end(); ++it ){
			it->second->load( groups[it->second->getMod()] );
		}
		return objs;
	}
protected:
	class ProduceThread: virtual public IceUtil::Thread {
	private:
		PrxAgentMap& _mapPrx;
		ProducerPtr& _producer;
        	std::vector<ConsumeThreadPtr>& _consume_threads;

		int _produce_batch_size;
		int _consume_batch_size;
		int _thread_size;
		//以上值均是从CacheLoader中引用而来

		std::deque<int> _queIds;	//消费队列
		short _waitingConsumers;	//等待中的消费者数量
		std::vector<bool> _loadingMod;		//记录有多少个需要预加载的mod，其size为cluster;
		IceUtil::Monitor<IceUtil::Mutex> _queMutex;	//_queIds的互斥体

		int _targetId;		//生产线程最后产生的id值(这个值会在加入新服务时重设)
		bool _nextLoop;		//生产线程在完成当前此轮id生产后，是否要进行下一轮（取决于消费过程中新服务加入的那一刻，_targetId和_consumeTo的比较）
		bool _producing;	//生产线程是否在运作
		IceUtil::Monitor<IceUtil::Mutex> _pMutex;	//以上四个值的读写锁

		int _consumeTo;		//消费线程当前消化的这批id中的最大值(这个值会在消费进程中动态变化）
		IceUtil::RWRecMutex _cMutex;
	public:
		ProduceThread( PrxAgentMap& mapPrx,
				ProducerPtr& producer,
        			std::vector<ConsumeThreadPtr>& consume_threads,
				int produce_batch_size,
				int consume_batch_size,
				int thread_size ) :
			_mapPrx(mapPrx),
			_producer(producer),
			_consume_threads(consume_threads),
			_produce_batch_size(produce_batch_size),
			_consume_batch_size(consume_batch_size),
			_thread_size(thread_size),
			_waitingConsumers(0),
			_targetId(0),
			_nextLoop(false),
			_producing(false),
			_consumeTo(0)
		{}

		std::vector<bool> getLoadingMod(){
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
			return _loadingMod;
		}

		void setLoadingMod( std::vector<bool>& loadingMod ){
			ostringstream modstr;
			for( size_t it = 0; it < loadingMod.size(); ++it ){
				if( loadingMod[it] ){
					modstr << it << " ";
				}
			}
			MCE_INFO( "[Producer::setLoadingMod] loading mod: " << modstr.str() );
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
			_loadingMod.swap( loadingMod );
		}

		//CacheLoader收到新的服务，或者消费线程到达末id时，调用此函数唤醒生产线程
		void reset( std::vector<PrxAgentPtr>& toBeLoad ){
			int targetIdTmp;
			{
				IceUtil::RWRecMutex::RLock lock(_cMutex);
				targetIdTmp = _consumeTo;
			}
			for( typename std::vector<PrxAgentPtr>::iterator it = toBeLoad.begin(); it != toBeLoad.end(); ++it ){
				(*it)->setTarget( targetIdTmp );
			}//让相应的PrxAgent进入接受数据状态
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
				MCE_INFO( "[Producer::reset] reset produce thread. set _targetId(" << _targetId << ") to _cosumeTo(" << targetIdTmp << ")" );
				_targetId = targetIdTmp;
				_nextLoop = true;
			}
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
				if ( _queIds.empty() && _waitingConsumers == _thread_size ){
					invoke(false);
				}
			}
		}
		//停止预加载
		void stop(){
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
				_targetId = 0;
				_nextLoop = false;
				_producing = false;
			}
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
				_queIds.clear();
				if( !_waitingConsumers ){
					_queMutex.notify();
				}
			}
        		for( typename std::vector<ConsumeThreadPtr>::iterator it = _consume_threads.begin();
					it != _consume_threads.end(); ++it ){
				(*it)->setValid( false );
			}
			{
				IceUtil::RWRecMutex::WLock lock(_cMutex);
				_consumeTo = 0;
			}
			MCE_INFO( "[Producer::stop] stoped" );
		}
		void invoke( bool secondLoop ){
			MCE_INFO( "[Producer::invoke] invoke produce thread" );
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
				//这个锁的存在是为了跟生产线程互斥
				if ( _producing ){		//生产线程正在运行
					return;
				}
			}
			//不论是由renew还是consumer的触发，当到达此处时: _producing==false; _queIds.empty()==true; _waitingConsumers==_thread_size;
			//生产线程并未执行，而且外部调用都处于_queMutex锁的互斥之下，所以pMutex和cMutex都是不必要的
			if( !_nextLoop ){		//已经彻底到达结束
				MCE_INFO( "[Producer::invoke] _nextLoop==false" );
				_consumeTo = 0;			//下一个服务来则从头开始
			} else{				//还要进行下轮
				MCE_INFO( "[Producer::invoke] _nextLoop==true" );
				std::map<std::string,PrxAgentPtr> mapPrxInfo = _mapPrx.getMap();
				if( secondLoop ){
					for ( typename std::map<std::string,PrxAgentPtr>::iterator it = mapPrxInfo.begin();
							it != mapPrxInfo.end(); ++it ){
						it->second->setNextLoop( false );
					}
					_nextLoop = false;	//这是由Consumer触发的，应该将下轮标志清掉
				}
				_producing = true;
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
				_pMutex.notify();
			}
		}

		MyUtil::IntSeq consume( int& minId, int& maxId, int& left, bool cValid ){
			MyUtil::IntSeq ids;
			while ( true ){
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
				if ( _queIds.empty() ){
					try {
						if( ++_waitingConsumers == _thread_size ){
							invoke(cValid);
						}
						_queMutex.wait();
						--_waitingConsumers;
					} catch( ... ){
						--_waitingConsumers;
						throw;
					}
					continue;
				}
				size_t popSize = _queIds.size() < (size_t)_consume_batch_size ? _queIds.size() : (size_t)_consume_batch_size;
				minId = _queIds.front();
				int tmpId;
				for ( size_t i = 0; i < popSize; i++ ) {
					tmpId = _queIds.front();
					if( _loadingMod[tmpId%(int)_loadingMod.size()] ){
						ids.push_back( tmpId );
					}
					_queIds.pop_front();
				}
				maxId = tmpId;
				left = _queIds.size();

				if( !_waitingConsumers && (int)left<(_consume_batch_size*_thread_size) ){
					_queMutex.notify();
				}
				break;
			}
			//MCE_INFO( "[Producer::consume] Consumer(" << cId << ") at " << maxId );
			{
				IceUtil::RWRecMutex::WLock lock(_cMutex);
				if( cValid ){
					_consumeTo = maxId > _consumeTo ? maxId : _consumeTo;
				} else{
					_consumeTo = 0;
				}
			}
			return ids;
		}

		virtual void run(){
			MCE_INFO( "[Producer::run] start" );
			while ( true ){
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
					if ( !_producing ){	//这里之所以没有对_nextLoop作限定是为了让生产队列在完成本轮生产之后先不进入下一轮
						_pMutex.wait();
						continue;
					}
				}

				MCE_INFO( "[Producer::run] loop begin" );
				int beginId = 0;
				int maxId = 0;
				bool next = true;
				while( next ){
					beginId = maxId;
					std::vector<int> ids;
					try{
						ids = _producer->produce(beginId,_produce_batch_size,maxId);
					}catch( Ice::Exception& e ){
						MCE_WARN( "Producer Ice::Exception " << e.what() );
						maxId = beginId;
					}catch(mysqlpp::Exception& e){
						MCE_WARN( "Producer mysqlpp::Exception " << e.what() );
						maxId = beginId;
					}catch(std::exception& e){
						MCE_WARN( "Producer std::exception " << e.what() );
						maxId = beginId;
					}catch( ... ){
						MCE_WARN( "Producer exception" );
						maxId = beginId;
					}

					ostringstream logInfo;
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
						logInfo << "(producing/all).  beginId=" << beginId << " maxId=" << maxId << " target=" << _targetId << " ... ";
						if( _nextLoop && ( maxId==beginId ) ){
							//本轮到达结尾
							logInfo << "[ _nextLoop==true && maxId==beginId, pause! waiting for next round! ]";
							next = false;		//停止小循环
							_producing = false;	//停止加载，下轮大循环时将因此进入wait()
						} else if( !_nextLoop && ( maxId>=_targetId || maxId==beginId ) ){
							//本轮到达标记点，全部结束
							logInfo << "[ _nextLoop==false && ( maxId>=_targetId || maxId==beginId ), finish! stop at << " << maxId << " ]";
							_targetId = maxId;	//已经结束所有id加载，_targetId进行微调，指向确切的结束点
							next = false;		//停止小循环
							_producing = false;	//停止加载，下轮大循环时将因此进入wait()
						} else if( !_producing ){
							//本轮还未结束，但是外部已经通过stop()将_producing设为false,需要强行停止
							logInfo << "[ force stop! ]";
							next = false;		//停止小循环
						}
					}

					bool notified = false;
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _queMutex );
						MCE_INFO( "[Producer::run] " << ids.size() << "/" << _queIds.size() << logInfo.str() );
						for (size_t i = 0; i < ids.size(); ++ i) {
							_queIds.push_back(ids[i]);
						}
						if ( _waitingConsumers ){
							_queMutex.notify();
						} else if( _queIds.size() > (size_t)(_consume_batch_size*_thread_size*100) ){
							_queMutex.wait();
							notified = true;
						}
					}
					if( notified ){
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
						if ( !_producing ){	//说明这个notify来自stop(),需要马上停止加载;
							break;
						}
					}
				}
			}
		}
	};

	class ConsumeThread: public IceUtil::Mutex, virtual public IceUtil::Thread {
	private:
		int _cid;
		ProduceThreadPtr& _produce_thr;
		PrxAgentMap& _mapPrx;
		CacheLoaderFactoryPtr& _cacheloader_factory;
		int _consume_batch_size;
		//以上值均是从CacheLoader中引用而来
		bool _valid;
	public:
		ConsumeThread( int cid,
				ProduceThreadPtr& produce_thr,
				PrxAgentMap& mapPrx,
				CacheLoaderFactoryPtr& cacheloader_factory,
				int consume_batch_size ) :
			_cid(cid),
			_produce_thr(produce_thr),
			_mapPrx(mapPrx),
			_cacheloader_factory(cacheloader_factory),
			_consume_batch_size(consume_batch_size),
			_valid(true) {
		}
		void setValid( bool valid ){
			IceUtil::Mutex::Lock lock( *this );
			_valid = valid;
		}
		virtual void run(){
			MCE_INFO( "[Consumer("<<_cid<<")::run] start" );
			while ( true ){

				int minId;
				int maxId;
				int left;
				bool valid;
				{
					IceUtil::Mutex::Lock lock( *this );
					valid = _valid;
				}
				MyUtil::IntSeq ids = _produce_thr->consume( minId, maxId, left, valid );
				{
					IceUtil::Mutex::Lock lock( *this );
					_valid = true;
				}

				MyUtil::ObjectResultPtr results;
				switch( _cacheloader_factory->getCreateType() ){
				case CacheLoaderFactory::idlist :
					MCE_INFO( "[Consumer("<<_cid<<")::run] consuming idlist, size=" << ids.size()
							<< " from minId:" << minId
							<< " to maxId:" << maxId << " left:" << left );
					try{
						results = _cacheloader_factory->create( ids );
					}catch(Ice::Exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(ids) handle Ice::Exception " << e.what());
					}catch(mysqlpp::Exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(ids) handle mysqlpp::Exception " << e.what());
					}catch(std::exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(ids) handle std::exception " << e.what());
					}catch(...){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(ids) handle unknown exception.");
					}

					break;
				case CacheLoaderFactory::minmax :
					MCE_INFO( "[Consumer("<<_cid<<")::run] consuming from minId:" << minId
							<< " to maxId:" << maxId << " left:" << left );
					try{
						results = _cacheloader_factory->create( minId, maxId );
					}catch(Ice::Exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(" << minId << "," << maxId << ") handle Ice::Exception " << e.what() );
					}catch(mysqlpp::Exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(" << minId << "," << maxId << ") handle mysqlpp::Exception " << e.what() );
					}catch(std::exception& e){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(" << minId << "," << maxId << ") handle std::exception " << e.what() );
					}catch(...){
						MCE_WARN( "[Consumer("<<_cid<<")::run] create(" << minId << "," << maxId << ") handle unknown exception." );
					}

					break;
				default :
					MCE_WARN( "[Consumer("<<_cid<<")::run] this should not be seen!" );
					break;
				}

				MCE_DEBUG( "[Consumer("<<_cid<<")::run] " << results->data.size() << " objects created!" );
				std::map<std::string,PrxAgentPtr> mapPrxInfo = _mapPrx.getMap();
				for ( typename std::map<std::string,PrxAgentPtr>::iterator it = mapPrxInfo.begin();
						it != mapPrxInfo.end(); ++it ){
					it->second->add( results );
				}
			}
		}
	};
};


}
}

#endif
