#ifndef _PVCOUNTDEMO_ICE
#define _PVCOUNTDEMO_ICE

module Demo {

	interface PVCountDemo {
		idempotent void add(int userId, int pvCount); //add a new user

		idempotent void remove(int userId); //delete a user by id

		idempotent void update(int userId, int pvCount); // update the user's pvCount
		
		int getPVCountByUserId(int id);	// get this user
	};
};

#endif
