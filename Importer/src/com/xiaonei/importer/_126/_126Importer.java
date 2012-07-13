package com.xiaonei.importer._126;

import java.util.HashMap;
import java.util.NoSuchElementException;
import java.util.UUID;

import com.xiaonei.importer.TaskManager;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.importer.google.InfomationCache;
import com.xiaonei.importer.relationship.RelationshipHelper;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;
import com.xiaonei.svc.importer.BuddyUserInfo;
import com.xiaonei.svc.importer.OwnerInfo;

public class _126Importer implements WorkerCallback {
	private static _126Importer instance = new _126Importer();
	
	public static _126Importer getInstance() {
		return instance;
	}
	private _126Importer(){
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

		TaskManager.getInstance().addTask(new _126Worker(owner.email, owner.password, requestId, this));
	}
	
	public BuddyList retriveBuddyList(OwnerInfo owner) {
		UUID requestId = InfomationCache.getInstance().getUUID(owner);
		if (null == requestId) {
			throw new NoSuchElementException("No Such request for userId=" + String.valueOf(owner.userId));
		}
		return getBuddyList(requestId);
	}
	
	public BuddyList getBuddyList(UUID requestId) {
		return InfomationCache.getInstance().getContacts(requestId);
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

}
