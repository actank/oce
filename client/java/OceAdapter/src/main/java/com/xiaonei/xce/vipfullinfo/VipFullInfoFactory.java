package com.xiaonei.xce.vipfullinfo;

public interface VipFullInfoFactory <T extends VipFullInfo<?>>{
	public T create();
}
