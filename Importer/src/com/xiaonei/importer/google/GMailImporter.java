package com.xiaonei.importer.google;

import java.util.HashMap;
import java.util.NoSuchElementException;
import java.util.UUID;

import com.xiaonei.importer.TaskManager;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.importer.relationship.RelationshipHelper;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;
import com.xiaonei.svc.importer.BuddyUserInfo;
import com.xiaonei.svc.importer.OwnerInfo;

public class GMailImporter implements WorkerCallback {
	
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	.getLog(GMailImporter.class);

	public void finishContactList(UUID requestId, BuddyInfo[] list,
			BuddyListStatus status) {
		BuddyList buddies = InfomationCache.getInstance().getContacts(requestId);
		// We need list to be put into BuddyList before fillRelationship
		buddies.buddies = list;
		if (status.code == 200 && buddies.buddies.length > 0) {
			RelationshipHelper.getInstance().fillRelationship(buddies);
		}
		buddies.status = status;
	}
	
	private static GMailImporter instance = new GMailImporter();
	private GMailImporter() {
	}
	public static GMailImporter getInstance() {
		return instance;
	}
	
	protected void finishAuthToken(UUID requestId, String token, BuddyListStatus status) {
		BuddyList list = InfomationCache.getInstance().getContacts(requestId);
		list.status = status;
		if (token != null) {
			InfomationCache.getInstance().putToken(requestId, token);
		}
	}
	
	public void requestBuddyList(OwnerInfo owner) {
		BuddyList newBuddyList = new BuddyList();
		newBuddyList.status = new BuddyListStatus(301, "Needs Token");
		newBuddyList.buddies = new BuddyInfo[] {};
		newBuddyList.owner = owner;
		newBuddyList.users = new HashMap<String, BuddyUserInfo>();

		UUID requestId = UUID.randomUUID();

		InfomationCache.getInstance().putContacts(requestId, newBuddyList);
		InfomationCache.getInstance().putUUID(owner, requestId);
		
		_logger.trace("requestBuddyList: requestId: " + requestId + ", owner.email: " + owner.email);

		TaskManager.getInstance().addTask(new GMailWorker(owner.email, owner.password, requestId, this));
	}

	public BuddyList retriveBuddyList(OwnerInfo owner) {
		UUID requestId = InfomationCache.getInstance().getUUID(owner);
		
		_logger.trace("retriveBuddyList: requestId: " + requestId + ", owner.email: " + owner.email);
		
		if (null == requestId) {
			throw new NoSuchElementException("No Such request for userId=" + String.valueOf(owner.userId));
		}
		return getBuddyList(requestId);
	}

	public BuddyList getBuddyList(UUID requestId) {
		return InfomationCache.getInstance().getContacts(requestId);
	}

}
