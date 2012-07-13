#include "SubjectObserverI.h"
#include "Date.h"
#include <boost/lexical_cast.hpp>
#include "util/cpp/String.h"

using namespace std;
using namespace MyUtil;

// ----- SubjectObserverI ----------------------------------
SbjTopicPtr SubjectObserverI::getSbjTopic(const string& name,
                                      const Ice::Current& current)  {
    	return query(name, current);
}

SbjTopicPtr SubjectObserverI::query(const string& name,
                                const Ice::Current& current)	{
	IceUtil::Mutex::Lock lock(*this);
	SbjTopicMap::iterator pos = topics.find(name);
	if ( pos != topics.end() ) {
		MCE_DEBUG("SubjectObserverI::query success -> " << name);
		return pos->second;
	}
	return 0;
}

void SubjectObserverI::subscribe(const string& name,
                                    const SbjSubscriberPrx& subscriber,
                                    const Ice::Current& current)
{
	//if prx has no timeout behavior, we set it to 300 ms, to make sure no one can block the NotifyTask
	SbjSubscriberPrx subscriberT = subscriber;
	if( subscriber->ice_toString().find( " -t " ) == std::string::npos ) {
		MCE_WARN("SubjectObserverI::subscribe -> Not set timeout -> " << name << " -> " << subscriber->ice_toString() );
		subscriberT = SbjSubscriberPrx::uncheckedCast( subscriber->ice_timeout( 300 ) );
	}
	MCE_DEBUG("SubjectObserverI::subscribe -> " << name << " -> " << subscriberT->ice_toString() );
	IceUtil::Mutex::Lock lock(*this);
	SbjTopicMap::iterator postopic = topics.find(name);
	if ( postopic == topics.end() ) {
		SbjTopicPtr sbjt = new SbjTopic;
		sbjt->name = name;
		sbjt->timestamp = time(NULL);
		topics.insert(make_pair(name, sbjt));
	}
	SbjSubscriberMap::iterator possub = subscribers.find(name);
	if ( possub == subscribers.end() ) {
		pair<SbjSubscriberMap::iterator, bool> result = 
			subscribers.insert(make_pair(name, SbjSubscriberSeq()));		
		if ( result.second ) {
			result.first->second.push_back(subscriberT);
		}
	}
	else {
		SbjSubscriberSeq& sbjss = possub->second;
		if ( find(sbjss.begin(), sbjss.end(), subscriberT) == sbjss.end() ) {
			sbjss.push_back(subscriberT);
		}
	}
}

void SubjectObserverI::unsubscribe(const string& name,
                                      const SbjSubscriberPrx& subscriber,
                                      const Ice::Current& current)	{
	IceUtil::Mutex::Lock lock(*this);
	doUnsubscribe(name, subscriber);
}


void SubjectObserverI::doUnsubscribe(const string& name, const SbjSubscriberPrx& subscriber) {

	SbjSubscriberMap::iterator possub = subscribers.find(name);
	if ( possub != subscribers.end() ) {
		SbjSubscriberSeq& sbjss = possub->second;
		sbjss.erase(remove(sbjss.begin(), sbjss.end(), subscriber), sbjss.end());
		MCE_DEBUG("[SubjectObserverI::unsubscribe] unsubscribe " << name << ", "
				<< subscriber->ice_toString());
	}
}

void SubjectObserverI::notifyall(const string& name,
                                 const SbjStatePtr& state,
                                 const Ice::Current& current)
{
	SbjTopicPtr oldTopic = getSbjTopic(name);
	if ( !oldTopic ) {
		MCE_WARN("[SubjectObserverI::notifyall] notify " << name << " is NULL");
		return;
	}
	SbjTopicPtr sbjt = SbjTopicPtr::dynamicCast(oldTopic->ice_clone());
	sbjt->timestamp = time(NULL);

	MCE_DEBUG("[XT] " << sbjt->name << " : " << subscribers[sbjt->name].size());
	
	update(sbjt, state, current.ctx);

}

SbjStateSeq SubjectObserverI::fetch(const string& name, 
				const SbjStatePtr& state, 
				const Ice::Current& current) {
	SbjStateSeq receives;
	SbjTopicPtr oldTopic = getSbjTopic(name);
	if ( !oldTopic ) {
		MCE_WARN("SubjectObserverI:fetch " << name << " is NULL");
		return receives;
	}
	SbjTopicPtr sbjt = SbjTopicPtr::dynamicCast(oldTopic->ice_clone());
	sbjt->timestamp = time(NULL);
	get(receives, sbjt, state, current.ctx);
	MCE_DEBUG("[TX] " << sbjt->name << " : " << subscribers[sbjt->name].size());
	
	return receives;
}
	
void SubjectObserverI::update(const SbjTopicPtr& sbjt, const SbjStatePtr& state, const Ice::Context& ctx) {

	IceUtil::Mutex::Lock lock(*this);
	SbjTopicPtr orig;
	SbjTopicMap::iterator pos = topics.find(sbjt->name);
	if ( pos != topics.end() ) {
		orig = pos->second;
	}
	topics[sbjt->name] = sbjt;
	if ( orig && orig->timestamp < sbjt->timestamp) {
		MCE_INFO("SubjectObserverI::updated -> " << sbjt->name << ", Old: "
			<< MyUtil::Date(orig->timestamp).str() << ", New: " << MyUtil::Date(sbjt->timestamp).str());
		SbjSubscriberMap::iterator pos1 = subscribers.find(sbjt->name);
		if (pos1 != subscribers.end()) {
			TaskManager::instance().execute(new NotifyTask(*this, sbjt, pos1->second, state, ctx));
		}
	}
}

void SubjectObserverI::get(vector<SbjStatePtr>& receives, const SbjTopicPtr& sbjt, const SbjStatePtr& state, const Ice::Context& ctx) {

	IceUtil::Mutex::Lock lock(*this);
	SbjTopicPtr orig;
	SbjTopicMap::iterator pos = topics.find(sbjt->name);
	if ( pos != topics.end() ) {
		orig = pos->second;
	}
	topics[sbjt->name] = sbjt;
	if ( orig && orig->timestamp < sbjt->timestamp) {
		MCE_INFO("[SubjectObserverI::get] SubjectObserverI::updated&fetch -> " << sbjt->name << ", Old: "
			<< MyUtil::Date(orig->timestamp).str() << ", New: " << MyUtil::Date(sbjt->timestamp).str());
		SbjSubscriberMap::iterator pos1 = subscribers.find(sbjt->name);
		if (pos1 != subscribers.end()) {
			SbjSubscriberSeq subscriberSeq = pos1->second;
			SbjSubscriberSeq failedSubscriberSeq;
			//the first time we check the items, if some Exceptions occured, we just put it to the failedSeq, and check it again later
			for (SbjSubscriberSeq::iterator it = subscriberSeq.begin(); it != subscriberSeq.end(); ++it) {
				try {
					SbjStatePtr recv = (*it)->opera(state, ctx);
					receives.push_back(recv);
					MCE_DEBUG("[RE_receives] : " << sbjt->name << " : " << receives.size());			
					MCE_DEBUG("[SubjectObserverI::get] Notify&Fetch success -> " << sbjt->name << " -> " << *it);
				} catch (Ice::Exception& e) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch Ice::Exception -> " << sbjt->name << " -> " << *it << " check again " <<	(*it)->ice_toString() << " : " << e.what());
					failedSubscriberSeq.push_back(*it);
					continue;
				} catch (std::exception& e) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch std::exception -> " << sbjt->name << " -> " << *it << " check again " <<	(*it)->ice_toString() << " : " << e.what());
					failedSubscriberSeq.push_back(*it);
					continue;
				} catch (...) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch exception -> " << sbjt->name << " -> " << *it << " check again " << (*it)->ice_toString() << " : UNKWOWN");
					failedSubscriberSeq.push_back(*it);
					continue;
				}
			}
			//check the failed items again to make sure that the Exception we got is correct or just a little probability event
			//and if this time the item also has Exceptions we confirm that it can not be pushed to client
			for (SbjSubscriberSeq::iterator it = failedSubscriberSeq.begin(); it != failedSubscriberSeq.end(); ++it) {
				try {
					SbjStatePtr recv = (*it)->opera(state, ctx);
					receives.push_back(recv);
					MCE_DEBUG("[RE_receives] : " << sbjt->name << " : " << receives.size());			
					MCE_DEBUG("[SubjectObserverI::get] Notify&Fetch success -> " << sbjt->name << " -> " << *it);
				} catch (Ice::Exception& e) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch Ice::Exception -> " << sbjt->name << " -> " << *it << " unsubscribe " <<	(*it)->ice_toString() << " : " << e.what());
					doUnsubscribe(sbjt->name, (*it));
					continue;
				} catch (std::exception& e) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch std::exception -> " << sbjt->name << " -> " << *it << " unsubscribe " <<	(*it)->ice_toString() << " : " << e.what());
					doUnsubscribe(sbjt->name, (*it));
					continue;
				} catch (...) {
					MCE_WARN("[SubjectObserverI::get] Notify&Fetch exception -> " << sbjt->name << " -> " << *it << " unsubscribe " << (*it)->ice_toString() << " : UNKWOWN");
					doUnsubscribe(sbjt->name, (*it));
					continue;
				}
			}
		}
	}							
}

// ----- NotifyTask ------------------------------------------------
void NotifyTask::handle() {
	MCE_DEBUG("NotifyTask : " << _subscribers.size() << " subscribers");
	int count = 0;
	for (SbjSubscriberSeq::iterator i = _subscribers.begin(); i != _subscribers.end(); ++i) {		
		try {
			(*i)->opera(_state, _ctx);
			MCE_DEBUG("NotifyTask::handle success -> " << _topic->name << " -> " << (*i)->ice_toString());
			count++;
		} catch (Ice::Exception& e) {
			MCE_WARN("NotifyTask::handle exception -> " << _topic->name << " -> " << (*i)->ice_toString()<< " -> " << e);
			_observer.unsubscribe(_topic->name, (*i));
			count++;
			continue;
		} catch (std::exception& e) {
			MCE_WARN("NotifyTask::handle exception -> " << _topic->name << " -> " << (*i)->ice_toString() << " -> " << e.what());
			_observer.unsubscribe(_topic->name, (*i));
			count++;
			continue;
		} catch (...) {
			MCE_WARN("NotifyTask::handle exception -> " << _topic->name << " -> " << (*i)->ice_toString() << " unknown");
			_observer.unsubscribe(_topic->name, (*i));
			count++;
			continue;
		}
	}
	MCE_INFO("NotifyTask::handle -> NotifyTask Count: "<<count);
}

