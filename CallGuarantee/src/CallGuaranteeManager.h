#ifndef __CALLGUARANTEEMANAGER_H__
#define __CALLGUARANTEEMANAGER_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <queue>
#include <ServiceI.h>

namespace callguarantee{

class GuaranteeHandle;
typedef IceUtil::Handle<GuaranteeHandle> GuaranteeHandlePtr;

class GuaranteeHandle : public Ice::Object
{
	public:
		GuaranteeHandle()
		{
			timeval tmp;
			gettimeofday(&tmp, NULL);
			_createTime = (tmp.tv_sec*1000*1000) + tmp.tv_usec;
		}
	public:
		virtual bool doHandle() = 0;
		virtual bool reDoHandle() = 0;
		virtual void merge(const GuaranteeHandlePtr& newData) = 0;
		virtual string getInfo()
		{
			return "Can't finish call!";
		}

	public:

		long _createTime;

};


class CallGuaranteeManager;
typedef IceUtil::Handle<CallGuaranteeManager> CallGuaranteeManagerPtr;

class CallGuaranteeManager : public Ice::Object {
public:
	CallGuaranteeManager(int redocount = 2, unsigned int redosize = 10000) :
		doer_(this), redoer_(this, redocount, redosize) {
		doer_.start();
		redoer_.start();
	}

	virtual ~CallGuaranteeManager() {}

	void Add(const int id, const GuaranteeHandlePtr& obj) 
	{
		if( mergeIfAlreadyExist(id, obj) == false )
		{
			doer_.Add(id, obj);
		}
	}

	void Add(const int id, const GuaranteeHandlePtr& obj, int time)
	{
		if( mergeIfAlreadyExist(id, obj) == false )
		{
			redoer_.Add(id, obj, time);
		}
	}

	void DoInTime(const int id, const GuaranteeHandlePtr& obj)
	{
		bool flag = false;
		try {
			flag = Do(obj);
		} catch (...) {
			MCE_WARN("CallGuaranteeManager::DoInTime -> unknown exception for " << typeid(*this).name());
		}
		if (!flag) {
			Add(id, obj, 0);
		}
	}

	virtual bool Do(const GuaranteeHandlePtr& obj)
	{
		return obj->doHandle();
	}

	virtual bool Redo(const GuaranteeHandlePtr& obj) 
	{
		return obj->reDoHandle();
	}

	void eraseIdToHandleMapObj(int id)
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		std::map<int, GuaranteeHandlePtr>::iterator findIt = idToHandleMap_.find(id);
		if( findIt != idToHandleMap_.end())
		{
			idToHandleMap_.erase(findIt);
		}
	}

protected:

	bool mergeIfAlreadyExist(int id, const GuaranteeHandlePtr& newData)
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		std::map<int, GuaranteeHandlePtr>::iterator findIt = idToHandleMap_.find(id);
		if( findIt == idToHandleMap_.end())
		{
			idToHandleMap_.insert(make_pair(id, newData));
			return false;
		}else
		{
			if( newData->_createTime > findIt->second->_createTime)
			{
				findIt->second->merge(newData);
				findIt->second->_createTime = newData->_createTime;
			}else
			{
				newData->merge(findIt->second);
				findIt->second = newData;
			}
			return true;
		}
		return true;
	}

private:
	class DoThread : public IceUtil::Thread {
	private:
		CallGuaranteeManagerPtr guarantee_;
		std::queue<std::pair<int,GuaranteeHandlePtr> > queue_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;

	public:
		
		DoThread(const CallGuaranteeManagerPtr& guarantee): guarantee_(guarantee)
		{
		}

		void Add(const int id, const GuaranteeHandlePtr& obj) {
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			queue_.push(make_pair(id, obj));
			if ( !queue_.empty() ) {
				mutex_.notify();
			}
		}

		virtual void run() {
			while (true) {
				bool flag = false;
				int id = 0;
				GuaranteeHandlePtr doObj;
				try 
				{
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
						if ( queue_.empty()) {
							mutex_.wait();
						}
						std::pair<int, GuaranteeHandlePtr> tmp = queue_.front();
						doObj = tmp.second;
						id = tmp.first;
						queue_.pop();
					}
				
					guarantee_->eraseIdToHandleMapObj(id);
				
					flag = guarantee_->Do(doObj);
				} catch (...) {
					MCE_WARN("CallGuaranteeManager::DoThread::run -> unknown exception for "<<typeid(*guarantee_).name());
				}
				if (! flag) {
					guarantee_->Add(id, doObj, 0);
				}
			}
		}	
	};

	class RedoThread : public IceUtil::Thread {
	private:
		CallGuaranteeManagerPtr guarantee_;
		std::queue<std::pair<int,std::pair<GuaranteeHandlePtr, int> > > queue_;
		int redotime_;
		unsigned int redosize_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
	
	public:
		RedoThread(const CallGuaranteeManagerPtr& guarantee, 
				   int redotime, unsigned int redosize) :
				   guarantee_(guarantee), redotime_(redotime), redosize_(redosize)
		{
		}
	
		void Add(const int id, const GuaranteeHandlePtr& obj, int time) {
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if (queue_.size() < redosize_) {
					queue_.push(make_pair(id, make_pair(obj, time)));
					mutex_.notify();
					return;
				}
			}
			Log(obj);
		}
	
		void Log(const GuaranteeHandlePtr& obj) const {
			ostringstream os;
			os << obj->getInfo();
			MCE_WARN("CallGuaranteeManager::RedoThread::Log -> Name: "<< typeid(*guarantee_).name() << " list: " << os.str());
		}
	
		virtual void run() {
			while (true) {
				bool flag = false;
				int id = 0;
				GuaranteeHandlePtr redoObj;
				int time = 0;
				try 
				{
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
						if (queue_.size() <= 0) {
							mutex_.wait();
						}
						id = queue_.front().first;
						redoObj = queue_.front().second.first;
						time = queue_.front().second.second;
						queue_.pop();
					}
					guarantee_->eraseIdToHandleMapObj(id);
					
					if (time < redotime_) {
						flag = guarantee_->Redo(redoObj);
					} else {
						Log(redoObj);
						flag = true;
					}
				} catch (...) {
					MCE_WARN("CallGuaranteeManager::RedoThread::run -> unknown exception for " << typeid(*guarantee_).name());
				}
				if (! flag) {
					guarantee_->Add(id, redoObj, ++time);
				}
			}
		}
	};

protected: 

	std::map<int,GuaranteeHandlePtr> idToHandleMap_;
	
	IceUtil::Monitor<IceUtil::Mutex> mutex_;

	DoThread doer_;
	RedoThread redoer_;

};


}

#endif
