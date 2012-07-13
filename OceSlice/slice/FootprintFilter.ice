#ifndef FOOTPRINT_FILTER_ICE
#define FOOTPRINT_FILTER_ICE

#include <Util.ice>
#include <FootprintNew.ice>

module mop {
module hi {
module svc {
module fpt {
	
    class Footprint
    {
        long id;
        long host;
        int guest;
        string guestName;
        string guestHead;
        long time;
    };
    sequence<Footprint> FootprintSeq;


    class FootprintFilterManager
    {
        void visitHomepage(Footprint fpt);
	void contral(MyUtil::Str2IntMap cmd);
    };
};


};
};
};

#endif
