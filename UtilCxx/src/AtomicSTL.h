#ifndef __ATOMIC_STL_H__
#define __ATOMIC_STL_H__

#include <queue>

namespace xce {
namespace atomicstl {

// ---------- 线程安全的map结构 ----------------------------------
template<class K, class V> 
class AtomicMap : public IceUtil::Mutex {
public:	
	void put(const K& key, const V& value) {
		IceUtil::Mutex::Lock lock(*this);

		if ( _map.find(key) != _map.end() ) {
			_map[key] = value;
		}
		else {
			_map.insert( make_pair<K, V>(key, value) );
		}
	};

	V& operator [](const K& key) {
		IceUtil::Mutex::Lock lock(*this);
		return _map[key];
	};

	bool has(const K& key) {
		IceUtil::Mutex::Lock lock(*this);
		if ( _map.find(key) != _map.end() ) {
			return true;
		} else {
			return false;
		}
	};

	size_t erase(const K& key) {
		IceUtil::Mutex::Lock lock(*this);
		return _map.erase(key);
	};

	size_t size() {
		IceUtil::Mutex::Lock lock(*this);
		return _map.size();
	};

	std::map<K, V> getMap() {
		IceUtil::Mutex::Lock lock(*this);
		return _map;
	};

private:
	std::map<K, V> _map;
};


// ----------- 线程安全的queue ------------------------------------
template<class T> class AtomicQueue : public IceUtil::Mutex	{
public:
	void push(const T& elem)  {
		IceUtil::Mutex::Lock lock(*this);
		_queueT.push(elem);
	};

	void pushSeq(const vector<T>& seq) {
		IceUtil::Mutex::Lock lock(*this);
		for (size_t i = 0; i < seq.size(); ++ i) {
			_queueT.push(seq[i]);
		}
	};

	T popfront() {
		IceUtil::Mutex::Lock lock(*this);
		T t = _queueT.front();
		_queueT.pop();
		return t;
	};
	
	T popfront(size_t& nQueueSize) {
		IceUtil::Mutex::Lock lock(*this);
		T t = _queueT.front();
		_queueT.pop();
		nQueueSize = _queueT.size();
		return t;
	};

	int popfrontSeq(vector<T>& seq, size_t nSeqSize) {
		IceUtil::Mutex::Lock lock(*this);
		size_t nValidSize = nSeqSize;
		if ( _queueT.size() < nValidSize ) {
			nValidSize = _queueT.size();
		}
		for ( size_t i = 0; i < nValidSize; i++ ) {
			T t = _queueT.front();
			seq.push_back(t);
			_queueT.pop();
		}
		return nValidSize;
	}

	int size() {
		IceUtil::Mutex::Lock lock(*this);
		return _queueT.size();
	};

	bool empty() {
		IceUtil::Mutex::Lock lock(*this);
		return _queueT.empty();
	};

private:
	std::queue<T> _queueT;
};

// ----------- 线程安全的blockingqueue ------------------------------------
template<class T> class BlockingQueue {
	
	public:
		void setProps(int maxSize, int getWaitTime=0,int addWaitTime=0)
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
			m_maxSize = (size_t) maxSize;
			m_getWaitTime = getWaitTime;
			m_addWaitTime = addWaitTime;
		}

	private:
		int             m_getWaitTime;
		int             m_addWaitTime;
		size_t             m_maxSize;
		std::queue<T>   m_queueT;
		
		IceUtil::Monitor<IceUtil::Mutex>	m_mutex;

	public:
	void push(const T& elem)  {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		while(m_queueT.size()+1 > m_maxSize)
		{
			m_mutex.wait();
		}
		m_queueT.push(elem);
		m_mutex.notify();
	};

	void pushSeq(const vector<T>& seq) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		
		while(m_queueT.size()+seq.size() > m_maxSize)
		{
			m_mutex.wait();
		}
	
		for (size_t i = 0; i < seq.size(); ++ i) {
			m_queueT.push(seq[i]);
		}
		m_mutex.notify();
	};

	T front() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		while(m_queueT.size()<=0)
		{
			m_mutex.wait();
		}
		T t = m_queueT.front();
		m_mutex.notify();
		return t;
	};
	
	T popfront() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		while(m_queueT.size()<=0)
		{
			m_mutex.wait();
		}
		T t = m_queueT.front();
		m_queueT.pop();
		m_mutex.notify();
		return t;
	};
	
	T popfront(size_t& nQueueSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		while(m_queueT.size()<=0)
		{
			m_mutex.wait();
		}
		T t = m_queueT.front();
		m_queueT.pop();
		nQueueSize = m_queueT.size();
		m_mutex.notify();
		return t;
	};

	int popfrontSeq(vector<T>& seq, size_t nSeqSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex); 
		while(m_queueT.size()<=0)
		{
			m_mutex.wait();
		}
		
		size_t nValidSize = nSeqSize;
		if ( m_queueT.size() < nValidSize ) {
			nValidSize = m_queueT.size();
		}
		for ( size_t i = 0; i < nValidSize; i++ ) {
			T t = m_queueT.front();
			seq.push_back(t);
			m_queueT.pop();
		}
		m_mutex.notify();
		return nValidSize;
	}
	
	int getSize() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		return m_queueT.size();
	};

	bool isEmpty() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		return m_queueT.empty();
	};

//以下方法将根据等待时间进行wait
	bool pushByTime(const T& elem)  {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		if(m_mutex.timedWait(IceUtil::Time::seconds(m_addWaitTime)))
		{
			if(m_queueT.size()+1<=m_maxSize)
			{ 
				m_queueT.push(elem);
				m_mutex.notify();
				return true;
			}else
			{
				m_mutex.notify();
				return false;
			}
		}
		m_mutex.notify();
		return false;
	};

	bool pushSeqByTime(const vector<T>& seq) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		if(m_mutex.timedWait(IceUtil::Time::seconds(m_addWaitTime)))
		{ 
			if(m_queueT.size()+seq.size()<=m_maxSize)
			{ 
				for (size_t i = 0; i < seq.size(); ++ i) {
					m_queueT.push(seq[i]);
				}
				m_mutex.notify();
				return true;
			}else
			{
				m_mutex.notify();
				return false;
			}
		}
		m_mutex.notify();
		return false;
	};

	T popfrontByTime() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		if(m_mutex.timedWait(IceUtil::Time::seconds(m_getWaitTime)))
		{ 
			T t = m_queueT.front(); 
			m_queueT.pop();
			m_mutex.notify(); 
			return t;
		}
		m_mutex.notify();
		return NULL;
	};
	
	T popfrontByTime(size_t& nQueueSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		if(m_mutex.timedWait(IceUtil::Time::seconds(m_getWaitTime)))
		{ 
			T t = m_queueT.front();
			m_queueT.pop();
			nQueueSize = m_queueT.size(); 
			m_mutex.notify();
			return t;
		}
		m_mutex.notify();
		return NULL;
	};

	int popfrontSeqByTime(vector<T>& seq, size_t nSeqSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
		
		if(m_mutex.timedWait(IceUtil::Time::seconds(m_getWaitTime)))
		{ 
			size_t nValidSize = nSeqSize;
			if ( m_queueT.size() < nValidSize ) {
				nValidSize = m_queueT.size();
			}
			for ( size_t i = 0; i < nValidSize; i++ ) {
				T t = m_queueT.front();
				seq.push_back(t);
				m_queueT.pop();
			}
			m_mutex.notify();
			return nValidSize;
		}
		m_mutex.notify();
		return 0;
	};
};


};
};
#endif
