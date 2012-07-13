package com.xiaonei.xce.xfeed;
import com.xiaonei.xce.log.Oce;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

//import junit.framework.TestCase;

public class XFeedTester {// extends TestCase {
	// public void testDispatch() throws Exception {
	// XFeedNewsAdapter xfna = new XFeedNewsAdapter(new XFeedFactoryI());
	// XFeedMiniAdapter xfma = new XFeedMiniAdapter(new XFeedFactoryI());
	// xfna.setAllXFeedNewsRead(80578957);
	// xfma.setAllXFeedMiniRead(80578957);
	// Thread.sleep(3*1000);
	// String xfeedXml =
	// "<feed><feedTimestamp>1220853867384</feedTimestamp><feedType>2301003</feedType><from><fromName>周冠莹chloe</fromName><fromID>251835762</fromID></from><poll><title>你喜欢哪种毛绒玩具</title><link>http://abc.xiaonei.com/knowabc/investigation/Voted.action?invID=587143</link></poll></feed>";
	// Map xfeedProp = new HashMap();
	// xfeedProp.put("actor", "80578957");
	// xfeedProp.put("type", "10");
	// xfeedProp.put("stype", "1001");
	// xfeedProp.put("resource", String.valueOf(System.currentTimeMillis()));
	// xfeedProp.put("weight", "200");
	// xfeedProp.put("merge", "1");
	// xfeedProp.put("renew", "1");
	// xfeedProp.put("unread", "1");
	// xfeedProp.put("expr", "u(80578957)");
	// XFeedDispatcherAdapter.getInstance().dispatch(xfeedXml, xfeedProp);
	// Thread.sleep(3*1000);
	// XFeedQuery xfqn = xfna.getXFeedNewsSeq(80578957, true, 0, -1);
	// XFeedQuery xfqm = xfma.getXFeedMiniSeq(80578957, true, 0, -1);
	// assertEquals(1, xfqn.getUnread());
	// assertEquals(1, xfqm.getUnread());
	//		
	//		
	// xfna.setXFeedNewsRead(80578957, xfqn.getData().get(0).getId());
	// xfma.setXFeedMiniRead(80578957, xfqm.getData().get(0).getId());
	//		
	// Thread.sleep(3*1000);
	// xfqn = xfna.getXFeedNewsSeq(80578957, true, 0, -1);
	// xfqm = xfma.getXFeedMiniSeq(80578957, true, 0, -1);
	// assertEquals(0, xfqn.getUnread());
	// assertEquals(0, xfqm.getUnread());
	// }
/*
	public void testXFeedMini() throws Exception {
		int uid = 620000013;
		XFeedMiniAdapter xfma = new XFeedMiniAdapter(new XFeedFactoryI());
		XFeedQuery xfqm = xfma.getXFeedMiniSeq(uid, false, 0, -1);
		System.out.println(xfqm.getData().size());
		List<XFeedContent> l = xfqm.getData();
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
		
		System.out.println("-----------");

		for (int j = 10; j < 100; j = j + 5) {
			xfqm = xfma.getXFeedMiniSeq(uid, false, j, 5);
			l = xfqm.getData();
			System.out.println(l.size());
			for (int i = 0; i < l.size(); ++i) {
				System.out.print("id:");
				System.out.print(l.get(i).getId());
				System.out.print(" -> stype:");
				System.out.print(l.get(i).getStype());
				System.out.print(" -> type:");
				System.out.print(l.get(i).getType());
				System.out.print(" -> unread:");
				System.out.println(l.get(i).isUnread());
			}
		}
		System.out.println("---test remove feed and reload---");
		xfqm = xfma.getXFeedMiniSeq(uid, false, 0, -1);
		l = xfqm.getData();
		System.out.println(l.size());
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
//		xfma.setXFeedMiniRead(uid, xfqm.getData().get(0).getId());
//		xfma.setXFeedMiniRead(uid, xfqm.getData().get(1).getId());
//		xfma.setXFeedMiniRead(uid, xfqm.getData().get(2).getId());
//		System.out.println("-------remove---------");
//		xfqm = xfma.getXFeedMiniSeq(uid, false, 0, -1);
//		l = xfqm.getData();
//		System.out.println(l.size());
//		for (int i = 0; i < l.size(); ++i) {
//			System.out.print("id:");
//			System.out.print(l.get(i).getId());
//			System.out.print(" -> stype:");
//			System.out.print(l.get(i).getStype());
//			System.out.print(" -> type:");
//			System.out.print(l.get(i).getType());
//			System.out.print(" -> unread:");
//			System.out.println(l.get(i).isUnread());
//		}
//		
//		System.out.println("-------sleep 10---------");
//		Thread.sleep(20000);
//		xfqm = xfma.getXFeedMiniSeq(uid, false, 0, -1);
//		l = xfqm.getData();
//		System.out.println(l.size());
//		for (int i = 0; i < l.size(); ++i) {
//			System.out.print("id:");
//			System.out.print(l.get(i).getId());
//			System.out.print(" -> stype:");
//			System.out.print(l.get(i).getStype());
//			System.out.print(" -> type:");
//			System.out.print(l.get(i).getType());
//			System.out.print(" -> unread:");
//			System.out.println(l.get(i).isUnread());
//		}
		
		System.out.println("-------test stype ---------");
		int stypes[] ={102,601};
		xfqm = xfma.getXFeedMiniSeqBySType(uid, stypes, false, 0, -1);
		l = xfqm.getData();
		System.out.println(l.size());
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
		
		System.out.println("-------test stype limit ---------");

		xfqm = xfma.getXFeedMiniSeqBySType(uid, stypes, false, 1, 2);
		l = xfqm.getData();
		System.out.println(l.size());
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
	
	//-------------test get by type--
		System.out.println("-------test stype ---------");
		int types[] ={12,6};
		xfqm = xfma.getXFeedMiniSeqByType(uid, types, false, 0, -1);
		l = xfqm.getData();
		System.out.println(l.size());
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
		
		System.out.println("-------test stype limit ---------");

		xfqm = xfma.getXFeedMiniSeqByType(uid, types, false, 1, 2);
		l = xfqm.getData();
		System.out.println(l.size());
		for (int i = 0; i < l.size(); ++i) {
			System.out.print("id:");
			System.out.print(l.get(i).getId());
			System.out.print(" -> stype:");
			System.out.print(l.get(i).getStype());
			System.out.print(" -> type:");
			System.out.print(l.get(i).getType());
			System.out.print(" -> unread:");
			System.out.println(l.get(i).isUnread());
		}
	
	}
*/
	public static void main(String args[]) throws Exception {
//		XFeedNewsAdapter xfeednews = new XFeedNewsAdapter(new XFeedFactoryI());
//		 System.out.println(xfeednews.getXFeedNewsSeqByType(202911262,new
//		 int[]{1,6}, true, 0, 100));
//		System.out.println(xfeednews.getXFeedNewsSeq(229870725, true, 0, 100));
				
		 String xfeedXml =
		 "<feed><feedTimestamp>1220853867384</feedTimestamp><feedType>2301003</feedType><from><fromName>周冠莹chloe</fromName><fromID>251835762</fromID></from><poll><title>你喜欢哪种毛绒玩具</title><link>http://abc.xiaonei.com/knowabc/investigation/Voted.action?invID=587143</link></poll></feed>";
		 Map xfeedProp = new HashMap();
		 xfeedProp.put("actor", "242332421");
		 xfeedProp.put("type", "3");
		 xfeedProp.put("stype", "301");
		 xfeedProp.put("resource", "2112301");
		 xfeedProp.put("merge", "1");
		 xfeedProp.put("unread", "1");
		 xfeedProp.put("time", "1");
		 xfeedProp.put("expr", "f(232332421)");
		 XFeedDispatcherAdapter.getInstance().dispatch(xfeedXml, xfeedProp);
		System.exit(0);
	}
	
	public void testDispatchAgain()
	{
		for(int i = 60*24; i < 1800; ++i){
			System.out.println(i);
			XFeedDispatcherAdapter.getInstance().dispatchAgain(i, i+1);
			try {
				Thread.sleep(30000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				Oce.getLogger().error(this.getClass().getName() + " " + e);
			}
		}
	}
	
	public void testNews()
	{
		
	}
}

//class XFeedFactoryI implements XFeedFactory {
//	public XFeedContent create() {
//		return new XFeedContent();
//	}
//}
