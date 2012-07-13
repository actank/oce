#ifndef PIPE_H_
#define PIPE_H_

#include "IceLogger.h"
#include "TaskManager.h"


namespace MyUtil {

typedef vector<Ice::ObjectPtr> ObjectSeq;

class Pipe : public Timer {
	ObjectSeq _queue;
	IceUtil::Mutex _mutex;
	static size_t _timeslot;

	string _name;
	int _index;

	void pop(ObjectSeq& seq) {
		IceUtil::Mutex::Lock lock(_mutex);
		if(!_queue.empty()){
			_queue.swap(seq);
		}
	}
	;

public:

	Pipe(const string& name, int index) :
		Timer(Pipe::_timeslot), _name(name), _index(index) {
		TaskManager::instance().schedule(this);
//		TaskManager::instance().scheduleRepeated(this);
	}
	;
	string& getName() {
		return _name;
	}
	int getIndex() {
		return _index;
	}
	;

	virtual void handle() {
		TaskManager::instance().schedule(this);
		try {
			ObjectSeq seq;
			pop(seq);
			if(seq.empty()){
				return;
			}
			handle(seq);
		} catch(Ice::Exception& e) {
			MCE_WARN("Pipe::handle() name:"<<_name<<" ice err:"<<e);
		} catch(std::exception& e){
			MCE_WARN("Pipe::handle() name:"<<_name<<" std err:"<<e.what());
		} catch(...){
			MCE_WARN("Pipe::handle() name:"<<_name<<" err");
		}

	}
	;

	virtual void handle(const ObjectSeq& seq) {
	}
	;

	void setTimeslot(size_t timeslot = 100) {
		_timeslot = timeslot;
	}
	;

	void push(const Ice::ObjectPtr& obj) {
		IceUtil::Mutex::Lock lock(_mutex);
		_queue.push_back(obj);
	}
	;

};

  

typedef IceUtil::Handle<Pipe> PipePtr;
//-------------------------

class PipeFactory : public IceUtil::Shared {
public:

	virtual PipePtr create(const string& type, int index) {
		return NULL;
	}
	;
};

typedef IceUtil::Handle<PipeFactory> PipeFactoryPtr;

//-------------------------

class PipePool : public Singleton<PipePool> {

	map<string , map<int,PipePtr> > _pool;

	PipeFactoryPtr _factory;
	IceUtil::Mutex _mutex;
public:
	void initialize(PipeFactoryPtr factory) {
		_factory = factory;
	}
	;

	void push(const string& name, int index, const Ice::ObjectPtr& obj) {

		IceUtil::Mutex::Lock lock(_mutex);
		if (!_factory) {
			return;
		}

		map<string , map<int,PipePtr> >::iterator it = _pool.find(name);
		if (it == _pool.end()) {

			PipePtr p = _factory->create(name, index);
			_pool[name][index] = p;
			p->push(obj);

		} else {
			map<int,PipePtr>::iterator pit = it->second.find(index);
			if (pit == it->second.end()) {
				PipePtr p = _factory->create(name, index);
				it->second[index] = p;
				p->push(obj);
			} else {
				pit->second->push(obj);
			}
		}
	}

	void setTimeslot(const string& name, int timeslot) {
		map<string , map<int,PipePtr> >::iterator it = _pool.find(name);
		if (it != _pool.end()) {
			if (!it->second.empty()) {
				it->second.begin()->second->setTimeslot(timeslot);
			}
		}
	}

};

}

#endif /*PIPE_H_*/
