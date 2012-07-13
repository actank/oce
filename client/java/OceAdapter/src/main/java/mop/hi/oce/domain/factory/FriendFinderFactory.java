package mop.hi.oce.domain.factory;

import mop.hi.oce.domain.model.FriendCommon;
import mop.hi.oce.domain.model.FriendShare;

public interface FriendFinderFactory {

	public FriendCommon createFriendCommon();

	public FriendShare createFriendShare();
}
