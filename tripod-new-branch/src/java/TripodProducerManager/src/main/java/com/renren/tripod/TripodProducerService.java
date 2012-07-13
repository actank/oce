package com.renren.tripod;

import org.apache.log4j.PropertyConfigurator;

import Ice.Communicator;

public class TripodProducerService implements IceBox.Service {

	private String name_;
	private Communicator ic_;
	private Ice.ObjectAdapter adapter_;

	@Override
	public void start(String name, Communicator ic, String[] arg2) {
    PropertyConfigurator.configure("/data/xce/User/etc/classes/log4j.properties");
		name_ = name;
		ic_ = ic;
		adapter_ = ic_.createObjectAdapter(name_);
		adapter_.add(new TripodProducerManagerI(), ic_.stringToIdentity("M"));
		adapter_.activate();
	}

	@Override
	public void stop() {

	}

}
