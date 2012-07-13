#include <ObjectCache.ice>
#include <Ice/BuiltinSequences.ice>

module xce {
module socialgraph {
module friendfindernewcomer {

	struct NewComerDbData{
		int userId;
		int time;
	};
	sequence<NewComerDbData> NewComerDbDataSeq;
	dictionary<int,NewComerDbDataSeq> NewComerDbDataMap;

	struct NewComerMcData{
		int userId;
		int time;
		int type;
		string schoolName;
	};
	sequence<NewComerMcData> NewComerMcDataSeq;
	//dictionary<int,NewComerMcData> NewComerMcDataMap;

	class NewComerCacheData{
		NewComerDbDataMap newsDbMap;
	};

        struct RmInfo{
                int hostId;
                int guestId;
                bool hit;
                bool found;
		MyUtil::IntSeq types;
        };

	/*
	interface FriendFinderNewComerWorker {

		void apply(int applicant, int acceptor);
		void accept(int acceptor, int applicant);
		void deny(int hostId, int guestId);
		void ignore(int userId, MyUtil::IntSeq ids);

		void addNewComer(int userId);
		void addNewComerWithTime(int userId,int time);

		void updateInfo(int userId);
	};
	*/

};
};
};

