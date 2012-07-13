#ifndef CELL_SOCIALGRAPH_ZKCLIENT_H__
#define CELL_SOCIALGRAPH_ZKCLIENT_H__

#include <string>
#include <list>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/foreach.hpp>

#include "zookeeper/zookeeper.h"

#include "base/hashmap.h"

namespace xce {

class Observer {
public:
	virtual void Update(const std::string&, const std::string&) = 0;
	virtual ~Observer() {}
};

typedef boost::shared_ptr<Observer> ObserverPtr;

class Subject {
public:
	Subject() {

	}
	void Attach(Observer* observer) {
		boost::lock_guard<boost::mutex> guard(mutex);
    if (observer) {
      observer_list_.push_back(observer);
    }
/*
    if (observer) {
      ObserverPtr ob;
      ob.reset(observer);
      observer_list_.push_back(ob);
    }
*/

	}

	void Detach(Observer* observer) {
		boost::lock_guard<boost::mutex> guard(mutex);
    if (observer) {
      observer_list_.remove(observer);
    }

/*
	  if (observer) {
      BOOST_FOREACH(ObserverPtr& ob, observer_list_) {
        if (ob.get() == observer) {
          observer_list_.remove(ob);
        }
      }     
    }
*/
	}

	void NotifyObservers(const std::string& key, const std::string& value) {
		boost::lock_guard<boost::mutex> guard(mutex);
    BOOST_FOREACH(Observer* ob, observer_list_) {
      ob->Update(key, value);
    }     

/*
    BOOST_FOREACH(ObserverPtr& ob, observer_list_) {
      ob->Update(key, value);
    }     
*/
	}
	virtual ~Subject() {
    
    BOOST_FOREACH(Observer* ob, observer_list_) {
      if (ob) {
        delete ob;
      }
    }     
    //observer_list_.clear();
	}
protected:
  boost::mutex mutex; 
  std::list<xce::Observer*> observer_list_;
//	std::list<xce::ObserverPtr> observer_list_; 
};


class ZooConfig : public xce::Subject {
public:
  static ZooConfig& GetInstance();
  ZooConfig():zh_(0) { Init(); }
  virtual ~ZooConfig() {Destroy();}
  void Init();

  bool Get(const std::string&, std::string*);
  bool Set(const std::string&, const std::string*);
  bool Update(const std::string&);
	bool Delete(const std::string&);

	void Destroy();


private:
	void Reset();
private:
  std::hash_map<std::string, std::string> map_;
  zhandle_t* zh_;
	char buffer_[10240];
	int buflen_;
};

}
#endif

