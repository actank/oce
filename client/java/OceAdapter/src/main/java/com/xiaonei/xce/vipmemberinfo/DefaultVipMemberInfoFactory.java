package com.xiaonei.xce.vipmemberinfo;

public class DefaultVipMemberInfoFactory implements VipMemberInfoFactory<DefaultVipMemberInfo>{
	public DefaultVipMemberInfo create() {
		return new DefaultVipMemberInfo();
	}

}
