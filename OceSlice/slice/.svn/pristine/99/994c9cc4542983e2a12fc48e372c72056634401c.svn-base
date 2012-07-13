#ifndef FOOTPRINT_ICE
#define FOOTPRINT_ICE
#include <Util.ice>
#include <FootprintFilter.ice>

module mop {
module hi {
module svc {
module fpt {

	exception FootprintException extends MyUtil::MceException {
		long id;
		long host;
		int guest;
	};

	class FootprintSet {
		idempotent int getSize() throws FootprintException;
		idempotent FootprintSeq getAll(int begin,int limit) throws FootprintException;
		bool add(Footprint fpt);
		void merge(FootprintSet fpt);
		FootprintSeq footprints;
	};

	class FootprintManager	{
		void visitHomepage(Footprint fpt);
		int getSize(long host) throws FootprintException;
		FootprintSeq getAll(long host,int begin,int limit) throws FootprintException;
	};
};
};
};
};
#endif

