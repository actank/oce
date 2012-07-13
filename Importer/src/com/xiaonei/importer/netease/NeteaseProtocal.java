package com.xiaonei.importer.netease;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Node;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;

public class NeteaseProtocal {

	@SuppressWarnings("unchecked")
	public static final BuddyInfo[] prase(String response) throws DocumentException, NeteaseProtocalException {
		List<BuddyInfo> result = new ArrayList<BuddyInfo>();
		
		Document document=DocumentHelper.parseText(response);
		
		Node code = document.selectSingleNode("/result/code");
		if(!"S_OK".equals(code.getText())) {
			throw new NeteaseProtocalException(code.getText());
		}
		
		List<Node> contacts = document.selectNodes("/result/array/object/array/object[string/@name='EMAIL;PREF']");
		for(Node c : contacts) {
			String email = c.selectSingleNode("string[@name='EMAIL;PREF']").getText();
			Node nameN = c.selectSingleNode("string[@name='FN']");
			String name;
			if(nameN == null)
				name = "";
			else
				name = nameN.getText();
			result.add(new BuddyInfo(name, email, new BuddyInfoStatus(0, "Contact not got.")));
		}
		
		return result.toArray(new BuddyInfo[result.size()]);
	}
	
	public static class NeteaseProtocalException extends Exception {
		/**
		 * 
		 */
		private static final long serialVersionUID = -3404951448696274911L;

		public NeteaseProtocalException(String code) {
			super("NeteaseProtocalException: "+code);
		}
	}
	
}
