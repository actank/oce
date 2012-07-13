#include "Distribution.h"

using xce::distcache::DistributionPtr;
using xce::distcache::Distribution;

DistributionPtr Distribution::instance(int blockSize) {
	return DistributionPtr(new Distribution(blockSize));
}

Distribution::Distribution(int blockSize) {
	if (blockSize < 1) {
		blockSize = 1;
	}
	blockSize_ = blockSize;
}

int Distribution::get(long id) {
	return (int)(abs(id) % blockSize_);
}
