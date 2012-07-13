package com.xiaonei.xce.fof.service;

import org.apache.log4j.Logger;

import com.linkedin.norbert.network.Serializer;

public abstract class AbstractNorbertCoreService<Req extends AbstractNorbertRequest, Res extends AbstractNorbertResult> {

	private final static Logger logger = Logger
			.getLogger(AbstractNorbertCoreService.class);

	public abstract Res execute(Req senseiReq);

	public abstract Res getEmptyResultInstance(Throwable error);

	public abstract Serializer<Req, Res> getSerializer();
}
