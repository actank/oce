#ifndef __GENERALDBCACHE_ICE__
#define __GENERALDBCACHE_ICE__

#include <Util.ice>

module xce
{
module generaldbcache
{

	class DbFields{
		MyUtil::StrSeq fields;
	};
	
	class DbRow{
		MyUtil::StrSeq values;
	};
	sequence<DbRow> DbRowSeq;
	
	class DbRows{
		DbRowSeq rows;
	};
	
	interface GeneralDbCacheManager{
		DbFields getDbFields();
		DbRows getDbRows(int id);
		void load(int id);
	};
	
};
};
#endif