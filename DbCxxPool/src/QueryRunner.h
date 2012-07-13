#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <string>
#include "Util.h"
#include <mysql++/mysql++.h>
//#include <mysql++/ssqls.h>
#include "ConnectionHolder.h"
#include "Singleton.h"
#include "TaskManager.h"
#include <boost/lexical_cast.hpp>
namespace com {
namespace xiaonei {
namespace xce {

class Statement {
public:
	Statement& operator<<(const mysqlpp::quote_type0& q);

	template<typename T> Statement& operator<<(const T& segment) {
		if (_segments.empty())
			_segments.push_back(std::vector<std::string>());
		_segments.rbegin()->push_back(boost::lexical_cast<string>(segment));
		return *this;
	}

	static Statement buildSelect(const std::string& table,
			const MyUtil::Str2StrMap& filter);
	static Statement buildUpdate(const std::string& table,
			const MyUtil::Str2StrMap& filter,
			const MyUtil::Str2StrMap& properties);
	static Statement buildInsert(const std::string& table,
			const MyUtil::Str2StrMap& properties);
	static Statement buildInsertIgnore(const std::string& table,
			const MyUtil::Str2StrMap& properties);
	static Statement buildReplace(const std::string& table,
			const MyUtil::Str2StrMap& properties);
	static Statement buildDelete(const std::string& table,
			const MyUtil::Str2StrMap& filter);

	void query(mysqlpp::Query& query) const;
	std::vector<std::vector<std::string> > segments() const {
		return _segments;
	}
	void clear() {
		_segments.clear();
	}
	bool empty() {
		return _segments.empty();
	}
private:
	static Statement buildInsert(const string& prefi, const string& table,
			const MyUtil::Str2StrMap& properties);

	inline void build_query(mysqlpp::Query& query, const std::vector<
			std::string>& segs, bool is_quote_first) const;
	std::vector<std::vector<std::string> > _segments;

};

class ResultHandler {
public:
	virtual ~ResultHandler() {
	}
#ifndef NEWARCH
	size_t handle(mysqlpp::ResUse& res) const;
#else
	size_t handle(mysqlpp::UseQueryResult& res) const;
#endif
protected:
	virtual bool handle(mysqlpp::Row& res) const = 0;
	virtual void finish() const {
	}
};

class QueryRunner {
public:
	QueryRunner(const std::string& name, int wrflag = CDbRServer,
			const std::string& pattern = " ");

	size_t query(const Statement& statement, const ResultHandler& handler);

//#ifndef NEWARCH
//	mysqlpp::Result store(const Statement& statement);
//#else
	mysqlpp::StoreQueryResult store(const Statement& statement);
//#endif

	void schedule(const Statement& statement, const std::string& log = "");

//#ifndef NEWARCH
//	mysqlpp::ResNSel execute(const Statement& statement,
//#else
	mysqlpp::SimpleResult execute(const Statement& statement,
//#endif
			const std::string& log = "");

private:
	std::string _name;
	int _wrflag;
	std::string _pattern;
};

class QueryHandler: public MyUtil::Task {
public:
	QueryHandler(const Statement& statement, const string& name, int wrflag =
			CDbRServer, const string& pattern = " ", const string& log = "") :
		MyUtil::Task(MyUtil::TASK_LEVEL_QUERY), _statement(statement), _name(
				name), _wrflag(wrflag), _pattern(pattern), _log(log) {
		;
	}
	virtual void handle();
private:
	Statement _statement;
	string _name;
	int _wrflag;
	string _pattern;
	string _log;
};

}
}
}
#endif /*STATEMENT_H_*/
