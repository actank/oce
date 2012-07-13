package com.xiaonei.xce.vipmemberinfo;

public interface VipMemberInfoFactory <T extends VipMemberInfo>{
	public T create();
}
