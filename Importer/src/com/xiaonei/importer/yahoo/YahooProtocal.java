package com.xiaonei.importer.yahoo;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;

public class YahooProtocal {

	@SuppressWarnings("unchecked")
	public static final BuddyInfo[] prase(String response) throws YahooProtocalException {
		List<BuddyInfo> result = new ArrayList<BuddyInfo>();
		String[] ss = response.split("\r\n");
		
		try {
		for(int i = 1; i < ss.length; i++) {
			String[] sss = ss[i].split(",");
			String name = sss[3];
			name = name.substring(1, name.length() -  1);
			String email = sss[4];
			email = email.substring(1, email.length() - 1);
			result.add(new BuddyInfo(name, email, new BuddyInfoStatus(0, "Contact not got.")));
		}
		} catch (Exception e) {
			throw new YahooProtocalException(e.getMessage());
		}
		
		return result.toArray(new BuddyInfo[result.size()]);

	}
	
	public static class YahooProtocalException extends Exception {
		/**
		 * 
		 */
		private static final long serialVersionUID = -3404951448696274911L;

		public YahooProtocalException(String code) {
			super("YahooProtocalException: "+code);
		}
	}
	
}
