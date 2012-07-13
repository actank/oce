package com.xiaonei.xce.test.dbdescriptor;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.channel.Channel;

import com.xiaonei.xce.DbDescriptorPrx;
import com.xiaonei.xce.DbDescriptorPrxHelper;
import com.xiaonei.xce.DbInstance;
import com.xiaonei.xce.XceAdapter;

@RunWith(Parameterized.class)
public class DbDescriptorTestCase {
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { {}, });
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUpBefore() throws Exception {
	}

	@After
	public void tearDownAfter() throws Exception {
	}

	@Test
	public void test() {
		DbDescriptorPrx proxy = DbDescriptorPrxHelper.uncheckedCast(Channel
				.newOceChannel("oce").getCommunicator().stringToProxy(
						"DCS@DbDescriptor"));
		DbInstance instance = proxy.getDbInstance("common");
		System.out.println(instance);
		
		try {
			XceAdapter.getInstance().getReadConnection("common").close();
		} catch (Exception e) {
			// TODO: handle exception
		}
		try {
			XceAdapter.getInstance().getReadConnection("common").close();
		} catch (Exception e) {
			// TODO: handle exception
		}
	}
}
