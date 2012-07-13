#ifndef __USER_COUNT_API_ICE__
#define __USER_COUNT_API_ICE__

#include <Util.ice>
#include <ObjectCache.ice>
module xce
{
module userapicount
{	
	class UserApiCounter
	{
		MyUtil::IntSeq types;
		MyUtil::IntSeq values;
	};
	
	interface UserApiCountReader
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		void set(int userId, int type, int value);
		int get(int userId, int type);
		UserApiCounter getAll(int userId);
		void setData(MyUtil::ObjectResult data);
	};
	interface UserApiCountWriter
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		void set(int userId, int type, int value);
		void reload(int userId);
	};

	interface UserApiCountManager
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		int get(int userId, int type);
		void set(int userId, int type, int value);
		UserApiCounter getAll(int userId);
		void reload(int userId);
	}; 
};
};

#endif

