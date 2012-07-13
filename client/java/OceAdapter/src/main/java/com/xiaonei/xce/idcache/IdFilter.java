package com.xiaonei.xce.idcache;

public class IdFilter {
	final int minFilterId = 2000000000;
	final int maxFilterId = 2100000000;
	
	public boolean isMaybeVirtualId(int id){
		return (id > minFilterId && id < maxFilterId);
	}

}
