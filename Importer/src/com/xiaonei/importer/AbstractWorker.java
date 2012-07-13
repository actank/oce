package com.xiaonei.importer;

import java.util.UUID;

public abstract class AbstractWorker implements Runnable {

	protected UUID _requestId;
	protected WorkerCallback _callback;

	protected AbstractWorker(UUID requestId, WorkerCallback callback) {
		_requestId = requestId;
		_callback = callback;
	}
}
