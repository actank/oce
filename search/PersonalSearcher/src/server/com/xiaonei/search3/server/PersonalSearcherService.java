package com.xiaonei.search3.server;

import com.xiaonei.search3.server.PersonalSearcherI;
import xce.util.service.ServiceI;

public class PersonalSearcherService extends ServiceI {

	@Override
	public void finishup() {
	}

	@Override
	public void initialize() {
		_adapter.add(new PersonalSearcherI(), Ice.Util.stringToIdentity("M"));
	}
}
