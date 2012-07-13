package mop.hi.oce.domain.model;

import com.xiaonei.xce.usercount.UserCountData;

import mop.hi.svc.count.CAppInvite;
import mop.hi.svc.count.CCallInvite;
import mop.hi.svc.count.CClassInvite;
import mop.hi.svc.count.CContactRequest;
import mop.hi.svc.count.CEventInvite;
import mop.hi.svc.count.CFriendCount;
import mop.hi.svc.count.CFriendMemoRequest;
import mop.hi.svc.count.CGameInvite;
import mop.hi.svc.count.CGuestRequest;
import mop.hi.svc.count.CImLeaveword;
import mop.hi.svc.count.CNewMessage;
import mop.hi.svc.count.CNewRequest;
import mop.hi.svc.count.CNotify;
import mop.hi.svc.count.COfferMSNContact;
import mop.hi.svc.count.CPhotoTagInvite;
import mop.hi.svc.count.CPoke;
import mop.hi.svc.count.CRecommendedFriend;
import mop.hi.svc.count.CRegApply;
import mop.hi.svc.count.CSurveyInvite;
import mop.hi.svc.count.CTeamInvite;
import mop.hi.svc.count.CTestInvite;
import mop.hi.svc.count.CTotalTypeSize;
import mop.hi.svc.count.CTribeRequest;

public class Counter {

	public Counter(UserCountData obj) {
		_values = new int[CTotalTypeSize.value];
		for (int i = 0; i < obj.types.length; ++i) {
			_values[obj.types[i]] = obj.values[i];
		}
	}

	private final int[] _values;

	public final int getGuestRequestCount() {
		return _values[CGuestRequest.value];
	}

	public final int getTribeRequestCount() {
		return _values[CTribeRequest.value];
	}

	public final int getNewMessageCount() {
		return _values[CNewMessage.value];
	}

	public final int getImLeavewordCount() {
		return _values[CImLeaveword.value];
	}

	public final int getClassInviteCount() {
		return _values[CClassInvite.value];
	}

	public final int getFriendCount() {
		return _values[CFriendCount.value];
	}

	public final int getRegApplyCount() {
		return _values[CRegApply.value];
	}

	public final int getPokeCount() {
		return _values[CPoke.value];
	}

	public final int TeamInviteCount() {
		return _values[CTeamInvite.value];
	}

	public final int getFriendMemoRequest() {
		return _values[CFriendMemoRequest.value];
	}

	public final int getRecommendFriend() {
		return _values[CRecommendedFriend.value];
	}

	public final int getEventInvite() {
		return _values[CEventInvite.value];
	}

	public final int getAppInvite() {
		return _values[CAppInvite.value];
	}

	public final int getGameInvite() {
		return _values[CGameInvite.value];
	}

	public final int getTestInvite() {
		return _values[CTestInvite.value];
	}

	public final int getSurveyInvite() {
		return _values[CSurveyInvite.value];
	}

	public final int getNotify() {
		return _values[CNotify.value];
	}

	public final int getCallInvite() {
		return _values[CCallInvite.value];
	}

	public final int getNewRequest() {
		return _values[CNewRequest.value];
	}

	public final int getContactRequest() {
		return _values[CContactRequest.value];
	}

	public final int getOfferMSNContact() {
		return _values[COfferMSNContact.value];
	}

	public final int getPhotoTagInvite() {
		return _values[CPhotoTagInvite.value];
	}

	@Override
	public String toString() {
		StringBuffer buff = new StringBuffer();
		for (int value : _values) {
			buff.append(value).append(" ");
		}
		return buff.toString();
	}
}
