package com.xiaonei.importer;

import java.util.UUID;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyListStatus;

public interface WorkerCallback {
	public void finishContactList(UUID requestId, BuddyInfo[] list, BuddyListStatus status);
}
