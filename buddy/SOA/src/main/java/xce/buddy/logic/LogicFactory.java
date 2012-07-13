package xce.buddy.logic;

import xce.buddy.logic.impl.FriendLogicImpl;

public class LogicFactory {

	private static FriendLogic friendLogic = null;

	public static FriendLogic getFriendLogic() {
		if (friendLogic == null) {
			synchronized (FriendLogic.class) {
				if (friendLogic == null) {
					friendLogic = new FriendLogicImpl();
				}
			}
		}
		return friendLogic;
	}

}
