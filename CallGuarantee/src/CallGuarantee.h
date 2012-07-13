#ifndef CALLGUARANTEE_H
#define CALLGUARANTEE_H

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <queue>
#include <ServiceI.h>

namespace callguarantee {

class CallGuarantee;
typedef IceUtil::Handle<CallGuarantee> CallGuaranteePtr;

class CallGuarantee : public Ice::Object {
public:
	CallGuarantee(int waittime = 2, unsigned int waitsize = 1000, int redocount = 2, unsigned int redosize = 10000) :
		doer_(this, waitsize, waittime), redoer_(this, redocount, redosize) {
		doer_.start();
		redoer_.start();
	}

	virtual ~CallGuarantee() {}

	void Add(int id, const Ice::ObjectPtr& object) {
		doer_.Add(id, object);
	}

	void Add(const std::vector<std::pair<int, Ice::ObjectPtr> >& data) {
		doer_.Add(data);
	}

	void Add(const std::vector<int>& ids, int time) {
		redoer_.Add(ids, time);
	}

	void DoInTime(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
		bool flag = false;
		try {
			flag = Do(params);
		} catch (...) {
			MCE_WARN("CallGuarantee::DoInTime -> unknown exception for " << typeid(*this).name());
		}
		if (!flag) {
			std::vector<int> ids;
			for (std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator i = params.begin(); i != params.end(); ++i) {
				ids.push_back(i->first);
			}
			Add(ids, 0);
		}
	}

	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) = 0;

	virtual bool Redo(const std::vector<int>& ids) {return true;}

private:
	class DoThread : public IceUtil::Thread {
	private:
		CallGuaranteePtr guarantee_;
		std::vector<std::pair<int, Ice::ObjectPtr> > queue_;
		unsigned int waitsize_;
		int waittime_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;

	public:
		DoThread(const CallGuaranteePtr& guarantee, unsigned int waitsize, int waittime) :
			guarantee_(guarantee), waitsize_(waitsize), waittime_(waittime) {}

		void Add(int id, const Ice::ObjectPtr& object) {
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			queue_.push_back(std::pair<int, Ice::ObjectPtr>(id, object));
			if (queue_.size() > waitsize_) {
				mutex_.notify();
			}
		}

		void Add(const std::vector<std::pair<int, Ice::ObjectPtr> >& data) {
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			queue_.insert(queue_.end(), data.begin(), data.end());
			if (queue_.size() > waitsize_) {
				mutex_.notify();
			}
		}

		virtual void run() {
			while (true) {
				bool flag = false;
				std::vector<std::pair<int, Ice::ObjectPtr> > doqueue;
				try {
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
						if (queue_.size() <= waitsize_) {
							mutex_.timedWait(IceUtil::Time::seconds(waittime_));
						}
						if (! queue_.empty()) {
							doqueue.swap(queue_);
						} else {
							continue;
						}
					}
					flag = guarantee_->Do(doqueue);
				} catch (...) {
					MCE_WARN("CallGuarantee::DoThread::run -> unknown exception for "<<typeid(*guarantee_).name());
				}
				if (! flag) {
					std::vector<int> ids;
					for (std::vector<std::pair<int, Ice::ObjectPtr> >::iterator i = doqueue.begin(); i != doqueue.end(); ++i) {
						ids.push_back(i->first);
					}
					guarantee_->Add(ids, 0);
				}
			}
		}	
	};

	class RedoThread : public IceUtil::Thread {
	private:
		CallGuaranteePtr guarantee_;
		std::queue<std::pair<std::vector<int>, int> > queue_;
		int redotime_;
		unsigned int redosize_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
	
	public:
		RedoThread(const CallGuaranteePtr& guarantee, int redotime, unsigned int redosize) :
			guarantee_(guarantee), redotime_(redotime), redosize_(redosize) {}
	
		void Add(const std::vector<int>& ids, int time) {
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if (queue_.size() < redosize_) {
					queue_.push(std::pair<std::vector<int>, int>(ids, time));
					mutex_.notify();
					return;
				}
			}
			Log(ids);
		}
	
		void Log(const std::vector<int>& ids) const {
			ostringstream os;
			for (std::vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
				if (i != ids.begin()) {
					os << ", ";
				}
				os << *i;
			}
			MCE_ERROR("CallGuarantee::RedoThread::Log -> Name: "<< typeid(*guarantee_).name() << " list: " << os.str());
		}
	
		virtual void run() {
			while (true) {
				bool flag = false;
				std::vector<int> redoqueue;
				int time = 0;
				try {
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
						if (queue_.size() <= 0) {
							mutex_.wait();
						}
						redoqueue.swap(queue_.front().first);
						time = queue_.front().second;
						queue_.pop();
					}
					if (time < redotime_) {
						flag = guarantee_->Redo(redoqueue);
					} else {
						Log(redoqueue);
						flag = true;
					}
				} catch (...) {
					MCE_WARN("CallGuarantee::RedoThread::run -> unknown exception for " << typeid(*guarantee_).name());
				}
				if (! flag) {
					guarantee_->Add(redoqueue, ++time);
				}
			}
		}
	};

	DoThread doer_;
	RedoThread redoer_;

};


}

#endif
