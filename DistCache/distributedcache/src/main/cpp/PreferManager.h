#ifndef PREFERMANAGER_H
#define PREFERMANAGER_H

#include "Node.h"
#include <vector>
#include <map>

namespace xce {
namespace distcache {

class PreferManager {

	public:

		PreferManager();
	
	public:
	xce::distcache::node::ReadNodePtr choose(const std::vector<xce::distcache::node::ReadNodePtr>& backends);
	xce::distcache::node::ReadNodePtr choose(const std::vector<xce::distcache::node::ReadNodePtr>& backends, const std::map<xce::distcache::node::ReadNodePtr, std::vector<xce::distcache::node::ItemPtr> >& categoried);
};

}
}

#endif
