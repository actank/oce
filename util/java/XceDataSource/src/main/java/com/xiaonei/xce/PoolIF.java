package com.xiaonei.xce;

public interface PoolIF<T> {
	public T borrowObject();

	public void returnObject(T obj);

}
