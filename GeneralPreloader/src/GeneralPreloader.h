#ifndef __GENERALPRELOADER_H__
#define __GENERALPRELOADER_H__

#include <iostream>

#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>
#include "UtilCxx/src/AtomicSTL.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClusterStateSubscriber.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClusterClient.h"

#include "GeneralPreloaderProducer.h"
#include "GeneralPreloaderFactory.h"



namespace xce {
namespace generalpreloader {

using namespace xce::generalpreloader ;

using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;


//服务代理，负责往服务中写入数据，带有数据队列和写线程

template<class T> class PrxAgent: public Ice::Object {
	
	private:
	
	T _prx;			         //直接代理

	std::string _endpoint;	
	
	int _cluster;		
	
	int _mod;		
	
	int _write_batch_size;	//批量写入的个数，由GeneralPreLoader的纯虚函数定义

	int _target_id;		    //开始id,也就是下一轮的结束id
	
	bool _next_loop;    	//是否已经到了最后一轮循环
	
	bool _is_valid;		    //是否允许设置valid(设置后不再做setData操作)
	
	bool _isFirstAdd;		//是否第一次通知，用来判断是否需要记录targetid
	
	IceUtil::Monitor<IceUtil::Mutex> _mutex;				                //以上三个变量的读写锁
	
	xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& _queSetResults;

		
	vector<pair<long, Ice::ObjectPtr> > _validData ;  //保存选择出来的属于自己的数据,这样做是为了批量的添加到queData中，
		                                                 //减少对queData中锁的调用，提高性能
	public:

	PrxAgent(T& prx, const std::string& endpoint, const int cluster,  const int mod,
			  const int write_batch_size, const bool isValid,
			  xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& queSetResults) :
		     _prx(prx), _endpoint(endpoint), _cluster(cluster), _mod(mod),
			 _write_batch_size(write_batch_size), _target_id(0), _next_loop(false), 
			 _is_valid(isValid),_isFirstAdd(true),_queSetResults(queSetResults)
	{
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

	bool getValid() {
		return _is_valid;
	}

	//此函数会使以后的add在id>_target_id并且没有下一轮写入时，不再接受新加载数据(_is_valid=true);

	void setNextLoop( bool next_loop ){
		MCE_INFO("PrxAgent " << _mod << " ::setNextLoop" );
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
		_next_loop = next_loop;
	}

	//设置prx的状态，因为有可能手动更改状态来开始或或停止预加载
	
	bool setValidState(bool isValid)
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
		MCE_INFO("PrxAgent " << _mod << " ::setValidState valid = " << isValid << " curValid = " << _is_valid );
		if(isValid == _is_valid)
		{
			return false;
		}
		if(isValid)
		{
			_is_valid = true;
			_isFirstAdd = false;
			_next_loop = false;
			return false;
		}else
		{
			_is_valid = false;
			_isFirstAdd = true;
			_next_loop = true;
			return true;
		}
		return false;
	}

	//批量版预加载，如果需要下次继续添加则返回true,否则返回false

	bool add( const MyUtil::ObjectResultPtr& newDatas ){
    
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
	
		{
       		if( _is_valid ){
				return false;                            //valid! 什么都不做	
       		}
			
			if(_isFirstAdd)
			{
				setTarget(newDatas->data.begin()->first);
				_isFirstAdd = false;
			}
		}
	
		
		for ( std::map<long, Ice::ObjectPtr>::const_iterator it = newDatas->data.begin();
			it!= newDatas->data.end(); ++it )
		{
			getValidData( it->first, it->second );    //判断数据是否属于该prx，如果属于则添加到validData中
			
			if(isMakeValid(it->first))                   //首先判断id是否能使该prx变成valid
			{
				if(_validData.size()>0)                   //还有最后一批数据需要加入推送队列
				{
				MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult();
				results->data.insert(_validData.begin(),_validData.end());
				_validData.clear();
				_queSetResults.push(make_pair<T,MyUtil::ObjectResultPtr>( _prx, results) );
				}
				setValid();                      //没有数据需要加入，而且队列已空，则setValid
				return false;
			}
		}
		if(_validData.size()>=(size_t)_write_batch_size)
		{
				MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult();
				results->data.insert(_validData.begin(),_validData.end());
				_validData.clear();
				_queSetResults.push(make_pair<T,MyUtil::ObjectResultPtr>( _prx, results) );
		}
		return true;
    }

	private:

	//设置开始推送的参数
	void setTarget( int target_id ){
		MCE_INFO("PrxAgent " << _mod << " ::setTarget target_id = " << target_id);
		
		_target_id = target_id;
		_next_loop = true;
		_is_valid = false;
	}

	//将服务设为可用
	void setValid(){
	
		MCE_INFO( "PrxAgent "<<_mod<<" ::setValid " );
		
		try {
			_prx->setValid(true);
		}
		catch (Ice::Exception& e){
			MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
		}
		catch (std::exception& e ){
			MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
		}
		catch (...){
			MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
		}
	}

	//判断id是否会使当前的prx变成valid
	//inValid! 不再有下一轮的数据，且当前加入的id超过了目标id，
	//且当前加入的这个ID不属于上一轮(因为consumer是异步的)，则不再接受数据，且允许setValid
	bool isMakeValid(long id)
	{ 
			
		if( ( !_next_loop )&& (id>=_target_id )){
			_is_valid = true;
			MCE_INFO( "PrxAgent "<<_mod<<" ::add final data for " << id << " to set valid" );
		}
		return _is_valid;
	}
	
	//供预加载使用，判断当前id是否属于该prx，属于则添加到vector中
	void getValidData(long id, Ice::ObjectPtr newData)
	{
		if( ( id%_cluster==_mod ) && ( newData!=0 ) ){
			_validData.push_back( make_pair<long, Ice::ObjectPtr>( id, newData ) );  //该对象不为空且属于自己，则加入队列
		}
	}
};

//各种Loader服务的基类，集成了预加载功能
template<class T> class GeneralPreloader : virtual public MyUtil::Singleton<GeneralPreloader<T> >
{

    protected:

	typedef IceUtil::Handle< PrxAgent<T> > PrxAgentPtr; 
	
	typedef xce::atomicstl::AtomicMap<std::string,PrxAgentPtr> PrxAgentMap;

	class ProduceThread;

	typedef IceUtil::Handle< ProduceThread > ProduceThreadPtr; 
	
	class SetDataThread;
	
	typedef IceUtil::Handle< SetDataThread > SetDataThreadPtr; 
	
	class ConsumeThread;
	
	typedef IceUtil::Handle< ConsumeThread > ConsumeThreadPtr; 
	
	class PrxAgentManager;
	
	typedef IceUtil::Handle< PrxAgentManager > PrxAgentManagerPtr; 
	
	protected: 

	PrxAgentMap _prxAgentMap;
	
	PrxAgentManagerPtr _prxAgentManager;
	
	GeneralPreloaderFactoryPtr _generalpreloaderFactory;

	ProduceThreadPtr _produceThreadPtr;

	vector<ConsumeThreadPtr> _consumeThreads;
	
	xce::atomicstl::BlockingQueue<long> _queIds;
	
	xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> > _queSetResults;

	std::map<int,bool> _confirmSetNextLoop;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	bool _isOneLoopFinish;

	protected:

	//生产线程
    class ProduceThread: virtual public IceUtil::Thread {
      
    	private:
    		
    		ProducerPtr _producer;
    
    		ConsumeThreadPtr _consumeThreadPtr;
    		
    		xce::atomicstl::BlockingQueue<long>& _queIds;
    	
    		int _produce_batch_size;
    	
		int _tableCluster;
	
    		bool _producing;	                        //生产线程是否在运作
    		
    		IceUtil::Monitor<IceUtil::Mutex> _mutex;	//_producing的锁

		std::vector<bool> _loadingMod;          //记录有多少个需要预加载的mod，其size为cluster;
    	
    	public:
    
    		ProduceThread(const ProducerPtr& producer, 
    				      xce::atomicstl::BlockingQueue<long>& queIds,
    				      const int produce_batch_size):
				          _producer(producer),
						  _queIds(queIds),
						  _produce_batch_size(produce_batch_size),
						_tableCluster(1)
		    {
	     	}
    		
		ProduceThread(const ProducerPtr& producer,
                                      xce::atomicstl::BlockingQueue<long>& queIds,
                                      const int produce_batch_size,
					const int tableCluster):
                                          _producer(producer),
                                                  _queIds(queIds),
                                                  _produce_batch_size(produce_batch_size),
                                                _tableCluster(tableCluster)
                    {
                }


			void init(ConsumeThreadPtr& consumeThreadPtr) 
    		{
    			_consumeThreadPtr = consumeThreadPtr;
    			_producing = false;
    		}
    
    		void invoke(bool isStart)
    		{
    			MCE_INFO("ProduceThread::invoke ");
    			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
    			
    			if(isStart)
    			{
    				if(!_producing)         //唤醒线程
    				{
    					_producing = true;
    					_mutex.notify();  
    					MCE_INFO("ProduceThread::invoke wake up producethread");
    				}
    			}else                       //停止线程
    			{
    				_producing = false;  
    				MCE_INFO("ProduceThread::invoke stop producethread");
    			}
    		}

		std::vector<bool> getLoadingMod(){
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
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
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
			_loadingMod.swap( loadingMod );
		}
    
    		virtual void run(){
    			MCE_INFO( "ProduceThread::run start" );
    			while ( true ){
    				MCE_INFO( "ProduceThread::run loop begin" );
    				long beginId = 0;
    				long maxId = 0;
    				bool next = true;
    				int tableMod = 0;
	
    				{ 
    					IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
    					if(!_producing)
    					{
    						MCE_INFO("ProduceThread::run producethread is waiting for new loop");
    						_mutex.wait();     //跑完一轮之后等待被唤醒
    						MCE_INFO("ProduceThread::run producethread is waked up");
    					}
    				}
    			
    				while( next)                   //当没有完成一轮，而且consumethread没有通知停止的时候，继续进行
    				{
    					{ 
    						IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
    						if(!_producing)
    						{
								MCE_INFO("ProduceThread::run producethread is waiting for continue the loop");
    							_mutex.wait();     //如果没有在生产，则等待pcthreadsmanager的唤醒
    							MCE_INFO("ProduceThread::run producethread is waked up");
    						}
    					}
    				
    					beginId = maxId;
    					std::vector<long> total_ids;
    					std::vector<long> ids;

						createIds(total_ids, maxId, beginId,tableMod);  //从db读取id序列
						
						ostringstream idInfo;
						for (std::vector<long>::iterator ui = total_ids.begin();
								ui !=total_ids.end(); ++ui) {
							if( _loadingMod[*ui%(int)_loadingMod.size()] ){
								idInfo << *ui << " ";
								ids.push_back( *ui );
							}
						}
						//MCE_DEBUG("producing, ids: " << idInfo.str());
    					
						ostringstream logInfo;
    					logInfo << "(producing/all).  beginId=" << beginId << " maxId=" << maxId <<  " ... ";
    					if( maxId==beginId  )            //本轮到达结尾
    					{
                 beginId = 0;
                 maxId = 0;
						if( ++tableMod >= _tableCluster){
    							next = false;                //标志本轮结束
							MCE_INFO( "ProduceThread::run one loop finish" );
						}
    					} 
    					MCE_INFO( "ProduceThread::run " << ids.size() << "/" << _queIds.getSize() );
    				
    					_queIds.pushSeq(ids);            //将生产出来的id加入queids
    				}
    			
    				_consumeThreadPtr->setOneLoopFinish();     //跑完一轮之后,重设所有prxagent的nextloop参数
    			}
    		}

		private:

			void createIds(vector<long>& ids, long& maxId, const long& beginId, int& tableMod)
			{
				try {
					ids = _producer->produceLongIds(beginId,_produce_batch_size,maxId, tableMod);
				} catch( Ice::Exception& e ){
					MCE_WARN( "ProduceThread::run Ice::Exception " << e.what() );
					maxId = beginId +1;
				} catch ( std::exception& e ){
					MCE_WARN( "ProduceThread::run std::exception " << e.what() );
					maxId = beginId +1;
				} catch( ... ){
					MCE_WARN( "ProduceThread::run exception" );
					maxId = beginId +1;
				}
			}
    };
    	
   //消费线程 	
   class SetDataThread: virtual public IceUtil::Thread {
    	
    	private:
    		
    		xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& _queSetResults;
    	
			int _cid;
    		
			int _set_batch_size;
    
    	public:
    		SetDataThread( const int cid,
					   xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& queSetResults,
					   const int set_batch_size): 
					   _queSetResults( queSetResults),
					   _cid ( cid),
					   _set_batch_size( set_batch_size)
					{
					}
    
    		
    		virtual void run(){
    			MCE_INFO( "SetDataThread( "<<_cid<<" )::run start" );
    			while ( true ){
    			
					vector<pair<T,MyUtil::ObjectResultPtr> > vecResults; 
    				_queSetResults.popfrontSeq( vecResults, _set_batch_size);
					MCE_INFO( "SetDataThread( "<<_cid<<" )::run queue size = " << _queSetResults.getSize() );
					for(typename vector<pair<T,MyUtil::ObjectResultPtr> >::iterator it = vecResults.begin();
						it!=vecResults.end(); ++it)
					{
                  		try {
								MCE_INFO( "SetDataThread( "<<_cid<<" )::run try to set size = " << it->second->data.size() );
                  				it->first->setData(it->second);
								MCE_INFO( "SetDataThread( "<<_cid<<" )::run finish set size = " << it->second->data.size() );
                  		}
                  		catch (Ice::Exception& e){
                  			MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
                  		}
                  		catch (std::exception& e ){
                  			MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
                  		}
                  		catch (...){
                  			MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
                  		}
					}
				}
			}
    
    };
    	
   //消费线程 	
   class ConsumeThread: virtual public IceUtil::Thread {
    	
    	private:
    
    		PrxAgentMap& _prxAgentMap;
    
    		ProduceThreadPtr _produceThreadPtr;
    		
    		GeneralPreloaderFactoryPtr _generalpreloader_factory;
    		
    		xce::atomicstl::BlockingQueue<long>& _queIds;
    	
			int _threadSize;
			
			int _cid;
    		
			int _consume_batch_size;
    
			bool& _isOneLoopFinish;

			std::map<int,bool>& _confirmSetNextLoop;
    		
    		IceUtil::Monitor<IceUtil::Mutex>& _mutex;				//isoneloopfinish的读写锁
    	
    	public:
    		ConsumeThread( const int cid,
    				   const GeneralPreloaderFactoryPtr& generalpreloader_factory,
					   const ProduceThreadPtr& produceThreadPtr, 
					   PrxAgentMap& prxAgentMap,
					   xce::atomicstl::BlockingQueue<long>& queIds,
    				   const int consume_batch_size, 
					   const int threadSize,
					   std::map<int,bool>& confirmSetNextLoop,
					   IceUtil::Monitor<IceUtil::Mutex>& mutex,
					   bool& isOneLoopFinish): 
				       _prxAgentMap ( prxAgentMap),
					   _produceThreadPtr ( produceThreadPtr),
					   _generalpreloader_factory ( generalpreloader_factory),
					   _queIds ( queIds),
					   _threadSize( threadSize),
					   _cid ( cid),
					   _consume_batch_size ( consume_batch_size),
					   _isOneLoopFinish ( isOneLoopFinish),
					   _confirmSetNextLoop(confirmSetNextLoop),
					   _mutex(mutex)
					{
					}
    
    		//pcthreadsmanager会调用此方法通知一轮produce已经结束
    		void setOneLoopFinish()
    		{
    			MCE_INFO( "ConsumeThread( "<<_cid<<" )::setoneloopfinish " );
    			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);				
    			_isOneLoopFinish = true;
    		}
    		
    		virtual void run(){
    			MCE_INFO( "ConsumeThread( "<<_cid<<" )::run start" );
    			while ( true ){
    				
    				long minId;
    				long maxId;
    				MyUtil::LongSeq ids;
    				long left;
    				
    				_queIds.popfrontSeq( ids, _consume_batch_size);
    					
    				minId = ids.front();
    				maxId = ids[ids.size()-1];
    				left = _queIds.getSize();
					
					MyUtil::ObjectResultPtr results;
					
					createResults(results, ids, minId, maxId, left);  //从db读取数据
    				
    				MCE_INFO( "ConsumeThread( "<<_cid<<" )::run " << results->data.size() << " objects created!" );
    			
					if(results->data.size()==0)
					{
						continue;         //如果没有数据需要add则继续下一轮
					}

					if(!add(results))     //将数据发送给prxagentmanager,
    					                                                 //并判断是否需要继续加载，不需要则停止	
    				{ 
    					_produceThreadPtr->invoke(false);
    				}
    
					trySetPrxNextLoop();                          //判断是否该通知prx,并进行相应操作
    			}
    		}
    
    	private: 
    
			//从db中读取数据并形成results
			void createResults(MyUtil::ObjectResultPtr& results, const MyUtil::LongSeq& ids, long minId, long maxId, long left)
			{ 
				switch( _generalpreloader_factory->getCreateType() ){
					case GeneralPreloaderFactory::idlist :
						MCE_INFO( "ConsumeThread( "<<_cid<<" )::run consuming idlist, size=" << ids.size()
								<< " from minId:" << minId
								<< " to maxId:" << maxId << " left: " << left << " ids." );
						  results = _generalpreloader_factory->createWithLongIds( ids );
						break;
    				case GeneralPreloaderFactory::minmax :
    					MCE_INFO( "ConsumeThread( "<<_cid<<" )::run consuming from minId:" << minId
    							  << " to maxId:" << maxId << " left: " << left << " ids." );
    					  results = _generalpreloader_factory->createWithLongIds( minId, maxId );
    					break;
    				default :
    					MCE_WARN( "ConsumeThread( "<<_cid<<" )::run this should not be seen!" );
    					break;
    				}
			}
    		
			//consumethread将数据添加到prxagentmanager中，并由prxagent分发出去
    		bool add(MyUtil::ObjectResultPtr& results)
    		{
    			bool isNeedContinueThreads = false;                 //判断是否还有服务需要继续加载
    
    			std::map<std::string,PrxAgentPtr> mapPrxInfo = _prxAgentMap.getMap();
    			
    			for ( typename std::map<std::string,PrxAgentPtr>::iterator it = mapPrxInfo.begin();
    					it != mapPrxInfo.end(); ++it ){
    				if(it->second->add( results ))
    				{
    					isNeedContinueThreads = true;
    				}
    			} 
    			
    			return isNeedContinueThreads;
    		}
   
			//判断是否需要进行setnextloop的操作

			bool isNeedSetNextLoop()
			{ 
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
					
				if(_isOneLoopFinish)
				{
    				MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) oneLoopFinish = true" );
					std::map<int,bool>::iterator it = _confirmSetNextLoop.find(_cid);
					if(it==_confirmSetNextLoop.end())
					{
						MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) set map flag false" );
						_confirmSetNextLoop.insert(make_pair<int,bool>(_cid,false));
					}else
					{
						if(it->second == false)
						{
							MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) set map flag true" );
							it->second = true;
							
							if(_confirmSetNextLoop.size()==(size_t)_threadSize)
							{
								bool waitFlag = false;
								for(it = _confirmSetNextLoop.begin();it!=_confirmSetNextLoop.end();++it)
								{
									if(it->second == false)
									{
										waitFlag = true;
									}
								}
								if(waitFlag == true)
								{
									MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) wait because some threads not finish two times" );
									_mutex.wait();
								}else
								{
									MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) i am setting nextloop and true map size = "<<_confirmSetNextLoop.size());
									_confirmSetNextLoop.clear();
									_isOneLoopFinish = false;   //重设是为了避免重复通知
									_mutex.notifyAll();
									return true;
								}
							}else
							{
								MCE_DEBUG( "ConsumeThread( "<<_cid<<" ) wait because some threads not finish one time" );
								_mutex.wait();
							}
						}
					}
				}
				return false;
			}
    		
			//consumethreads跑完一轮之后，重设所有prxagent的nextloop为false
    		void trySetPrxNextLoop()
    		{
    			MCE_INFO( "ConsumeThread " << _cid <<" ::trysetprxnextloop try" );
    	
				if(isNeedSetNextLoop())
				{ 
					MCE_INFO( "ConsumeThread " << _cid <<" ::trysetprxnextloop set" );
					std::map<std::string,PrxAgentPtr> mapPrxInfo = _prxAgentMap.getMap();
					for ( typename std::map<std::string,PrxAgentPtr>::iterator it = mapPrxInfo.begin();
							it != mapPrxInfo.end(); ++it ){

						it->second->setNextLoop(false); 
					}
				}else
				{ 
					MCE_INFO( "ConsumeThread " << _cid <<" ::trysetprxnextloop not this time" );
				}
    	
    		}
    };
    
    //管理PrxAgent的manager
    class PrxAgentManager : virtual public xce::clusterstate::ClusterClient
	{
    	private: 
    		
    		PrxAgentMap& _prxAgentMap;					//服务代理列表
    	
			ProduceThreadPtr _produceThreadPtr;
		
            std::string _ctrEndpoint;		            //Controller的endpoints
           
    		int _subInterval;			                //向Controller注册的间隔
    	
    		int _timeout;				                //服务直接代理的调用timeout
    		
    		int _write_batch_size;				        //prx中的每次推送的数据量
    		
			string _identifier;				            //同个进程中存在多个服务时的单个服务的标识符

			bool _isMutilService;                       //是否是在同一个进程中存在多个服务的类型
    		
    		xce::clusterstate::ClusterStateSubscriberPtr _subscriber;	//注册组件
			
			xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& _queSetResults;

    		IceUtil::Monitor<IceUtil::Mutex> _mutex;	
    	
		public:
   
    	//初始化
    	PrxAgentManager(const std::string& ctrEndpoint, const int subInterval,
				const int timeout, const int writeBatchSize,
				PrxAgentMap& prxAgentMap, 
				ProduceThreadPtr& produceThreadPtr,
				xce::atomicstl::BlockingQueue<pair<T,MyUtil::ObjectResultPtr> >& queSetResults,
				string identifier = "",bool isMutilService = false):
			    _prxAgentMap ( prxAgentMap),
				_produceThreadPtr(produceThreadPtr),
				_ctrEndpoint ( ctrEndpoint),
				_subInterval ( subInterval),
				_timeout ( timeout),
				_write_batch_size ( writeBatchSize),
				_identifier(identifier),
				_isMutilService(isMutilService),
				_queSetResults(queSetResults)
        {
    		_subscriber = new ClusterStateSubscriber( *this, _ctrEndpoint, _subInterval );   //向Controller注册的组件
		}	
    	
		//Controller刷新服务的入口
    	virtual void renew( const ServerStateMap& clusterState ){
    		MCE_INFO( "PrxAgentManager::renew clustersize = " << clusterState.size() << " ServerStateInfoPtr " );
    		
    		std::map<std::string,  ServerStateInfoPtr> mapNew;
    		
    		getServerStateFromController(clusterState, mapNew);        // 获取controller发来的服务信息
    		
		std::vector<bool> _loadingMod = _produceThreadPtr->getLoadingMod();
		std::vector<bool> loadingMod( _loadingMod.size(), false );		//记录有多少个需要预加载的mod，其size为cluster;

    		deleteInvalidPrx(mapNew, loadingMod);                                  // 剔除现有列表中，不在Controller发来的列表中的服务
    
            std::vector<PrxAgentPtr> toBeLoad;                         //记录当前需要推送的服务
            
    		renewMapPrx(toBeLoad,mapNew,loadingMod);                              //将当前的prxmap进行更新，并把需要开始推送的服务获取出来

		{
    			IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );

			if( _loadingMod.size() != loadingMod.size() ){
				MCE_WARN( "[PrxAgentManager::renew] loadingMod resize cluster: from " << _loadingMod.size() << " to " << loadingMod.size() );
				_produceThreadPtr->setLoadingMod( loadingMod );
			} else{
				for( size_t it = 0; it < loadingMod.size(); ++it ){
					if( _loadingMod[it] != loadingMod[it] ){
						MCE_WARN( "[CacheLoader::renew] loadingMod found mod " << it << " changed valid status" );
						_produceThreadPtr->setLoadingMod( loadingMod );
					}
				}
			}
		}

			if ( !toBeLoad.empty() ){
    			                                                       // 如果有服务需要推送则通知producethread
    			_produceThreadPtr->invoke(true);                       //通知pcthreadsmanager启动推送过程,
    		}
    		MCE_INFO( "PrxAgentManager::renew renew complete " );
    	}
    	
    	
    	private:
    
    		// 获取controller发来的服务信息
    		void getServerStateFromController(const ServerStateMap& clusterState, std::map<std::string,  ServerStateInfoPtr>& mapNew )
    		{ 
    			ServerStateMap mapSS = clusterState;
    			for ( ServerStateMap::const_iterator it = mapSS.begin(); it != mapSS.end(); ++it ){
    				for( ServerStateSeq::const_iterator itSeq = it->second.begin(); itSeq != it->second.end(); ++itSeq ){
    					T prxSvc = createDirectPrx( (*itSeq));
						if(!prxSvc)
						{
							continue;
						}
						
						try
						{
							prxSvc->setData(new MyUtil::ObjectResult);
						}catch(Ice::OperationNotExistException& e)
						{
							MCE_WARN("PrxAgentManager::getserverstatefromcontroller agent not have setdata operation (mod " << (*itSeq)->mod << " )" << prxSvc->ice_toString() );
							continue;
						}
    				
						mapNew.insert( make_pair<std::string,  ServerStateInfoPtr>( prxSvc->ice_toString(), (*itSeq) ) );
    					MCE_INFO( "PrxAgentManager::getserverstatefromcontroller new server:(mod " << (*itSeq)->mod << " )" << prxSvc->ice_toString() );
    				}
    			}
    		}
    
    		// 剔除现有列表中，不在Controller发来的列表中的服务
    		void deleteInvalidPrx(const std::map<std::string,  ServerStateInfoPtr>& mapNew, std::vector<bool>& loadingMod)
    		{ 
    			std::map<std::string,PrxAgentPtr> mapInfo = _prxAgentMap.getMap();
    			for ( typename std::map<std::string,PrxAgentPtr>::const_iterator it = mapInfo.begin();
    					it != mapInfo.end(); ++it ){
    				if ( mapNew.find( it->first ) == mapNew.end() ){ 
    					_prxAgentMap.erase( it->first );
    					MCE_INFO( "PrxAgentManager::deleteinvalidprx  the old server:(mod "
    							  << it->second->getMod() << " )" << it->first
    							  << " is not in the new list, remove it !!!" );
    				} else if ( !it->second->getValid() ) {
					loadingMod[it->second->getMod()] = true;
					MCE_INFO("mod: " << it->second->getMod() << " is true");
				}
    			}
    		}
    
            //将当前的prxmap进行更新，并把需要推送的服务获取出来
    		void renewMapPrx(std::vector<PrxAgentPtr>& toBeLoad, std::map<std::string,  ServerStateInfoPtr>& mapNew, std::vector<bool>& loadingMod )
    		{ 
    			MCE_INFO( "PrxAgentManager::renewmapprx old/new: " << _prxAgentMap.size() << "/" << mapNew.size() );
    			for ( std::map<std::string,ServerStateInfoPtr>::iterator it = mapNew.begin(); it != mapNew.end(); ++it ){
    				if( _prxAgentMap.has( it->first ) )                                    // 当前可用服务列表中有此服务
    				{
    					if ( it->second->newset ){
    						MCE_INFO( "PrxAgentManager::renewmapprx the old server:(mod "
    								  << it->second->mod << " )" << it->first
    								  << " has a \"true\" newset, reload it !!!" );
    						toBeLoad.push_back( _prxAgentMap[it->first] );                 // 重设老的服务(很有可能是服务在注册间隔内瞬间重启)
    					}
						if(_prxAgentMap[it->first]->setValidState(isServiceValid(it->second)))
						{
							toBeLoad.push_back(_prxAgentMap[it->first]);
						}
    				} else                                                            // 当前可用服务列表中无此服务
    				{
    					bool isValid = isServiceValid(it->second);
					if( it->second->cluster != (int)loadingMod.size() ){
						loadingMod.resize( it->second->cluster, false );	//修正Cluster;
					}

						T prxSvc = createDirectPrx(it->second);
						if(!prxSvc)
						{
							continue;
						}
    					PrxAgentPtr pa = new PrxAgent<T>(prxSvc, prxSvc->ice_toString(),
    							                         it->second->cluster,
    													 it->second->mod,
    													 _write_batch_size,
														 isValid,_queSetResults );
    					
    					_prxAgentMap.put( pa->getEndpoint(), pa );
    					MCE_INFO( "PrxAgentManager::renewmapprx  add new server:(mod "
    							  << pa->getMod() << " )" << pa->getEndpoint() << " !!!" );
    					if( !isValid ){
    						toBeLoad.push_back( pa );
						loadingMod[it->second->mod] = true;
						MCE_INFO("mod: " << it->second->mod << " is true");
    					}
    				}
    			}
    		}

			//判断该服务是否为valid

			bool isServiceValid(const ServerStateInfoPtr& ssip)
			{
				if(_isMutilService)
				{
					try
					{
						return (ServerStateInfoMutilPtr::dynamicCast(ssip))->statusMap.find(_identifier)->second == 1 ? true : false;
					}catch(...)
					{
						MCE_WARN("PrxAgentmanager::isservicevalid ServerStateInfoMutil Exception: can not cast the prx of identifier = " << _identifier);
					}
				}else
				{
					try
					{
						return ssip->status ==1 ? true : false;
					}catch(...)
					{
						MCE_WARN("PrxAgentmanager::isservicevalid ServerStateInfo Exception: can not cast the prx"); 
					}
				}
				return false;
			}

			//创建prx的直接代理

			T createDirectPrx(const ServerStateInfoPtr& ssip)
			{
				if(_isMutilService)
				{
					try
					{
						ServerStateInfoMutilPtr ssimp = ServerStateInfoMutilPtr::dynamicCast(ssip);

						ServerPrxMap::iterator findIt = ssimp->prxMap.find(_identifier);

						if(findIt != ssimp->prxMap.end())
						{ 
							return T::uncheckedCast(findIt->second->ice_timeout( _timeout )->ice_twoway()); 
						}
					}catch(...)
					{
						MCE_WARN("PrxAgentmanager::createdirectprx ServerStateInfoMutil Exception: can not cast the prx of identifier = " << _identifier);
					}
				}else
				{
					try
					{
						return T::uncheckedCast( ssip->prx->ice_timeout( _timeout )->ice_twoway() );
					}catch(...)
					{
						MCE_WARN("PrxAgentmanager::createdirectprx ServerStateInfo Exception: can not cast the prx"); 
					}
				}
				return NULL;
			}
    };
    
	public:

	//初始化方法
	void initialize(const ProducerPtr& producer,
			const GeneralPreloaderFactoryPtr& generalpreloader_factory,
			const std::string& ctrEndpoint,
			const int cluster,
			const int subInterval,
			const int timeout,
			const int setDataQueueMaxSize ,
			const int produceBatchSize,
			const int consumeBatchSize,
			const int writeBatchSize,
			const int consumethreadSize,
			string identifier = "",
			bool isMutilService = false,
			const int produceTableCluster = 1){
		
		MCE_INFO( "GenrealLoader::initialize initialize start" );
	
		int setDataQueue_MaxSize = setDataQueueMaxSize > 0 ? setDataQueueMaxSize : 10000;
		int produce_batch_size = produceBatchSize > 0 ? produceBatchSize : 1;
		int consume_batch_size = consumeBatchSize > 0 ? consumeBatchSize : 1;
		int write_batch_size = writeBatchSize > 0 ? writeBatchSize : 1;
		int consumethread_size = consumethreadSize > 0 ? consumethreadSize : 1;
		int set_thread_size = cluster > 10 ? consumethread_size *2 : consumethread_size ; 
		int set_batch_size = cluster/set_thread_size > 0 ? cluster/set_thread_size : 10; 
	
		_generalpreloaderFactory = generalpreloader_factory;
		_queIds.setProps( consume_batch_size * consumethread_size);
		_queSetResults.setProps( setDataQueue_MaxSize);
		
		MCE_INFO( "[GeneralPreloader::initialize] initialize: ctrEndpoint=" << ctrEndpoint
			<< "  cluster=" << cluster
			<< "  subInterval=" << subInterval 
			<< "  timeout =" << timeout 
			<< "  setDataQueue_MaxSize =" << setDataQueue_MaxSize 
			<< "  produce_batch_size=" << produce_batch_size
			<< "  consume_batch_size=" << consume_batch_size
			<< "  write_batch_size=" << write_batch_size
			<< "  consumethread_Size=" << consumethread_size 
			<< "  setthread_Size=" << set_thread_size 
			<< "  set_size=" << set_batch_size 
			<< "  identifier =" << identifier);


		ProduceThreadPtr produceThreadPtr = NULL;
		if(1 >= produceTableCluster){
			produceThreadPtr = new ProduceThread(producer,_queIds,produce_batch_size);
		}else{
			produceThreadPtr = new ProduceThread(producer,_queIds,produce_batch_size,produceTableCluster);
		}

		//初始化消费线程

		_isOneLoopFinish = false;

		for(int i=0;i<consumethread_size ;++i)
		{
			ConsumeThreadPtr consumeThreadPtr = new ConsumeThread(i,_generalpreloaderFactory,
					               produceThreadPtr,_prxAgentMap, _queIds,
    				               consume_batch_size, consumethread_size,_confirmSetNextLoop,_mutex, _isOneLoopFinish);
			consumeThreadPtr->start( 128*1024 ).detach();
			_consumeThreads.push_back(consumeThreadPtr);
		}
		//初始化消费线程

		for(int i=0;i<set_thread_size ;++i)
		{
			SetDataThreadPtr setDataThreadPtr = new SetDataThread(i, _queSetResults, set_batch_size);
			setDataThreadPtr->start( 128*1024 ).detach();
		}
		//初始化生产线程
		
    	produceThreadPtr->init( _consumeThreads[0] ); 
    
		produceThreadPtr->start( 128*1024 ).detach();
		//初始化prxagentmanager
	
		_prxAgentManager = new 	PrxAgentManager(ctrEndpoint, subInterval,
				                         timeout, writeBatchSize,
										 _prxAgentMap, 
										 produceThreadPtr,_queSetResults,
										 identifier,isMutilService);

		MCE_INFO( "GeneralPreloader::initialize initialize complete" );
	}
	
};

}
}

#endif
