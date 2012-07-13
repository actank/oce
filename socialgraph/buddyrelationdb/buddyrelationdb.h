#ifndef BUDDYRELATIONDB_H_
#define BUDDYRELATIONDB_H_

#include "socialgraph/socialgraphutil/calculator/calculator.h"

namespace xce {
namespace socialgraph {

class BuddyRelationDb : public Calculator {
public:
	BuddyRelationDb();
	virtual ~BuddyRelationDb();
	
	void Detail();
};

}
}

#endif
