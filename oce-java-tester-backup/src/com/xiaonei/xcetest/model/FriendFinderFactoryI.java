package com.xiaonei.xcetest.model;

import mop.hi.oce.domain.factory.FriendFinderFactory;
import mop.hi.oce.domain.model.FriendCommon;
import mop.hi.oce.domain.model.FriendShare;

class OFriendCommon extends FriendCommon {

}

class OFriendShare extends FriendShare {

}

public class FriendFinderFactoryI implements FriendFinderFactory {
	public FriendCommon createFriendCommon() {
		return new OFriendCommon();
	}

	public FriendShare createFriendShare() {
		return new OFriendShare();
	}
}