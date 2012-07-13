package com.xiaonei.importer.service;

import Ice.Communicator;
import Ice.LocalObjectImpl;
import Ice.ObjectAdapter;
import IceBox.Service;

public class ImporterService extends LocalObjectImpl implements Service {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ImporterService.class);

	private ObjectAdapter _adapter;

	public void start(String name, Communicator ic, String[] args) {
		_logger.debug("Service starting.");
		_adapter = ic.createObjectAdapter(name);
		_logger.debug("ObjectAdapter created.");
		Ice.Object obj = new ImporterManagerI();
		_logger.debug("ImporterManager created.");
		_adapter.add(obj, Ice.Util.stringToIdentity("M"));
		_logger.debug("ImporterManager registered");
		_adapter.activate();
		_logger.debug("Service started.");
	}

	public void stop() {
		_logger.debug("Stoping service.");
		_adapter.deactivate();
		_logger.debug("Service stoped.");
	}

}
