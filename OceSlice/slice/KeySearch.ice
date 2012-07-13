#ifndef __KEY_SEARCH_ICE__
#define __KEY_SEARCH_ICE__

#include <Util.ice>
module demo{

	interface KeySearchManager{
	int size();
        int lookup(string Key);
	bool insert(string Key,int Value);
	bool remove(string Key);
	};

};
#endif
