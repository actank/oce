#ifndef __UTILSLICE_TASKMANAGERAPI__
#define __UTILSLICE_TASKMANAGERAPI__

#include <Util.ice>

module xce {
module svcapi {
	interface TaskManagerAPI {
		idempotent void config(int level, int min, int max, int stack);
		idempotent int clear(int level);
		["cpp:const"] idempotent int size(int level);
		["cpp:const"] idempotent MyUtil::Int2IntMap getLevel2Size();
	};
};
};
#endif
