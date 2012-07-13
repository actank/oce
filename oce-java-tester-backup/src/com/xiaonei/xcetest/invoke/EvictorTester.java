package com.xiaonei.xcetest.invoke;

import xce.util.cache.TimedEvictor;
import junit.framework.TestCase;


public class EvictorTester extends TestCase {

	protected void setUp() throws Exception {
		_evictor.add("BBC", "BBC");
		_evictor.add("ABC", "ABC");
		_evictor.add("CCC", "CCC");
	}

	private TimedEvictor _evictor = new TimedEvictor(null, 3);
	
	public void test1(){
		for(int i=0;i<15;i++){
			System.out.println(_evictor.locate("BBC"));
			System.out.println(_evictor.locate("ABC"));
			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		System.out.println(_evictor.locate("ABC"));
	}
}
