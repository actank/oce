#include "PreferManager.h"
#include "Node.h"

#include "LogWrapper.h"

using xce::distcache::node::ReadNodePtr;
using xce::distcache::node::ReadNode;
using xce::distcache::node::ItemPtr;
using xce::distcache::PreferManager;
using std::vector;
using std::map;

PreferManager::PreferManager()
{
	srand( time(NULL) );
}

ReadNodePtr PreferManager::choose(const vector<ReadNodePtr>& backends) {
	int bSize = (int)backends.size();

	if( bSize <= 0 )
	{
		bSize = 1;
		MCE_WARN( "PreferManger::choose WARN backends size = 0!" );
	}
	
	return backends.at( rand() % bSize );
}

ReadNodePtr PreferManager::choose(const vector<ReadNodePtr>& backends, const map<ReadNodePtr, vector<ItemPtr> >& categoried) {
	
	int iRandom = rand() % backends.size();
	ReadNodePtr current = ReadNodePtr( backends.at( iRandom ) );
	vector<ReadNodePtr>::const_iterator it = backends.begin();
	for ( vector<ReadNodePtr>::const_iterator it = backends.begin(); it != backends.end(); ++it )
	{
		if ( categoried.find( *it ) != categoried.end()) {
			return *it;
		}
	}
	return current;
}
