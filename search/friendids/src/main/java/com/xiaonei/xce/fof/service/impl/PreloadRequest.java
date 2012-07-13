package com.xiaonei.xce.fof.service.impl;

import java.util.Map;

import com.xiaonei.xce.fof.service.AbstractNorbertRequest;

public class PreloadRequest implements AbstractNorbertRequest {

	private Map<Integer,Map<Integer, int[]>> update;

	public Map<Integer, Map<Integer, int[]>> getUpdate() {
		return update;
	}

	public void setUpdate(Map<Integer, Map<Integer, int[]>> update) {
		this.update = update;
	}

}
