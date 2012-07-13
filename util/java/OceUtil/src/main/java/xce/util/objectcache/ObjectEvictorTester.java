package xce.util.objectcache;

//import org.junit.Assert;
//import org.junit.Before;
//import org.junit.Test;

import xce.util.tools.Closeable;

public class ObjectEvictorTester {
//	private static class TestObject implements Closeable {
//		public TestObject(long id) {
//			System.out.println("TestObject constucted " + id);
//			this.id = id;
//		}
//
//		private final long id;
//		public char[] name = new char[10 * 1024];
//
//		@Override
//		protected void finalize() throws Throwable {
//			System.out.println("TestObject finallized " + id);
//		}
//
//		public void close() {
//			System.out.println("TestObject closed " + id);
//		}
//	}
//
//	private static class TestObjectFactory implements ObjectFactory<TestObject> {
//		public TestObject create(int id) {
//			return new TestObject(id);
//		}
//	}
//
//	/* Tests below */
//
//	private ObjectEvictor<TestObject> evictor = null;
//
//	@Before
//	public void initialize() {
//		evictor = new ObjectEvictor<TestObject>(new TestObjectFactory(), 10);
//	}
//
//	@Test
//	public void putIntoCache() {
//
//		for (int id = 0; id < 40; ++id) {
//			evictor.locate(id);
//			evictor.locate(id);
//			System.out.println(evictor.size());
//			try {
//				// System.gc();
//				// evictor.cache.flushAll();
//				Thread.sleep(10);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		}
//
//		System.err.println("second round");
//		for (int id = 0; id < 40; ++id) {
//			evictor.locate(id);
//		}
//
//		Assert.assertTrue(evictor.size() == 10);
//
//	}
}
