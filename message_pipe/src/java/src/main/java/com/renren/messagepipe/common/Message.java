package com.renren.messagepipe.common;

public interface Message {

	public byte[] toBytes();
	public void fromBytes(byte[] bytes);
	
}
