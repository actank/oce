package com.xiaonei.importer.CSV;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.UUID;
import java.util.Map.Entry;

import Ice.Current;

import com.xiaonei.importer.relationship.RelationshipHelper;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;
import com.xiaonei.svc.importer.BuddyUserInfo;
import com.xiaonei.svc.importer.OwnerInfo;

public class CSVImporter {

	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(CSVImporter.class);

	private static CSVImporter _instance = new CSVImporter();

	public static CSVImporter getInstance() {
		return _instance;
	}

	public CSVImporter() {

	}

	public void finishContactList(UUID requestId, BuddyInfo[] list,
			BuddyListStatus status) {
	}

	public void requestBuddyList(OwnerInfo owner, Current __current) {
		if (_logger.isDebugEnabled()) {
			_logger.debug("requestBuddyList OwnerInfo[" + owner.userId + ","
					+ owner.email + "," + owner.password + "]");
		}
		BuddyList newBuddyList = new BuddyList();
		newBuddyList.status = new BuddyListStatus(301, "processing...");
		newBuddyList.owner = owner;
		newBuddyList.users = new HashMap<String, BuddyUserInfo>();
		List<BuddyInfo> tmpBuddyInfo = new LinkedList<BuddyInfo>();
		for (Entry<String, String> entry : __current.ctx.entrySet()) {
			tmpBuddyInfo.add(new BuddyInfo(entry.getValue(), entry.getKey(),
					new BuddyInfoStatus(0, "Needs Fill")));
		}
		newBuddyList.buddies = tmpBuddyInfo.toArray(new BuddyInfo[tmpBuddyInfo
				.size()]);

		UUID requestId = UUID.randomUUID();
		InfomationCache.getInstance().putContacts(requestId, newBuddyList);
		InfomationCache.getInstance().putUUID(owner, requestId);
		
		RelationshipHelper.getInstance().fillRelationship(newBuddyList);
		newBuddyList.status = new BuddyListStatus(200, "OK");
	}

	public BuddyList getBuddyList(UUID requestId) {
		return InfomationCache.getInstance().getContacts(requestId);
	}

	public BuddyList retriveBuddyList(OwnerInfo owner) {
		if (_logger.isDebugEnabled()) {
			_logger.debug("retriveBuddyList OwnerInfo[" + owner.userId + ","
					+ owner.email + "," + owner.password + "]");
		}
		UUID requestId = InfomationCache.getInstance().getUUID(owner);
		if (_logger.isDebugEnabled()) {
			_logger.debug("retriveBuddyList OwnerInfo[" + owner.userId + ","
					+ owner.email + "," + owner.password + "] UUID="
					+ requestId);
		}
		if (null == requestId) {
			throw new NoSuchElementException("No Such request for userId="
					+ String.valueOf(owner.userId));
		}
		return getBuddyList(requestId);
	}
}
