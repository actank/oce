#include "UgcDBhelper.h"

using namespace xce::dbhelper;
using namespace MyUtil;
using namespace item;

//--------------------------------------------------------------------------------------------

bool PhotoDataReadHelper::handle(mysqlpp::Row& row) const {
    //int state = 0;
    if (IsMultiPhoto(row["state"])) {
        //state = MULTI_PHOTO;

        UGCItem ugcUnit((int)row["owner"],(long)row["album"], (int)row["commentcount"], (int)row["viewcount"], MULTI_PHOTO);
	//	cout << row["time"] << "	" << row["owner"] << "	" << row["album"] << "	" 
	//		<< row["commentcount"] << "	" << row["viewcount"] << endl;
        _ugcList.push_back(ugcUnit);
    } else {
        //state = SINGLE_PHOTO;

        UGCItem ugcUnit((int)row["owner"], (long)row["id"], (int)row["commentcount"], (int)row["viewcount"], SINGLE_PHOTO);
	//	cout << row["time"] << "	" << row["owner"] << "	" << row["id"] << "	" 
	//		<< row["commentcount"] << "	" << row["viewcount"] << endl;
        _ugcList.push_back(ugcUnit);
    }

    return true;
}

bool PhotoDataReadHelper::IsMultiPhoto(int state) const {
    if ((state & STATE_MULTI_PHOTO) == STATE_MULTI_PHOTO) {
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------

bool BlogDataReadHelper::handle(mysqlpp::Row& row) const {
    UGCItem ugcUnit((int)row["owner"], (long)row["id"], (int)row["comment_count"], (int)row["view_count"], BLOG);
    _ugcList.push_back(ugcUnit);
	//cout << row["time"] << "	" << row["id"] << "	" << row["owner"] << endl;

    return true;
}

//--------------------------------------------------------------------------------------------

