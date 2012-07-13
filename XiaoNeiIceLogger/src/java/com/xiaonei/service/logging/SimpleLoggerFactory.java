package com.xiaonei.service.logging;

import Ice.Communicator;
import Ice.LocalObjectImpl;
import Ice.Logger;

public class SimpleLoggerFactory extends LocalObjectImpl implements Ice.LoggerFactory {

	public Logger create(Communicator ic, String[] args) {
		return new SimpleLogger();
	}

}
