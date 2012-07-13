package mop.hi.oce.domain.model;


public abstract class FriendCommon {

	private int _userId;
	private int _weight;
	private int[] _shares;

	public void setUserId(int id) {
		_userId = id;
	}

	public void setWeight(int weight) {
		this._weight = weight;
	}

	public void setShares(int[] shares) {
		this._shares = shares;
	}
	
	public int getUserId() {
		return _userId;
	}

	public int getWeight() {
		return _weight;
	}

	public int[] getShares() {
		return _shares;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		StringBuffer s = new StringBuffer();
		s.append("Uid: ");
		s.append(getUserId());
		s.append("   ");
		s.append("Num:");
		s.append(getWeight());
		return s.toString();
	}

}
