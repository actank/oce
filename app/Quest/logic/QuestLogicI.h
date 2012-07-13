/* -------------------------------------
 * QuestLogicI.h
 *
 *  Created on: 2009-12-24
 *      Author: han.zhang
 ------------------------------------- */

#ifndef __QUEST_LOGIC_H__
#define __QUEST_LOGIC_H__

#include <Quest.h>
#include "app/Quest/share/QuestUtil.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace quest {

class QuestLogicI : virtual public QuestLogic,
		    public MyUtil::Singleton<QuestLogicI> {
public:
	virtual void addTip(const string& title, const string& content, const string& url, int showCount, const Ice::Current& crt=Ice::Current());	

	virtual void removeTip(int tipid, const Ice::Current& crt=Ice::Current());

	virtual void modifyTip(int tipid, const string& title, const string& content, const string& url, const Ice::Current& crt=Ice::Current());

	virtual void setTipFlow(const TipFlowPtr& flow, const Ice::Current& crt=Ice::Current());

	virtual void renew(const Ice::Current& crt = Ice::Current());

private:
	friend class MyUtil::Singleton<QuestLogicI>;
	QuestLogicI() {
	};

};



};
};

#endif
