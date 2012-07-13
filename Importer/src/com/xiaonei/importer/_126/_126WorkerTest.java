package com.xiaonei.importer._126;

import junit.framework.TestCase;

public class _126WorkerTest extends TestCase {

	public void testRun() {
		_126Worker w = new _126Worker("importerxn", "123test456", null, null);
		w.run();
	}

}
