package com.xiaonei.xce.feed.reply;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.io.IOException;
import java.io.StringWriter;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;

public class BuildDocument2 {
	private static BuildDocument2 instance;
	private Document document;

	private Element f_element;
	private Element reply_element;

	private Element id_element;
	private Element type_element;
	private Element time_element;
	private Element body_element;
	private Element im_element;
	private Element from_element;

	private Element im_body_element;

	private Element from_id_element;
	private Element from_name_element;
	private Element from_tinyimg_element;
	private Element from_icon_element;

	private SimpleDateFormat formatter = new SimpleDateFormat(
			"yyyy-MM-dd HH:mm");

	private String getStringDate(Date date) {
		return formatter.format(date);
	}

	protected BuildDocument2() {
		document = DocumentHelper.createDocument();
		document.setXMLEncoding("utf-8");

		f_element = document.addElement("f");

		reply_element = f_element.addElement("reply");

		id_element = reply_element.addElement("id");

		type_element = reply_element.addElement("type");
		time_element = reply_element.addElement("time");
		body_element = reply_element.addElement("body");
		im_element = reply_element.addElement("im");
		from_element = reply_element.addElement("from");

		im_body_element = im_element.addElement("body");

		from_id_element = from_element.addElement("id");
		from_name_element = from_element.addElement("name");
		from_tinyimg_element = from_element.addElement("tinyimg");
		from_icon_element = from_element.addElement("icon");
	}

	public static BuildDocument2 getInstance() {
		if (instance == null) {
			instance = new BuildDocument2();
		}
		return instance;
	}
	
	private void clearXml() {
		id_element.clearContent();

		type_element.clearContent();

		time_element.clearContent();
		body_element.clearContent();

		im_body_element.clearContent();

		from_id_element.clearContent();
		from_name_element.clearContent();
		from_tinyimg_element.clearContent();
		from_icon_element.clearContent();

	}

	//istrans为true，转义，为false，不转义
	private String formatXml(boolean istrans) {
		OutputFormat format = OutputFormat.createPrettyPrint();
		format.setEncoding("utf-8");
		StringWriter sw = new StringWriter();
		XMLWriter xw = new XMLWriter(sw, format);
		xw.setEscapeText(istrans);
		try {
			xw.write(document);
			xw.flush();
			xw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return sw.toString();
		
	}

	
	public String getReplyXml(FeedReplyObj replyObj) {
		
		if (replyObj == null) {
			try {
				throw new Exception("replyObj is null !");
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
		
		String ret = null;
		
		clearXml();
		id_element.addText(String.valueOf(replyObj.getReplyId()));

		type_element.addText(String.valueOf(replyObj.getReplyType()));

		time_element.addText(getStringDate(replyObj.getTime()));
		// System.out.println("co:" + reply.newContent);
		// System.out.println("cont:"
		// + XmlFilter.getInstance().write(reply.newContent));.
		// body_element.addText("\"" + XmlFilter.getInstance().write(replyObj.getContent()) + "\"");

		body_element.addText(XmlFilter.getInstance().write(replyObj.getContent()));
		im_body_element.addText(XmlFilter.getInstance().write(replyObj.getImContent()));

		from_id_element.addText(String.valueOf(replyObj.getFromId()));
		from_name_element.addText(XmlFilter.getInstance().write(replyObj.getFromName()));
		from_tinyimg_element.addText(replyObj.getTinyImg());
		from_icon_element.addText(replyObj.getIsShowIcon());
		//ret = f_element.asXML();
		ret = formatXml(false);
		ret = ret.substring(ret.indexOf("<f>"));
		ret = ret.trim();	

		return ret;
	}
	
}
