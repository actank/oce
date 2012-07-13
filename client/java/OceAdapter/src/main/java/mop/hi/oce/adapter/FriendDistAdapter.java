package mop.hi.oce.adapter;

import java.util.Map;

import mop.hi.svc.model.FriendCount;

public interface FriendDistAdapter {
	public int getFriendDist(int userId, int networkId) throws Exception;

	public Map getFriendDist(int userId) throws Exception;

	public FriendCount[] getFriendDistSeq(int userId, int begin, int limit) throws Exception;
	public FriendCount[] getFriendDistSeq(int userId) throws Exception;
	
	public void moveFriendDist(int userId, int fromId, int toId);
}
