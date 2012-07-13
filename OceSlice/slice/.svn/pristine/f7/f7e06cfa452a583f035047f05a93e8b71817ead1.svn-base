#ifndef FOOTPRINT_ICE_NEW
#define FOOTPRINT_ICE_NEW

#include <Util.ice>

module xce {
module fptn {

	class FootprintTiny {	// 只包含guest和访问时间的脚印的tiny结构
		int guest;
		int timestamp;
	};
	sequence<FootprintTiny> FootprintSeq;

	class Footprint {
        	int host;
		FootprintTiny tinyinfo;
        };
	sequence<Footprint> FootprintFullSeq;

	class FootprintInfo {
		FootprintSeq fpts;
		int visitcount;
	};

	class FootprintManager	{
		void visitHomepageOneway(Footprint fpt);
		FootprintInfo visitHomepage(Footprint fpt, int begin, int limit);
		int getSize(int uid);		
		FootprintInfo getAll(int uid,int begin,int limit);

		int getUserVisitCount(int userId);
		void setUserVisitCount(int userId, int count);
		void incUserVisitCount(int userId);
		void incUserVisitCountBatch(MyUtil::IntSeq userIds, int count);
	};

    	class FootprintFilterManager    {
	        FootprintInfo visitHomepage(Footprint fpt);
		void visitHomepageOneway(Footprint fpt);
		void visitHomepageBatch(FootprintFullSeq fpts);
		void contral(MyUtil::Str2IntMap cmd);
    	};
};
};

/* module mop {
module hi {
module svc {
module fptn {

	class Footprint {
        	int host;
	   	int guest;
        };	
	sequence<int> FootprintSeq;

	class FootprintInfo {
		FootprintSeq fpts;
		int visitcount;
	};

	class FootprintSet {
		idempotent int getSize();
		idempotent FootprintSeq getAll(int begin,int limit);
		bool add(Footprint fpt);
		void merge(FootprintSet fpt);					
		FootprintSeq footprints;
	};

	class UserVisitCount {
		int getCount();
		void setCount(int count);
		void incCount(int step);
		int count;
	};


	class FootprintManager	{
		FootprintInfo visitHomepage(Footprint fpt, int begin, int limit);
		int getSize(int uid);		
		FootprintInfo getAll(int uid,int begin,int limit);

		int getUserVisitCount(int userId) throws mop::hi::svc::model::UserException;
		void setUserVisitCount(int userId, int count);
		void incUserVisitCount(int userId);
		void incUserVisitCountBatch(MyUtil::IntSeq userIds, int count);
	};
};
};
};
}; */

#endif

