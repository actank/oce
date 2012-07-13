package com.xiaonei.xce.fof.service;

import org.apache.log4j.Logger;

import com.linkedin.norbert.javacompat.network.RequestHandler;

public final class NorbertCoreServiceMessageHandler<REQUEST extends AbstractNorbertRequest, RESULT extends AbstractNorbertResult>
		implements RequestHandler<REQUEST, RESULT> {
	private static final Logger logger = Logger
			.getLogger(NorbertCoreServiceMessageHandler.class);
	private final AbstractNorbertCoreService<REQUEST, RESULT> _svc;

	public NorbertCoreServiceMessageHandler(
			AbstractNorbertCoreService<REQUEST, RESULT> svc) {
		_svc = svc;
	}

	@Override
	public RESULT handleRequest(final REQUEST request) throws Exception {
		return _svc.execute(request);
	}
}
