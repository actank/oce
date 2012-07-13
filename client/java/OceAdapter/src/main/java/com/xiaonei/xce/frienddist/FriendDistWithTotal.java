package com.xiaonei.xce.frienddist;

import java.util.ArrayList;
import java.util.List;

public abstract class FriendDistWithTotal<T extends FriendDistCount> {

	private List<T> _friendDist;
	private int _totalCount;

	public List<T> getFriendDist() {
		return _friendDist;
	}

	public int getTotalCount() {
		return _totalCount;
	}

	protected void setBase(mop.hi.svc.model.FriendDistWithTotal ffc,
			FriendDistFactory<T, ?> factory) {
		_totalCount = ffc.totalCount;
		List<T> result = new ArrayList<T>();
		for (mop.hi.svc.model.FriendCount fc : ffc.networks) {
			T o = factory.createFriendDistCount();
			o.setBase(fc.networkId, fc.count);
			result.add(o);
		}
		_friendDist = result;
	}

	@Override
	public String toString() {
		StringBuffer res = new StringBuffer();
		res.append("[FriendDistWithTotal: totalCount=").append(_totalCount)
				.append(";\n");
		for (FriendDistCount o : _friendDist) {
			res.append("\t").append(o.toString()).append(";\n");
		}
		res.append("]");
		return res.toString();
	}
}
