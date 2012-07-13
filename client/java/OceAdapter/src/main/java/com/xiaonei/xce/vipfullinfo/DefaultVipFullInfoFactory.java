package com.xiaonei.xce.vipfullinfo;

public class DefaultVipFullInfoFactory implements VipFullInfoFactory<DefaultVipFullInfo>{
	public DefaultVipFullInfo create() {
		return new DefaultVipFullInfo();
	}

}
