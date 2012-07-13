package com.xiaonei.xce.fof.service.impl;

import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.linkedin.norbert.network.JavaSerializer;
import com.linkedin.norbert.network.Serializer;
import com.xiaonei.xce.fof.service.AbstractNorbertCoreService;

public class PreloadFriendOfFriendServiceImpl extends
		AbstractNorbertCoreService<PreloadRequest, PreloadResult> {
	public static final Serializer<PreloadRequest, PreloadResult> SERIALIZER = JavaSerializer
			.apply("PreloadRequest", PreloadRequest.class, PreloadResult.class);

	private static final Logger logger = Logger
			.getLogger(PreloadFriendOfFriendServiceImpl.class);

	private FriendCore _core;

	public PreloadFriendOfFriendServiceImpl(FriendCore core) {
		_core = core;
	}

	@Override
	public PreloadResult execute(PreloadRequest req) {
		for (Entry<Integer, Map<Integer, int[]>> entry : req.getUpdate()
				.entrySet()) {
			_core.setFriendIds(entry.getKey(), entry.getValue());
		}
		return new PreloadResult();
	}

	@Override
	public PreloadResult getEmptyResultInstance(Throwable error) {
		return new PreloadResult();
	}

	@Override
	public Serializer<PreloadRequest, PreloadResult> getSerializer() {
		return SERIALIZER;
	}

}
