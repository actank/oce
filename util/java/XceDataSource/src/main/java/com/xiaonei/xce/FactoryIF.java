package com.xiaonei.xce;

public interface FactoryIF<T> {

	public T makeObject();

	public boolean validObject(T obj);

	public void destoryObject(T obj);
}
