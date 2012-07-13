package com.xiaonei.importer.sina;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;

public class SinaProtocal {

	@SuppressWarnings("unchecked")
	public static final BuddyInfo[] prase(String response) throws SinaProtocalException {
		List<BuddyInfo> result = new ArrayList<BuddyInfo>();
		String[] ss = response.split("\n");
		try {
		for(int i = 1; i < ss.length; i++) {
			String[] sss = ss[i].split(",");
			String name = sss[1];
			String email = sss[3];
			result.add(new BuddyInfo(name, email, new BuddyInfoStatus(0, "Contact not got.")));
		}
		} catch (Exception e) {
			throw new SinaProtocalException(response);
		}
		
		return result.toArray(new BuddyInfo[result.size()]);

	}
	
	public static class SinaProtocalException extends Exception {
		/**
		 * 
		 */
		private static final long serialVersionUID = -3404951448696274911L;

		public SinaProtocalException(String code) {
			super("SinaProtocalException: "+code);
		}
	}
	
}
