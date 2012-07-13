package com.xiaonei.importer.sina;

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

public class SinaImporter implements WorkerCallback {

	private static SinaImporter instance = new SinaImporter();
	public static SinaImporter getInstance() {
		return instance;
	}
	private SinaImporter() {
	}

	public void finishContactList(UUID requestId, BuddyInfo[] list,
			BuddyListStatus status) {
		BuddyList buddies = InfomationCache.getInstance().getContacts(requestId);
		buddies.buddies = list;
		if (status.code >= 200 && status.code < 300 && buddies.buddies.length > 0) {
			RelationshipHelper.getInstance().fillRelationship(buddies);
		}
		buddies.status = status;
	}
	
	public BuddyList getBuddyList(UUID requestId) {
		return InfomationCache.getInstance().getContacts(requestId);
	}
	
	public BuddyList retriveBuddyList(OwnerInfo owner) {
		UUID requestId = InfomationCache.getInstance().getUUID(owner);
		if (null == requestId) {
			throw new NoSuchElementException("No Such request for userId=" + String.valueOf(owner.userId));
		}
		return getBuddyList(requestId);
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

		TaskManager.getInstance().addTask(new SinaWorker(owner.email, owner.password, requestId, this));
	}


}
