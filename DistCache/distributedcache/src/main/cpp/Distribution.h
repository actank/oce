#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <boost/shared_ptr.hpp>

namespace xce {
namespace distcache {

class Distribution;

typedef boost::shared_ptr<Distribution> DistributionPtr;

class Distribution {
public:
	static DistributionPtr instance(int blockSize);
	int get(long id);
private:
	int blockSize_;

	Distribution(int blockSize);
};

}
}

#endif
