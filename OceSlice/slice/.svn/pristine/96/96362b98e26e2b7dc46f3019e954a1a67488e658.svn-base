#ifndef RECENTCONTACT_ICE
#define RECENTCONTACT_ICE

module xce {
module recentcontact {

	enum ContactType {
		XNMESSAGE 
	};

	sequence<ContactType> ContactTypeSeq;

	class ContactInfo {
		int ownerId;
		int guestId;
		int time;
		ContactType type;
	};

	sequence<ContactInfo> ContactInfoSeq;

	interface RecentContactManager {
		ContactInfoSeq get(int ownerId, ContactType type, int begin, int limit);
		void add(ContactInfoSeq contactInfos);//ownerId从contactInfos中可以取到
		void remove(int ownerId, int guestId);
	};

};
};

#endif

