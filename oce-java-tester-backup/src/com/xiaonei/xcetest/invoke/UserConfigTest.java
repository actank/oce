package com.xiaonei.xcetest.invoke;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.UserConfigInfoAdapterImpl;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.svc.model.UserException;

public class UserConfigTest extends TestCase {

	UserConfigInfoAdapterImpl userConfigImpl = new UserConfigInfoAdapterImpl();

	public void testgetStatusConfig() throws UserException, Exception {
		int id = 202911262;
		UserConfigInfo ret = userConfigImpl.getUserConfig(id);
		System.out.println(ret.getStatusConfig());
	}

	public void testsetStatusConfig() throws UserException, Exception {
		int userid = 200353733;
		UserConfigInfo ret = userConfigImpl.getUserConfig(userid);
		System.out.println("Original => " + ret.getStatusConfig());
		ret.setStatusConfig(0);
		userConfigImpl.setUserConfig(userid, ret);
	}

}
