#ifndef __CONNECTION_QUERY_H__
#define __CONNECTION_QUERY_H__

#include "dbpool/connectionholder.h"

#include <string>
#include <Ice/Ice.h>
#include <boost/lexical_cast.hpp>

namespace com {
namespace xiaonei {
namespace xce {
using namespace std;

class ConnectionQuery;
typedef IceUtil::Handle<ConnectionQuery> DatabasePtr;

class ResSet : public Ice::Object {
public:
//#ifndef NEWARCH
//	virtual bool buildResult(mysqlpp::ResUse res) = 0;
//#else
	virtual bool buildResult(mysqlpp::UseQueryResult res) = 0;
//#endif
	virtual size_t size() = 0;
};
typedef IceUtil::Handle<ResSet> ResSetPtr;

//---------------------------------------------------------------------------

struct ResSetFactory : public IceUtil::Shared {
	virtual ResSetPtr create() = 0;
};
typedef IceUtil::Handle<ResSetFactory> ResSetFactoryPtr;

template<class T> struct GeneralResSetFactory : public ResSetFactory {
	virtual ResSetPtr create() {
		return new T;
	}
};

//---------------------------------------------------------------------------
typedef map<string, string> StringMap;

class ConnectionQuery : public ConnectionHolder {
public:
	ConnectionQuery(const string& name, int wrflag = CDbRServer, const string& pattern = "");
	virtual ~ConnectionQuery();

	ResSetPtr doQuery(const string& sql,
			const ResSetFactoryPtr factory);
	Ice::Long doExecute(const string& sql);

	ResSetPtr doSelect(const string& sql,
			const ResSetFactoryPtr factory);
	ResSetPtr doSelect(const string& table, const StringMap& filter,
			const ResSetFactoryPtr factory);

	void doUpdate(const string& sql);
	void doUpdate(const string& table, const StringMap& filter,
			const StringMap& properties);

	Ice::Long doInsert(const string& sql);

	Ice::Long doInsert(const string& table, const StringMap& properties);
	Ice::Long doInsertIgnore(const string& table, const StringMap& properties);
	Ice::Long doReplace(const string& table, const StringMap& properties);

	void doDelete(const string& sql);
	void doDelete(const string& table, const StringMap& filter);

private:
	Ice::Long doInsert(const string& sqlPrefix, const string& table, const StringMap& properties);
};

}
;
}
;
}
;
#endif

