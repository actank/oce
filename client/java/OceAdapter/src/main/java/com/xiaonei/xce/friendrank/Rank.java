package com.xiaonei.xce.friendrank;

public abstract class Rank {
	private int _userId;
	private int _weight;
	
	public void setRank(int userId, int weight){
		_userId = userId;
		_weight = weight;
	}
	
	public int getUserId(){
		return _userId;
	}
	
	public int getWeight(){
		return _weight;
	}

}

