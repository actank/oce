package com.xiaonei.xce.buddy.relation.impl;

import java.util.List;

import mop.hi.oce.adapter.BuddyCoreAdapter;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.domain.buddy.BuddyApply;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.buddy.BuddyBlock;
import mop.hi.oce.domain.buddy.BuddyRelation;

import com.xiaonei.xce.buddy.relation.BuddyRelationWriteContext;

public class BuddyRelationWriteContextImpl implements BuddyRelationWriteContext {

	protected BuddyCoreAdapter buddycore = new BuddyCoreAdapterImpl();
	
	@Override
	public void acceptAllApply(int accepter) {
		// TODO Auto-generated method stub
		buddycore.acceptAllApply(accepter);
	}

	@Override
	public void acceptApply(BuddyApply request) {
		// TODO Auto-generated method stub
		buddycore.acceptApply(request);
	}

	@Override
	public void addApply(BuddyApplyWithInfo request, String ref) {
		// TODO Auto-generated method stub
		buddycore.addApply(request,ref);
	}

	@Override
	public void addApplyBatch(List<BuddyApplyWithInfo> requests, String ref) {
		// TODO Auto-generated method stub
		buddycore.addApplyBatch(requests, ref);
	}

	@Override
	public void addBlock(BuddyBlock block) {
		// TODO Auto-generated method stub
		buddycore.addBlock(block);
	}

	@Override
	public void addFriend(int from, int to) {
		// TODO Auto-generated method stub
		buddycore.addFriendDirectly(from, to);
	}

	@Override
	public void denyAllApplyByApplicant(int applicant) {
		// TODO Auto-generated method stub
		buddycore.denyAllApplyByApplicant(applicant);
	}

	@Override
	public void denyApply(BuddyApply request) {
		// TODO Auto-generated method stub
		buddycore.denyApply(request);
	}

	@Override
	public void removeBlock(BuddyBlock block) {
		// TODO Auto-generated method stub
		buddycore.removeBlock(block);
	}

	@Override
	public void removeFriend(BuddyRelation relation) {
		// TODO Auto-generated method stub
		buddycore.removeFriend(relation);
	}

	@Override
	public void denyAllApply(int accepter) {
		// TODO Auto-generated method stub
		buddycore.denyAllApply(accepter);
	}

}
