#ifndef __XCE_ICEEXT_DATA_SET_H__
#define __XCE_ICEEXT_DATA_SET_H__

#include "Util.h"
#include "Evictor.h"
#include "util/cpp/Type.h"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
#include "LogWrapper.h"

namespace MyUtil {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

class DataHandle;
typedef IceUtil::Handle<DataHandle> DataHandlePtr;
typedef vector<DataHandlePtr> DataHandleSeq;

class DataHandle : public Ice::Object {
protected:
	Ice::ObjectPtr _value;

public:
	virtual bool less(const DataHandlePtr& rhs) {
		return false;
	}
	virtual bool greater(const DataHandlePtr& rhs) {
		return true;
	}
	virtual bool equal(const DataHandlePtr& rhs) {
		return false;
	}

	template<class T> T value() {
		return T::dynamicCast(_value);
	}
	;

	void value(const Ice::ObjectPtr& value) {
		_value = value;
	}
	;

	virtual string getProperty(const string& key) = 0;
	virtual void setProperty(const string& key, const string& value) = 0;

	virtual Str2StrMap getProperties() = 0;
	virtual void setProperties(const Str2StrMap& properties) = 0;
};

//---------------------------------------------------------------------------
typedef Int2Type<0> NoIndex;

typedef Int2Type<0> Set;
typedef Int2Type<1> Map;
typedef Int2Type<2> Evictor;
typedef Int2Type<3> Vector;

template<class ValueT, class IndexT =NoIndex, class ContainerT =Set> class DataSetI : public Ice::Object {

};

//---------------------------------------------------------------------------
template<class ValueT, class IndexT> class DataSetI<ValueT, IndexT, Map> : public Ice::Object {
public:
	typedef vector<IndexT> IndexSeq;
	typedef vector<ValueT> ValueSeq;
	typedef map<IndexT, ValueT> ValueMap;
	typedef typename ValueMap::iterator Iterator;
	typedef typename ValueMap::reverse_iterator ReverseIterator;

private:
	ValueMap _ds;
	string _index;
	IceUtil::Mutex _mutex;

	IndexT getIndexOf(const DataHandlePtr& d) {
		return boost::lexical_cast<IndexT>(d->getProperty(_index));
	}
public:
	DataSetI(const string& index) : _index(index) {};

	ValueT find(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		Iterator pos = _ds.find(index);
		if (pos != _ds.end()) {
			return pos->second;
		} else {
			return 0;
		}
	}

	bool contains(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		return _ds.find(index) != _ds.end();
	};

	Ice::Int size() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _ds.size();
	};

	ValueT get(const IndexT& index) {
		return find(index);
	}
	ValueSeq getAll(const IndexSeq& indices) {
		ValueSeq result;
		for (int i = 0; i < indices.size(); ++i) {
			result.push_back(find(indices.at(i)));
		}
		return result;
	}
	ValueSeq getAll(
			Ice::Int offset,
			Ice::Int limit,
			bool reverse = false) {
		ValueSeq result;
		int begin = 0;
		IceUtil::Mutex::Lock lock(_mutex);
		limit = (limit < 0) ? _ds.size() : limit;
		if (reverse) {
			for (ReverseIterator it = _ds.rbegin(); it != _ds.rend(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= (size_t)limit) {
					break;
				}
				result.push_back(it->second);
			}
		} else {
			for (Iterator it = _ds.begin(); it != _ds.end(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= (size_t)limit) {
					break;
				}
				result.push_back(it->second);
			}
		}
		return result;
	}

	void add(const ValueT& d) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds[getIndexOf(d)] = d;
	}

	void addAll(const ValueSeq& ds) {
		for (size_t i = 0; i < ds.size(); ++i) {
			add(ds.at(i));
		};
	}

	void remove(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.erase(index);
	};
	void removeAll(const IndexSeq& indices) {
		for (int i = 0; i < indices.size(); ++i) {
			remove(indices.at(i));
		};
	}

	void clear() {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.clear();
	};
};

//---------------------------------------------------------------------------
template <class ValueT>
class DataSetI<ValueT, NoIndex, Set> :
public Ice::Object
{
public:
	typedef vector<ValueT> ValueSeq;
	typedef ValueT IndexT;
	typedef vector<IndexT> IndexSeq;
	typedef set<ValueT> ValueSet;
	typedef typename ValueSet::iterator Iterator;
	typedef typename ValueSet::reverse_iterator ReverseIterator;

private:
	ValueSet _ds;
	IceUtil::Mutex _mutex;

public:
	ValueT find(const IndexT& index) {
		Iterator pos = _ds.find(index);
		if (pos != _ds.end()) {
			return *pos;
		} else {
			return 0;
		}
	}

	bool contains(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		return _ds.find(index) != _ds.end();
	};

	Ice::Int size() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _ds.size();
	};

	ValueT get(const IndexT& index) {
		return find(index);
	}
	ValueSeq getAll(const IndexSeq& indices) {
		ValueSeq result;
		for (int i = 0; i < indices.size(); ++i) {
			result.push_back(find(indices.at(i)));
		}
		return result;
	}
	ValueSeq getAll(
			Ice::Int offset,
			Ice::Int limit,
			bool reverse = false) {
		ValueSeq result;
		int begin = 0;
		IceUtil::Mutex::Lock lock(_mutex);
		limit = (limit < 0) ? _ds.size() : limit;
		if (reverse) {
			for (ReverseIterator it = _ds.rbegin(); it != _ds.rend(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= limit) {
					break;
				}
				result.push_back(*it);
			}
		} else {
			for (Iterator it = _ds.begin(); it != _ds.end(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= limit) {
					break;
				}
				result.push_back(*it);
			}
		}
		return result;
	}

	void add(const ValueT& d) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.insert(d);
	}

	void addAll(const ValueSeq& ds) {
		for (int i = 0; i < ds.size(); ++i) {
			add(ds.at(i));
		};
	}

	void remove(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.erase(index);
	};
	void removeAll(const IndexSeq& indices) {
		for (int i = 0; i < indices.size(); ++i) {
			remove(indices.at(i));
		};
	}
	void clear() {
		_ds.clear();
	};
};

//---------------------------------------------------------------------------
template <class ValueT>
class DataSetI<ValueT, NoIndex, Vector> :
public Ice::Object
{
public:
	typedef vector<ValueT> ValueSeq;
	typedef ValueT IndexT;
	typedef vector<IndexT> IndexSeq;
	typedef ValueSeq ValueVector;
	typedef typename ValueVector::iterator Iterator;
	typedef typename ValueVector::reverse_iterator ReverseIterator;

private:
	ValueVector _ds;
	IceUtil::Mutex _mutex;

public:
	ValueT find(const IndexT& index) {
		Iterator pos = find(_ds.begin(), _ds.end(), index);
		if (pos != _ds.end()) {
			return *pos;
		} else {
			return 0;
		}
	}

	bool contains(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		return find(_ds.begin(), _ds.end(), index) != _ds.end();
	};

	Ice::Int size() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _ds.size();
	};

	ValueT get(const IndexT& index) {
		return find(index);
	}
	ValueSeq getAll(const IndexSeq& indices) {
		ValueSeq result;
		for (int i = 0; i < indices.size(); ++i) {
			result.push_back(find(indices.at(i)));
		}
		return result;
	}
	ValueSeq getAll(
			Ice::Int offset,
			Ice::Int limit,
			bool reverse = false) {
		ValueSeq result;
		int begin = 0;
		IceUtil::Mutex::Lock lock(_mutex);
		limit = (limit < 0) ? _ds.size() : limit;
		if (reverse) {
			for (ReverseIterator it = _ds.rbegin(); it != _ds.rend(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= limit) {
					break;
				}
				result.push_back(*it);
			}
		} else {
			for (Iterator it = _ds.begin(); it != _ds.end(); ++it) {
				if (begin++ < offset) {
					continue;
				}
				if (result.size() >= limit) {
					break;
				}
				result.push_back(*it);
			}
		}
		return result;
	}

	void add(const ValueT& d) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.insert(d);
	}

	void addAll(const ValueSeq& ds) {
		for (int i = 0; i < ds.size(); ++i) {
			add(ds.at(i));
		};
	}

	void remove(const IndexT& index) {
		IceUtil::Mutex::Lock lock(_mutex);
		_ds.erase(remove(_ds.begin(), _ds.end(), index), _ds.end());
	};
	void removeAll(const IndexSeq& indices) {
		for (int i = 0; i < indices.size(); ++i) {
			remove(indices.at(i));
		};
	}

	void clear() {
		_ds.clear();
	};
};

//---------------------------------------------------------------------------
template <class ValueT, class IndexT>
class DataSetI<ValueT, IndexT, Evictor> :
public Ice::Object
{
public:
	typedef vector<ValueT> ValueSeq;
	typedef vector<IndexT> IndexSeq;
	typedef SizedEvictor<IndexT, ValueT> ValueEvictor;

private:
	ValueEvictor _ds;
	string _index;

	IndexT getIndexOf(const ValueT& d) {
		return boost::lexical_cast<IndexT>(d->getProperty(_index));
	}
public:
	DataSetI(const string& index) : _index(index) {_ds.setSize(-1);};

	ValueT find(const IndexT& index) {
		try {
			return _ds.find(index);
		} catch (Ice::ObjectNotExistException& e) {
			return 0;
		}
	}

	bool contains(const IndexT& index) {
		try {
			return find(index) != 0;
		} catch (...) {
			return false;
		}
	};
	Ice::Int size() {
		return _ds.size();
	};

	ValueT get(const IndexT& index) {
		return find(index);
	}
	ValueSeq getAll(const IndexSeq& indices) {
		ValueSeq result;
		for (int i = 0; i < indices.size(); ++i) {
			result.push_back(find(indices.at(i)));
		}
		return result;
	}
	ValueSeq getAll(
			Ice::Int offset,
			Ice::Int limit,
			bool reverse = false) {
		return _ds.list(offset, limit, reverse);
	}

	void add(const ValueT& d) {
		_ds.add(d, getIndexOf(d));
	}

	void addAll(const ValueSeq& ds) {
		for (int i = 0; i < ds.size(); ++i) {
			add(ds.at(i));
		};
	}

	void remove(const IndexT& index) {
		_ds.remove(index);
	};
	void removeAll(const IndexSeq& indices) {
		for (int i = 0; i < indices.size(); ++i) {
			remove(indices.at(i));
		};
	}

	void clear() {
		_ds.clear();
	};

};

typedef DataSetI<DataHandlePtr, int, Evictor> DataHandleSetEI;
typedef IceUtil::Handle<DataHandleSetEI> DataHandleSetEIPtr;

typedef DataSetI<DataHandlePtr, int, Map> DataHandleSetMI;
typedef IceUtil::Handle<DataHandleSetMI> DataHandleSetMIPtr;

typedef DataSetI<DataHandlePtr, NoIndex, Set> DataHandleSetSI;
typedef IceUtil::Handle<DataHandleSetSI> DataHandleSetSIPtr;

class DataResSet : public ResSet
{
public:
	virtual size_t size() {return rows.size();}

	DataHandleSeq rows;
};
typedef IceUtil::Handle<DataResSet> DataResSetPtr;

template <class Parser>
class DataResSetI : public DataResSet
{
public:
#ifndef NEWARCH
	virtual bool buildResult(mysqlpp::ResUse res) {
#else
	virtual bool buildResult(mysqlpp::UseQueryResult res) {
#endif
		try {
			for (mysqlpp::Row row = res.fetch_row(); row; row = res.fetch_row()) {
				rows.push_back(Parser::parse(row));
			}
#ifndef NEWARCH
		} catch (mysqlpp::EndOfResults& e) {
			// ok end of result set
		}
#else
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("DataResSetI mysqlpp::Exception -> " << e.what() );
		} catch (std::exception& e) {
			MCE_WARN("DataResSetI std::exception -> " << e.what() );
		}
#endif
		return true;
	}
};

}; // end of namespace MyUtil

namespace std {
using namespace MyUtil;
template<> struct less<DataHandlePtr> : public binary_function<DataHandlePtr, DataHandlePtr, bool> {
	bool operator()(const DataHandlePtr& x, const DataHandlePtr& y) const {
		return x->less(y);
	}
};

template<> struct greater<DataHandlePtr> : public binary_function<DataHandlePtr, DataHandlePtr, bool> {
	bool operator()(const DataHandlePtr& x, const DataHandlePtr& y) const {
		return x->greater(y);
	}
};

template<> struct equal_to<DataHandlePtr> : public binary_function<DataHandlePtr, DataHandlePtr, bool> {
	bool operator()(const DataHandlePtr& x, const DataHandlePtr& y) const {
		return x->equal(y);
	}
};
}
;

#endif

