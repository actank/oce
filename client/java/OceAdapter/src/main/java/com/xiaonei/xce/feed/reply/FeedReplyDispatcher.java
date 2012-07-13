package com.xiaonei.xce.feed.reply;

import java.util.Date;

import xce.feed.FeedReply;

import com.xiaonei.xce.feed.FeedLogicAdapter;

public class FeedReplyDispatcher {
	

	private static FeedReplyDispatcher instance;

	protected FeedReplyDispatcher() {
	};

	public static FeedReplyDispatcher getInstance() {
		if (instance == null) {
			instance = new FeedReplyDispatcher();
		}
		return instance;
	}

	
/*
	private String BuildOldReply(FeedReplyPara rep)  {

		Map<String, Object> oldMap = new HashMap();
		oldMap.put("rid", rep.oldReplyId);
		oldMap.put("type", rep.oldReplyType);
		oldMap.put("rt", getStringDate(rep.oldTime));
		//oldMap.put("con", "\"" + XmlFilter.getInstance().write(XmlFilter.getInstance().write(rep.oldContent)) + "\"");
		oldMap.put("hcon", rep.oldImContent);

		oldMap.put("ui", rep.oldFromId);
		oldMap.put("un", rep.oldFromName);
		oldMap.put("tu", rep.oldTinyImg);
		oldMap.put("isShowIcon", rep.oldIsShowIcon);

		String xml = null;
		String escapedXml = null;
		try {
			xml = XmlTemplateMangager.getInstance().process(
					REPLY_FTL_PATH, oldMap);
			escapedXml = XmlFilter.getInstance().write(xml);
			if (null == escapedXml) {
				// System.out.println("xml is null!");
//				FeedLoger
//						.getLoger()
//						.error(
//								"FeedReply Xml create error cause by:\n ftl parse result is null");
				//throw new PublishException("FeedReply Xml Exception)");
			}

		} catch (Exception e) {
			e.printStackTrace();
			//.getLoger().fatal("excetion xml=" + escapedXml);
			//throw new PublishException("Xml create error cause by:\n" + e);
		}
		return escapedXml;
	}
	*/
/*
	private String BuildNewReply(FeedReplyPara rep) throws PublishException {

		Map<String, Object> newMap = new HashMap();
		newMap.put("rid", rep.newReplyId);
		newMap.put("type", rep.newReplyType);
		newMap.put("rt", getStringDate(rep.newTime));
		//newMap.put("con", "\""+rep.newContent+"\"");
		newMap.put("con", "\"" + XmlFilter.getInstance().write(XmlFilter.getInstance().write(rep.newContent)) + "\"");
		System.out.println("con:" + XmlFilter.getInstance().write(rep.newContent) );
		System.out.println(XmlFilter.getInstance().write(XmlFilter.getInstance().write(rep.newContent)));
		newMap.put("hcon", rep.newImContent);

		newMap.put("ui", rep.newFromId);
		newMap.put("un", rep.newFromName);
		newMap.put("tu", rep.newTinyImg);
		newMap.put("isShowIcon", rep.newIsShowIcon);

		String xml = null;
	
		try {
			xml = XmlTemplateMangager.getInstance().process(
					REPLY_FTL_PATH, newMap);
						
			if (null == xml) {
				// System.out.println("xml is null!");
//				FeedLoger
//						.getLoger()
//						.error(
//								"FeedReply Xml create error cause by:\n ftl parse result is null");
//				throw new PublishException("FeedReply Xml Exception)");
			}

		} catch (Exception e) {
			e.printStackTrace();
//			FeedLoger.getLoger().fatal("excetion xml=" + xml);
//			throw new PublishException("Xml create error cause by:\n" + e);
		}
		return xml;
	}
*/
	public void removeFeedReply(FeedReplyObj newReplyObj, FeedReplyObj oldReplyObj, int count, long removeReplyId, long source, int stype, int actor) {
		String newReplyXml = null;
		String oldReplyXml = null;

		FeedReply reply = new FeedReply();
		reply.count = count;
		
		if (newReplyObj != null) {

			try {
				newReplyXml = BuildDocument.getInstance().getReplyXml(
						newReplyObj);
				if (newReplyXml == null) {
					throw new Exception("newReplyXml is null !");
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			// reply.newReply =
			// "<f><reply><id>111</id><type>0</type><time>2011-07-06 15:19</time><body>\"abbbaab\"</body><im><body></body></im><from><id>123</id><name></name><tinyimg></tinyimg><icon></icon></from></reply></f>";

			reply.newReply = newReplyXml;
			reply.newReplyId = newReplyObj.getReplyId();
		}
		
		if (oldReplyObj != null) {

			try {
				oldReplyXml = BuildDocument.getInstance().getReplyXml(
						oldReplyObj);
				if (oldReplyXml == null) {
					throw new Exception("oldReplyXml is null !");
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			// reply.oldReply =
			// "<f><reply><id>111</id><type>0</type><time>2011-07-06 15:19</time><body>\"abbbaab\"</body><im><body></body></im><from><id>123</id><name></name><tinyimg></tinyimg><icon></icon></from></reply></f>";

			reply.oldReply = oldReplyXml;
			reply.oldReplyId = oldReplyObj.getReplyId();
		}
		
		
		

		FeedLogicAdapter.getInstance().removeFeedReply(source, stype, actor, removeReplyId, reply);
	}
	
//	public void addFeedReply(FeedReplyObj newReplyObj, int count, long source, int stype, int actor) {
//		//FeedReplyXml reply_xml = null;
//		
//		if (newReplyObj == null) {
//			try {
//				throw new Exception("newReply is null !");
//			} catch (Exception e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//				return;
//			}
//				
//		}
//		
//		String newReplyXml = null;
//		
//		try {
//			newReplyXml = BuildDocument.getInstance().getReplyXml(newReplyObj);
//			if (newReplyXml == null) {
//				throw new Exception("newReplyXml is null !");
//			}
//
//		} catch (Exception e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//			return ;
//		}
//		
//		// System.out.println("xml:" + xml);
//		FeedReply reply = new FeedReply();
//		reply.count = count;
//		reply.newReply = newReplyXml;
//		//reply.newReply = "<f><reply><id>111</id><type>0</type><time>2011-07-06 15:19</time><body>\"abbbaab\"</body><im><body></body></im><from><id>123</id><name></name><tinyimg></tinyimg><icon></icon></from></reply></f>";
//		reply.newReplyId = newReplyObj.getReplyId();
//
//		FeedLogicAdapter.getInstance().addFeedReply(source, stype, actor, reply);
//	}
	
	public void addFeedReply(FeedReplyObj newReplyObj, FeedReplyObj oldReplyObj, int count, long source, int stype, int actor) {
	
		String newReplyXml = null;
		String oldReplyXml = null;

		FeedReply reply = new FeedReply();
		reply.count = count;
		
		if (newReplyObj != null) {

			try {
				newReplyXml = BuildDocument.getInstance().getReplyXml(
						newReplyObj);
				if (newReplyXml == null) {
					throw new Exception("newReplyXml is null !");
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			// reply.newReply =
			// "<f><reply><id>111</id><type>0</type><time>2011-07-06 15:19</time><body>\"abbbaab\"</body><im><body></body></im><from><id>123</id><name></name><tinyimg></tinyimg><icon></icon></from></reply></f>";

			reply.newReply = newReplyXml;
			reply.newReplyId = newReplyObj.getReplyId();
		}
		
		if (oldReplyObj != null) {

			try {
				oldReplyXml = BuildDocument.getInstance().getReplyXml(
						oldReplyObj);
				if (oldReplyXml == null) {
					throw new Exception("oldReplyXml is null !");
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			// reply.oldReply =
			// "<f><reply><id>111</id><type>0</type><time>2011-07-06 15:19</time><body>\"abbbaab\"</body><im><body></body></im><from><id>123</id><name></name><tinyimg></tinyimg><icon></icon></from></reply></f>";

			reply.oldReply = oldReplyXml;
			reply.oldReplyId = oldReplyObj.getReplyId();
		}
		
		
		

		FeedLogicAdapter.getInstance().addFeedReply(source, stype, actor, reply);
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
//		String str;
	
		// TODO Auto-generated method stub
		FeedReplyDispatcher dis = FeedReplyDispatcher.getInstance();
		//System.out.println("ss:" + dis.ll);
		FeedReplyObj rp = new FeedReplyObj();
		rp.setContent("ccccccccccc");
		rp.setFromId(225092473);
		rp.setFromName("啊迪");
		rp.setReplyId(111);
		rp.setTime(new Date());
		
		
		FeedReplyObj rp2 = new FeedReplyObj();
		rp2.setContent("bbbbbbbb");
		rp2.setFromId(225092473);
		rp2.setFromName("啊迪");
		rp2.setReplyId(112);
		rp2.setTime(new Date());
		
//		rp.actor=327580522;
//		rp.count=1;
//		rp.source=2180336678L;
//		rp.sType=502;
//		rp.newFromId=123;
//		rp.newContent="abb<baab";
//		rp.newTime=new Date();
//		rp.newReplyId=111;
	    
		//dis.addFeedReply(rp,rp2, 3, 2180336678L, 502, 327580522);
		dis.removeFeedReply(null, null, 3, 111, 2180336678L, 502, 327580522);
		
	}

}