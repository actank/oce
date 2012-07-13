package mop.hi.oce.domain.model;

public abstract class FriendShare {
	private int _userId;

	public void setUserId(int id) {
		this._userId = id;
	}

	public int getUserId() {
		return _userId;
	}
}
