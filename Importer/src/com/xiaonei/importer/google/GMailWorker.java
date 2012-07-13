package com.xiaonei.importer.google;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import com.google.gdata.client.contacts.ContactsService;
import com.google.gdata.data.contacts.ContactEntry;
import com.google.gdata.data.contacts.ContactFeed;
import com.google.gdata.data.extensions.Email;
import com.google.gdata.data.extensions.Im;
import com.google.gdata.util.AuthenticationException;
import com.google.gdata.util.ServiceException;
import com.xiaonei.importer.AbstractWorker;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;

public class GMailWorker extends AbstractWorker {
	
	private final String uid;
	private final String passwd;
	
	private BuddyInfo[] _buddylist;
	
	ContactsService service = new ContactsService("Google-contactsExampleApp-1");
	
	protected GMailWorker(String uid, String passwd, UUID requestId, WorkerCallback callback) {
		super(requestId, callback);
		this.uid = uid;
		this.passwd = passwd;
	}

	public void run() {
		boolean auth = getAuth();
		
		BuddyListStatus status = null;
		if(auth) {
			status = new BuddyListStatus(302, "Token OK, needs conntacts");
		} else {
			status = new BuddyListStatus(403, "Auth failed.");
		}
		BuddyList list = InfomationCache.getInstance().getContacts(this._requestId);
		list.status = status;
		
		if(auth) {
			exportContacts();
		}
	}

	boolean getAuth() {
		try {
			service.setUserCredentials(uid, passwd);
		} catch (AuthenticationException e) {
			return false;
		}
		return true;
	}

	void exportContacts() {
		List<BuddyInfo> result = new ArrayList<BuddyInfo>();
		
		URL feedUrl = null;
		try {
			feedUrl = new URL(
					"http://www.google.com/m8/feeds/contacts/default/base");
		} catch (MalformedURLException e) {
			e.printStackTrace();
		}
		ContactFeed resultFeed = null;
		try {
			resultFeed = service.getFeed(feedUrl, ContactFeed.class);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ServiceException e) {
			e.printStackTrace();
		}

		for (int i = 0; i < resultFeed.getEntries().size(); i++) {
			ContactEntry entry = resultFeed.getEntries().get(i);
			String name = entry.getTitle().getPlainText();
			if(name == null) {
				name = "";
			}

			Email em = entry.getEmailAddresses().get(0);
			String email = em.getAddress();
			result.add(new BuddyInfo(name, email, new BuddyInfoStatus(0, "Contact not got.")));
		}
		
		_buddylist = result.toArray(new BuddyInfo[result.size()]);
		BuddyListStatus status = new BuddyListStatus(200, "OK");
		_callback.finishContactList(_requestId, _buddylist, status);
	}

}
