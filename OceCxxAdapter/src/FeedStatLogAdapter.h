/*
 * FeedStatLogAdapter.h
 *
 *  Created on: Nov 1, 2011
 *      Author: qun.liu
 */

#ifndef FEEDSTATLOGADAPTER_H_
#define FEEDSTATLOGADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedStatLog.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedStatLogAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,
				FeedStatLogAdapter> {
public:
	FeedStatLogAdapter() {
	}
        
        void AddLog(MyUtil::StrSeq category, string log);
protected:
	virtual string name() {
		return "FeedStatLog0";
	}

	virtual string endpoints() {
		return "@FeedStatLog0";
	}

        virtual size_t cluster() {
                return 0;
        } 

        FeedStatLogPrx getManager();
	FeedStatLogPrx getManagerOneway();

};

};
};
#endif /* FEEDSTATLOGADAPTER_H_ */
