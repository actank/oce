package com.xiaonei.xce.pool.tester;

//import junit.framework.TestCase;
import com.xiaonei.xce.log.Oce;

public class regexTester {// extends TestCase {
	public void test1() {
		String a = "gossip_11";
		String b = "gossip_[0-9]{1}|gossip_1[0-9]{1}|gossip_2[0-4]{1}";
		Oce.getLogger().info(a.matches(b));

	}

	public void testEnv() {
		Oce.getLogger().info(System.getProperty("XCE_USE_DBCP"));
	}
}
