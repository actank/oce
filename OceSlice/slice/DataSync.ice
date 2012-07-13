#ifndef __DATA_SYNC_H__
#define __DATA_SYNC_H__

#include <Util.ice>

module xce
{
module datasync
{
	class ConfigData
	{
		string identifier;

		string prop;

	};

	sequence<ConfigData> ConfigDataSeq;

	class ConfigManager
	{
		ConfigDataSeq get();

		bool add( ConfigDataSeq configs );
		
		bool remove( MyUtil::StrSeq identifiers );
		
		bool reload();
	};


	class CrossCommunicator
	{
		bool send( MyUtil::Str2StrMapSeq data );

		bool receive( string msg );
		
		bool changeConfigInfo( ConfigDataSeq configs );
	};

	class ConflictData
	{
		MyUtil::Str2StrMap oldInfo;
		
		MyUtil::Str2StrMap newInfo;
	};
	
	sequence<ConflictData> ConflictDataSeq;

	class ConflictManager
	{
		void add( ConflictDataSeq data );
	};

	class InDataManager
	{
		void add ( MyUtil::Str2StrMapSeq data );
	};

	class OutDataManager
	{
		void add ( MyUtil::Str2StrMapSeq data );
	};

	class SyncManager
	{
		void add ( MyUtil::Str2StrMapSeq data );
	};
};
};

#endif
