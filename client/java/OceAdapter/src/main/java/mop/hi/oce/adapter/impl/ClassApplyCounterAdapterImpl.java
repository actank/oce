package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.ClassApplyCounterAdapter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.service.classapplycounter.ClassApply;
import com.xiaonei.service.classapplycounter.ClassApplyManagerPrx;
import com.xiaonei.service.classapplycounter.ClassApplyManagerPrxHelper;

public class ClassApplyCounterAdapterImpl extends ServiceAdapter implements
		ClassApplyCounterAdapter {
	private static final String endpoints = "@ClassApplyCounter";
	private ClassApplyManagerPrx manager;
	private ClassApplyManagerPrx managerOneway;

	public void acceptApply(int userId, int classId) {
		managerOneway.acceptApply(userId, classId);
	}

	public void addAdmin(int adminId, int classId) {
		managerOneway.addAdmin(adminId, classId);
	}

	public void denyApply(int userId, int classId) {
		managerOneway.denyApply(userId, classId);
	}

	public mop.hi.oce.domain.model.ClassApply[] getCounterSeq(int userId) {
		ClassApply[] caPojos = manager.getCounterSeq(userId);
		mop.hi.oce.domain.model.ClassApply[] caBeans = new mop.hi.oce.domain.model.ClassApply[caPojos.length];
		for (int i = 0; i < caPojos.length; i++) {
			caBeans[i] = new mop.hi.oce.domain.model.ClassApply(
					caPojos[i].classId, caPojos[i].count, caPojos[i].className);
		}
		return caBeans;
	}

	public void removeAdmin(int adminId, int classId) {
		managerOneway.removeAdmin(adminId, classId);
	}

	public void sendApply(int applicant, int classId) {
		managerOneway.sendApply(applicant, classId);
	}

	public void reloadClass(int classId) {
		managerOneway.reloadClass(classId);
	}

	public void reloadAdmin(int adminId) {
		managerOneway.reloadAdmin(adminId);
	}

	public ClassApplyCounterAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("M", Channel.Invoke.Twoway,
				ClassApplyManagerPrxHelper.class, 300);
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				ClassApplyManagerPrxHelper.class, 300);
	}

	public ClassApplyManagerPrx getManager() {
		return manager;
	}

	public ClassApplyManagerPrx getManagerOneway() {
		return managerOneway;
	}
}
