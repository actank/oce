#ifndef _ABSTRACTCHECKERADAPTER_H_
#define _ABSTRACTCHECKERADAPTER_H_

#include <Checker.h>
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"
#include "Checker/share/OperaData.h"

namespace xce {
namespace check {

const unsigned int MAXQUEUE = 100000;
const unsigned int BLOCK = 100;

template<class C> class AddDataHelper;
template<class C> class NotifyDataHelper;
template<class C> class AbstractCheckerAdapter : virtual public MyUtil::AdapterI{
public:
class DataHelper {
public:
        virtual ~DataHelper(){}
        void initialize(AbstractCheckerAdapter<C>& adapter,std::string id,bool delay,std::string path){
                _id = id;
                _delay = delay;
                _dataThread = new DataThread(*this, adapter);
                _dataThread->start(1024*1024).detach();
                _pathstr = path;
        }
        void add(Entry& entry){
                //MCE_INFO(_id << "DataHelper::add " << entry.key);
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _pMutex );
                if(_queue.size() < MAXQUEUE ){
                        _queue.push_back(entry);
                } else{
                        MCE_WARN("DataHelper::add Queue is full, and entry is " << entry.key << " " << entry.checksum);
                }
        }
	void swap(std::vector<Entry>& todo){
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_pMutex);
                if(_delay){
                        //if (_queue.size() < 100){
                                //MCE_INFO( _id << " Data _pMutex.timedWait(IceUtil::Time::seconds(1) " ); 
                        //MCE_INFO(_id << " "<<_pathstr<<" DataHelper::timedwait begin");
                        _pMutex.timedWait(IceUtil::Time::seconds(2));
                        //MCE_INFO(_id << " "<<_pathstr<<" DataHelper::timedwait end");
                        //}     
                        //MCE_INFO(_id << " "<<_pathstr<<" DataHelper::timedwait queue.size=" << _queue.size() << "queueDelay.size="<<_queueDelay.size() );
                        _queueDelay.swap(todo);
                        _queueDelay.clear();    
                        _queue.swap(_queueDelay);
                }else{                  
                        if (_queue.size() < 10){
                                //MCE_INFO( _id << " Data _pMutex.timedWait(IceUtil::Time::seconds(1) " ); 
                                _pMutex.timedWait(IceUtil::Time::seconds(1));
                        }       
                        _queue.swap(todo);
                }
        }
                
        virtual void handle(AbstractCheckerAdapter<C>& adapter,std::vector<Entry>& batch) = 0;
        
        std::string getId(){
                return _id;
        }
private:
        std::vector<Entry> _queue;
        std::vector<Entry> _queueDelay;
        IceUtil::Monitor<IceUtil::Mutex> _pMutex;
        std::string _id;
                
        bool _delay;    
        std::string _pathstr;
        
        class DataThread: virtual public IceUtil::Thread {
        public: 
                DataThread(DataHelper& helper,AbstractCheckerAdapter<C>& adapter) :
                        helper_(helper),
                        adapter_(adapter){
                        }
                virtual void run(){
                        while ( true ){ 
                                std::vector<Entry> todo;
                                helper_.swap(todo);
                                if( todo.empty() ){
                                        MCE_INFO(helper_.getId() <<" DataHelper::timedwait [DataHelper::DataThread::run] addData: 0 entries." );
                                        continue;
                                }
                                MCE_INFO( helper_.getId() << " [DataHelper::DataThread::run] addData:  " << todo.size() << " entries" );
                                std::vector<Entry> block;       
                                for ( std::vector< Entry >::iterator it = todo.begin();
                                                it != todo.end(); ++it ){
                                        block.push_back(*it);
                                        if(block.size() >= BLOCK || *it == todo.back()){
                                                helper_.handle(adapter_,block);
                                                block.clear();
                                        }
                                }
                        }
                }
        private:
                DataHelper& helper_;
                AbstractCheckerAdapter<C>& adapter_;
        };
        typedef IceUtil::Handle<DataThread> DataThreadPtr;

        DataThreadPtr _dataThread;
};

class AddDataHelper : virtual public DataHelper, virtual public IceUtil::Shared{
public:
        virtual ~AddDataHelper(){}
        virtual void handle(AbstractCheckerAdapter<C>& adapter,std::vector<Entry>& batch){
                try{
                        adapter.getCheckerManagerOneway(0)->addBatch( batch);
                } catch( const Ice::NoEndpointException& e) {
                        MCE_WARN( " AddDataHelper adapter.getCheckerManagerOneway(0)->addBatch Ice::NoEndpointException" << e.what());
                } catch( const Ice::Exception& e){
                        MCE_WARN( " AddDataHelper adapter.getCheckerManagerOneway(0)->addBatch Ice::Exception" << e.what());
                } catch( ... ){
                        MCE_WARN( " AddDataHelper adapter.getCheckerManagerOneway(0)->addBatch Exception" );
                }
        }
};
typedef IceUtil::Handle<AddDataHelper> AddDataHelperPtr;

class NotifyDataHelper : virtual public DataHelper, virtual public IceUtil::Shared{
public:
        virtual ~NotifyDataHelper(){}
        virtual void handle(AbstractCheckerAdapter<C>& adapter,std::vector<Entry>& batch){
                try{
                        adapter.getCheckerManagerOneway(0)->notifyBatch( batch);
                } catch( const Ice::NoEndpointException& e) {
                        MCE_WARN( " NotifyDataHelper adapter.getCheckerManagerOneway(0)->notifyBatch Ice::NoEndpointException" << e.what());
                } catch( const Ice::Exception& e){
                        MCE_WARN( " NotifyDataHelper adapter.getCheckerManagerOneway(0)->notifyBatch Ice::Exception" << e.what());
                } catch( ... ){
                        MCE_WARN( " NotifyDataHelper adapter.getCheckerManagerOneway(0)->notifyBatch Exception" );
                }
        }
};
typedef IceUtil::Handle<NotifyDataHelper> NotifyDataHelperPtr;

public:

        AbstractCheckerAdapter(){
        }

	virtual void initialize( std::string endpoints, std::string path, std::string step, std::string replica ){
		_name = "";
		_endpoints = endpoints;
		_cluster = 0;
		_path = path;
		_step = step;
		_replica = replica;
		_addData = new AddDataHelper;
		_addData->initialize(*this, "AddDataHelper",false,path);
		_notifyData = new NotifyDataHelper;
		_notifyData->initialize(*this, "NotifyDataHelper",true,path);
	}

	void add(const OperaDataPtr& opera, const std::string& checksum) {
		getCheckerManagerOneway(0)->add( _path, _step, _replica, opera->toString(), checksum);
	}

	void asycAdd(const OperaDataPtr& opera, const string& checksum) {
		Entry entry;
                entry.path=_path;
                entry.step=_step;
                entry.replica=_replica;
                entry.key=opera->toString();
                entry.checksum=checksum;
		_addData->add(entry);
	}

	void notify(const OperaDataPtr& opera, const string& checksum) {
        	getCheckerManagerOneway(0)->notify( _path, _step, _replica, opera->toString(), checksum);
	}

	void asycNotify(const OperaDataPtr& opera, const string& checksum) {
		Entry entry;
		entry.path=_path;
		entry.step=_step;
		entry.replica=_replica;
		entry.key=opera->toString();
		entry.checksum=checksum;
		_notifyData->add(entry);
	}

	bool reloadConfig() {
		bool success = locate<CheckerManagerPrx>(_managers, "M", 0, MyUtil::TWO_WAY)->reloadConfig();
		return success;
	}

protected:
	CheckerManagerPrx getCheckerManager(int id) {
                return locate<CheckerManagerPrx>(_managers, "M", id, MyUtil::TWO_WAY);
        }
	CheckerManagerPrx getCheckerManagerOneway(int id) {
                return locate<CheckerManagerPrx>(_managersOneway, "M", id, MyUtil::ONE_WAY);
        }

        virtual string name() {
                return _name;
        }
        virtual string endpoints() {
                return _endpoints;
        }
        virtual size_t cluster() {
                return _cluster;
        }
private:
	typedef vector<CheckerManagerPrx> CheckerManagerSeq;
	CheckerManagerSeq _managers;
	CheckerManagerSeq _managersOneway;
	string _path;
	string _step;
	string _replica;	
	AddDataHelperPtr _addData;
	NotifyDataHelperPtr _notifyData;

};

};
};
#endif
