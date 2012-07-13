package com.xiaonei.xce.feed.reply;

import java.util.Date;
import java.text.SimpleDateFormat;
import java.io.IOException;
import java.io.StringWriter;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;

import xce.feed.FeedReply;

import com.xiaonei.xce.feed.FeedLogicAdapter;

public class FeedReplyDispatcher2 {
	

	private static FeedReplyDispatcher2 instance = new  FeedReplyDispatcher2();

	protected FeedReplyDispatcher2() {
	};

	public static FeedReplyDispatcher2 getInstance() {
		/*if (instance == null) {
			instance = new FeedReplyDispatcher2();
		}*/
		return instance;
	}

	private void initDocument(Document document) {
		document.setXMLEncoding("utf-8");
		Element f_element = document.addElement("f");
		document.setRootElement(f_element);
		Element reply_element = f_element.addElement("reply");
		reply_element.addElement("id");
		reply_element.addElement("type");
                reply_element.addElement("time");
                reply_element.addElement("body");
		Element im_element = reply_element.addElement("im");
		im_element.addElement("body");
		Element from_element = reply_element.addElement("from");
		from_element.addElement("id");
		from_element.addElement("name");
		from_element.addElement("tinyimg");
		from_element.addElement("icon");
	}

	private void fillDocument(Document document, FeedReplyObj replyObj) {
		Element f_element = document.getRootElement();
                Element reply_element = f_element.element("reply");
                Element id_element = reply_element.element("id");
                Element type_element = reply_element.element("type");
                Element time_element = reply_element.element("time");
                Element body_element = reply_element.element("body");
                Element im_element = reply_element.element("im");
                Element im_body_element = im_element.element("body");
                Element from_element = reply_element.element("from");
                Element from_id_element = from_element.element("id");
                Element from_name_element = from_element.element("name");
                Element from_tinyimg_element = from_element.element("tinyimg");
                Element from_icon_element = from_element.element("icon");

		id_element.clearContent();
                type_element.clearContent();
                time_element.clearContent();
                body_element.clearContent();
                im_body_element.clearContent();
                from_id_element.clearContent();
                from_name_element.clearContent();
                from_tinyimg_element.clearContent();
                from_icon_element.clearContent();

                id_element.addText(String.valueOf(replyObj.getReplyId()));
                type_element.addText(String.valueOf(replyObj.getReplyType()));
                SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm");
                String replyTime = formatter.format(replyObj.getTime());
                time_element.addText(replyTime);
                body_element.addText(XmlFilter.getInstance().write(replyObj.getContent()));
                im_body_element.addText(XmlFilter.getInstance().write(replyObj.getImContent()));
                from_id_element.addText(String.valueOf(replyObj.getFromId()));
                from_name_element.addText(XmlFilter.getInstance().write(replyObj.getFromName()));
                from_tinyimg_element.addText(replyObj.getTinyImg());
                from_icon_element.addText(replyObj.getIsShowIcon());
	}

	private String documentToXml(Document document) {
		String ret = null;

		OutputFormat format = OutputFormat.createPrettyPrint();
		format.setEncoding("utf-8");
		format.setSuppressDeclaration(true);
		StringWriter sw = new StringWriter();
		XMLWriter xw = new XMLWriter(sw, format);
		xw.setEscapeText(false);
		try {
			xw.write(document);
			xw.flush();
			xw.close();
			ret = sw.toString();
		} catch (IOException e) {
			e.printStackTrace();
			ret = null;
		}
		return ret;
	}

	public void removeFeedReply(FeedReplyObj newReplyObj, FeedReplyObj oldReplyObj, int count, long removeReplyId, long source, int stype, int actor) {
		String newReplyXml = null;
		String oldReplyXml = null;

		Document document = DocumentHelper.createDocument();
		initDocument(document);

		FeedReply reply = new FeedReply();
		reply.count = count;
		
		if (newReplyObj != null) {

			try {
				fillDocument(document, newReplyObj);
				newReplyXml = documentToXml(document);
				if (newReplyXml == null) {
					throw new Exception("newReplyXml is null !");
				}
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}

			reply.newReply = newReplyXml;
			reply.newReplyId = newReplyObj.getReplyId();
		}
		
		if (oldReplyObj != null) {

			try {
				fillDocument(document, oldReplyObj);
				oldReplyXml = documentToXml(document);
                                if (oldReplyXml == null) {
                                        throw new Exception("oldReplyXml is null !");
                                }
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}

			reply.oldReply = oldReplyXml;
			reply.oldReplyId = oldReplyObj.getReplyId();
		}
		
		FeedLogicAdapter.getInstance().removeFeedReply(source, stype, actor, removeReplyId, reply);
	}
		
	public void addFeedReply(FeedReplyObj newReplyObj, FeedReplyObj oldReplyObj, int count, long source, int stype, int actor) {	
		String newReplyXml = null;
		String oldReplyXml = null;

		Document document = DocumentHelper.createDocument();
		initDocument(document);
	
		FeedReply reply = new FeedReply();
		reply.count = count;
		
		if (newReplyObj != null) {

			try {
				fillDocument(document, newReplyObj);
                                newReplyXml = documentToXml(document);
				if (newReplyXml == null) {
                                        throw new Exception("newReplyXml is null !");
                                }
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}

			reply.newReply = newReplyXml;
			reply.newReplyId = newReplyObj.getReplyId();
		}
		
		if (oldReplyObj != null) {

			try {
				fillDocument(document, oldReplyObj);
				oldReplyXml = documentToXml(document);
                                if (oldReplyXml == null) {
                                        throw new Exception("oldReplyXml is null !");
                                }
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}

			reply.oldReply = oldReplyXml;
			reply.oldReplyId = oldReplyObj.getReplyId();
		}
		
		FeedLogicAdapter.getInstance().addFeedReply(source, stype, actor, reply);
	}

}
