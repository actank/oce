#include "UserDangerAdapter.h"

using xce::userdanger::UserDangerAdapter;

UserDangerAdapter::UserDangerAdapter() : MyUtil::ReplicatedClusterAdapterI <UserDangerManagerPrx>("ControllerUserDanger", 120, 100) {
}

bool UserDangerAdapter::isNotFeed(int userid) {
  try {
    return getProxy(0)->isType(userid, NOTFEED);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isNotFeed Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isNotAcceptBuddyApplication(int userid) {
  try {
    return getProxy(0)->isType(userid, NOTACCEPTBUDDYAPPLICATION);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isNotAcceptBuddyApplication Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isOnlySeenByFriend(int userid) {
  try {
    return getProxy(0)->isType(userid, ONLYSEENBYFRIEND);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isOnlySeenByFriend Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isOnlySeenBySelf(int userid) {
  try {
    return getProxy(0)->isType(userid, ONLYSEENBYSELF);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isOnlySeenBySelf Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isUGCOnlySeenBySelf(int userid) {
  try {
    return getProxy(0)->isType(userid, UGCONLYSEENBYSELF);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isUGCOnlySeenBySelf Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isUGCAllCheck(int userid) {
  try {
    return getProxy(0)->isType(userid, UGCALLCHECK);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isUGCAllCheck Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isSearchShielding(int userid) {
  try {
    return getProxy(0)->isType(userid, SEARCHSHIELDING);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isSearchShielding Ice::Exception " << e);
  }
  return false;
}

map<int, bool> UserDangerAdapter::isSearchShielding(const std::vector<int>& ids) {
  map<int, bool> result;
  for (std::vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
    result[*i] = false;
  }
  try {
    return getProxy(0)->isTypes(ids, SEARCHSHIELDING);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isSearchShielding Ice::Exception " << e);
  }
  return result;
}

bool UserDangerAdapter::isMailSubscriptionShielding(int userid) {
  try {
    return getProxy(0)->isType(userid, MAILSUBSCRIPTIONSHIELDING);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isMailSubscriptionShielding Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isUgcAuditFirst(int userid) {
  try {
    return getProxy(0)->isType(userid, UGCAUDITFIRST);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isUgcAuditFirst Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isMiniSiteHide(int userid) {
  try {
    return getProxy(0)->isType(userid, MINISITEHIDE);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isMiniSiteHide Ice::Exception " << e);
  }
  return false;
}

bool UserDangerAdapter::isMiniGroupHide(int userid) {
  try {
    return getProxy(0)->isType(userid, MINIGROUPHIDE);
  } catch (const Ice::Exception& e) {
    MCE_WARN("UserDangerAdapter::isMiniGroupHide Ice::Exception " << e);
  }
  return false;
}

