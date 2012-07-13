#ifndef __USER_BASE_READER_ADAPTER_H__
#define __USER_BASE_READER_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userbasereader {

using namespace ::xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserBaseFullView : public Ice::Object {
public:
	UserBaseFullView(const UserBaseViewDataNPtr& data) :
		id_(data->upd->uID),
		status_(data->upd->status), safestatus_(data->upd->safeStatus), account_(data->upd->account), passwordmd5_(data->upd->passwordmd5), domain_(data->upd->domain),
		name_(data->und->name), nickname_(data->und->nickname),
		auth_(data->urd->auth), rights_(data->urd->rights),
		state_(data->usd->state), star_(data->usd->star), level_(data->usd->level),
		logincount_(data->ulcd->logincount),
		gender_(data->userbasic->gender), birthday_(data->userbasic->birthDay), birthmonth_(data->userbasic->birthMonth), birthyear_(data->userbasic->birthYear),islunarbirth_(data->userbasic->isLunarBirth),homeprovince_(data->userbasic->homeProvince), homecity_(data->userbasic->homeCity), citycode_(data->userbasic->cityCode), astrology_(data->userbasic->astrology),univ_(data->userbasic->univ), stage_(data->userbasic->stage), doing_(data->userbasic->doing), doingtime_(data->userbasic->doingTime),
		largeurl_(data->userurl->largeUrl), tinyurl_(data->userurl->tinyUrl), headurl_(data->userurl->headUrl), mainurl_(data->userurl->mainUrl) {}

	int id() const {return id_;}
	int status() const {return status_;}
	int safestatus() const {return safestatus_;}
	string account() const {return account_;}
	string passwordmd5() const {return passwordmd5_;}
	string domain() const {return domain_;}

	string name() const {return name_;}
	string nickname() const {return nickname_;}

	int auth() const {return auth_;}
	int rights() const {return rights_;}

	int state() const {return state_;}
	int star() const {return star_;}
	int level() const {return level_;}

	int logincount() const {return logincount_;}

	string gender() const {return gender_;}
	int birthday() const {return birthday_;}
	int birthmonth() const {return birthmonth_;}
	int birthyear() const {return birthyear_;}
	bool islunarbirth() const {return islunarbirth_;}
	string homeprovince() const {return homeprovince_;}
	string homecity() const {return homecity_;}
	string citycode() const {return citycode_;}
	string astrology() const {return astrology_;}

	int univ() const {return univ_;}
	int stage() const {return stage_;}

	string doing() const {return doing_;}
	int doingtime() const {return doingtime_;}

	string largeurl() const {return largeurl_;}
	string tinyurl() const {return tinyurl_;}
	string headurl() const {return headurl_;}
	string mainurl() const {return mainurl_;}
	
private:
	int id_;
	int status_;
	int safestatus_;
	string account_;
	string passwordmd5_;
	string domain_;

	string name_;
	string nickname_;

	int auth_;
	int rights_;

	int state_;
	int star_;
	int level_;

	int logincount_;

	string gender_;
	int birthday_;
	int birthmonth_;
	int birthyear_;
	bool islunarbirth_;
	string homeprovince_;
	string homecity_;
	string citycode_;
	string astrology_;

	int univ_;
	int stage_;

	string doing_;
	int doingtime_;

	string largeurl_;
	string tinyurl_;
	string headurl_;
	string mainurl_;

	
};

typedef IceUtil::Handle<UserBaseFullView> UserBaseFullViewPtr;

class UserBaseCommonView : public Ice::Object {
public:
	UserBaseCommonView(const UserCommonViewDataPtr& data) :
		id_(data->userpassport->uID),
		status_(data->userpassport->status), safestatus_(data->userpassport->safeStatus), account_(data->userpassport->account), passwordmd5_(data->userpassport->passwordmd5), domain_(data->userpassport->domain),
		name_(data->username->name), nickname_(data->username->nickname),
		auth_(data->userright->auth), rights_(data->userright->rights),
		state_(data->userstate->state), star_(data->userstate->star), level_(data->userstate->level),
		logincount_(data->userlogincount->logincount),
		gender_(data->gender), univ_(data->univ), stage_(data->stage),
		largeurl_(data->userurl->largeUrl), tinyurl_(data->userurl->tinyUrl), headurl_(data->userurl->headUrl), mainurl_(data->userurl->mainUrl) {}

	int id() const {return id_;}
	int status() const {return status_;}
	int safestatus() const {return safestatus_;}
	string account() const {return account_;}
	string passwordmd5() const {return passwordmd5_;}
	string domain() const {return domain_;}

	string name() const {return name_;}
	string nickname() const {return nickname_;}

	int auth() const {return auth_;}
	int rights() const {return rights_;}

	int state() const {return state_;}
	int star() const {return star_;}
	int level() const {return level_;}

	int logincount() const {return logincount_;}

	string gender() const {return gender_;}
	int univ() const {return univ_;}
	int stage() const {return stage_;}

	string largeurl() const {return largeurl_;}
	string tinyurl() const {return tinyurl_;}
	string headurl() const {return headurl_;}
	string mainurl() const {return mainurl_;}
private:
	int id_;
	int status_;
	int safestatus_;
	string account_;
	string passwordmd5_;
	string domain_;

	string name_;
	string nickname_;

	int auth_;
	int rights_;

	int state_;
	int star_;
	int level_;

	int logincount_;

	string gender_;
	int univ_;
	int stage_;

	string largeurl_;
	string tinyurl_;
	string headurl_;
	string mainurl_;
};

typedef IceUtil::Handle<UserBaseCommonView> UserBaseCommonViewPtr;

class UserBaseReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserBaseViewReaderPrx>,
	public MyUtil::Singleton<UserBaseReaderAdapter>
{
public:
	UserBaseReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserBaseViewReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel,true,"UBVM"){
	}
	UserBaseFullViewPtr getUserBaseFullView(int id, const Ice::Context& ctx = Ice::Context());
	UserBaseCommonViewPtr getUserBaseCommonView(int id, const Ice::Context& ctx = Ice::Context());
		

	UserBaseViewReaderPrx getUserBaseViewReader(int userId);
	UserBaseViewReaderPrx getUserBaseViewReaderOneway(int userId);
};

}
}
}

#endif

