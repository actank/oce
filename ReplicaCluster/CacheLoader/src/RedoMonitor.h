/**
 * @file
 * @author   
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * RedoMonitor.h：  包含虚基类RedoMonitor \n
 *                  实现类SetDataRedoMonitor，SetValidRedoMonitor
 */

#ifndef __REDOMONITOR_H__
#define __REDOMONITOR_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace redomonitor {

/**
  * @brief class RedoMonitor 
  *  实现了操作重做逻辑，分两种：
  *  1. 直接redo
  *  2. 保存重做数据，插入重做队列，由重做线程来扫描重做
  *   
  */
class RedoMonitor {
public:
	/**
	  *构造函数
          *@param T& prx --- 模板参数T类型对象
          *@param bool beUseQueue --- 是否使用重做队列
	  *@param std::string desc --- 操作情况详细描述
          */
        RedoMonitor(bool beUseQueue, std::string desc ):
                _beUseQueue(beUseQueue),_desc(desc) {
		_expired.tv_sec = 0;
		_expired.tv_nsec = 10000000;
        }
	/**
          *虚析构函数
          */
	virtual ~RedoMonitor(){}
	/**
          *对外提供的操作接口
          */
	virtual bool monitorHandle(){
                try {
                        handle();
                }
		catch (Ice::TimeoutException& e){
			MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
                        goto END;
		}
		catch (Ice::Exception& e){
			MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
			goto END;
		}
                catch (...){
			MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
                        goto END;
                }
                return true;
END:
                MCE_WARN( "[PrxAgent RedoMonitor WARN] Handle failed! Detail: "<<_desc );
                if(_beUseQueue){
                        insertRedoQueue();
                }
                else{
                        redo();
                }
                return false;
        }
protected:
	/**
          *纯虚函数，子类必须自定义操作实现
          */
        virtual void handle() = 0;
        /**
          *重做快速处理,由monitorHandle函数调用
          *TODO: 暂时通过日志观察redo失败情况，若量大，则考虑使用数据库来记录，以备恢复数据
          */
        virtual void redo() {
                try {
			nanosleep(&_expired,NULL);
			
			handle();
                }
                catch (...){
                        MCE_WARN( "[PrxAgent RedoMonitor WARN] Redo failed! detail: "<<_desc );
			//TODO: 暂时通过日志观察redo失败情况，若量大，则考虑使用数据库来记录，以备恢复数据
                }
        }
        /**
          *TODO:重做延迟处理，插入重做队列
          */
	virtual void insertRedoQueue() {}

private:
        bool _beUseQueue;
	std::string _desc;
	struct timespec _expired;
};




/**
  * @brief class SetDataRedoMonitor
  *  CacheLoaderI 使用此实现类,实现 
  */
template<class T> class SetDataRedoMonitor :  virtual public RedoMonitor,virtual public Ice::Object
{
public:
    SetDataRedoMonitor( T& prx,bool beUseQueue, std::string desc,MyUtil::ObjectResultPtr& result):
         RedoMonitor(beUseQueue,desc),_prx(prx),_result(result){
    }
    virtual void handle( ){
        _prx->setData(_result);
    }
/*
    virtual void insertRedoQueue(T& prx){
       IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_pap->getCacheLoaderI()._redomutex );
       RedoStruct redo(SETDATA,&_result);
       _pap->getCacheLoaderI().queueRedo.push(make_pair<long, RedoStruct>(0,redo));
       _pap->>getCacheLoaderI()._redomutex.notify();
    }
    virtual void saveData(T& prx){
       _pap->objectResultPtrVec.push_back(_result);
    }*/
private:
    T _prx;
    MyUtil::ObjectResultPtr& _result;
};

/**
  * @brief class SetValidRedoMonitor
  *  CacheLoaderI 使用此实现类,实现 
  */
template<class T> class SetValidRedoMonitor :  virtual public RedoMonitor,virtual public Ice::Object
{
public:
    SetValidRedoMonitor(T& prx,bool beUseQueue,std::string desc,bool valid):
        RedoMonitor(beUseQueue,desc),_prx(prx),_valid(valid){
    }
    virtual void handle( ){
        _prx->setValid( _valid );
    }
/*
    virtual void insertRedoQueue(T& prx){
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_pap->>getCacheLoaderI()._redomutex );
        RedoStruct redo(SETDATA,&_pap);
        _pap->getCacheLoaderI().queueRedo.push( make_pair<long, RedoStruct>( id,redo));
        _pap->>getCacheLoaderI()._redomutex.notify();
    }
*/
private:
    T _prx;
    bool _valid;
};


}
}

#endif //__REDOMONITOR_H__
