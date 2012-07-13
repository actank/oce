package com.renren.xce.util;

public class StdoutLoggerPluginFactory implements Ice.PluginFactory {	    
	public Ice.Plugin create(Ice.Communicator communicator, String name, String[] args) {
	    return new Ice.LoggerPlugin(communicator, new StdoutLogger());
	}
}
