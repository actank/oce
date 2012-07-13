package com.xiaonei.xce.buddy.relation.impl;

import mop.hi.oce.adapter.BuddyCoreAdapter;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;

import com.xiaonei.xce.buddy.relation.BuddyRelationContext;
import com.xiaonei.xce.buddy.relation.BuddyRelationReadContext;
import com.xiaonei.xce.buddy.relation.BuddyRelationWriteContext;

public class BuddyRelationContextImpl implements BuddyRelationContext {

//	private BuddyRelationContext instance;
	
//	
//	public BuddyRelationContext getInstance() {
//		if (instance == null) {
//			synchronized (this) {
//				if (instance == null) {
//					instance = new BuddyRelationContextImpl();
//				}
//			}
//		}
//		return instance;
//	}

	protected BuddyCoreAdapter buddycore = new BuddyCoreAdapterImpl();
	
	private BuddyRelationReadContext readContext = new BuddyRelationReadContextImpl();

	private BuddyRelationWriteContext writeContext = new BuddyRelationWriteContextImpl();

	@Override
	public BuddyRelationReadContext read() {
		return readContext;
	}

	@Override
	public BuddyRelationWriteContext write() {
		return writeContext;
	}

}
