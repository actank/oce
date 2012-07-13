package com.xiaonei.xce.personalinfo;

import com.xiaonei.xce.log.Oce;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.personalinfo.PersonalInfoWriterPrx;
import xce.personalinfo.PersonalInfoWriterPrxHelper;

final class PersonalInfoWriterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerPersonalInfoWriter";
	private static int _interval = 120;

	public PersonalInfoWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	public void createPersonalInfo(int userId, PersonalInfo info) throws Exception {
		PersonalInfoWriterPrx prx = null;
		if (userId <= 0) {
			return;
		}
		try {
			prx = getPersonalInfoWriter10sPrx(userId);
			prx.createPersonalInfo(info.demark());
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".createPersonalInfo [" + prx + "] userId=" + userId + " " + e.toString());
			throw e;
		}
	}
	
	public void setPersonalInfo(int userId, PersonalInfo info) throws Exception {
		PersonalInfoWriterPrx prx = null;
		if (userId <= 0) {
			return;
		}
		try {
			prx = getPersonalInfoWriterPrx(userId);
			prx.setPersonalInfo(userId, info.demark());
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".setPersonalInfo [" + prx + "] userId=" + userId + " " + e.toString());
			throw e;
		}
	}

	private PersonalInfoWriterPrx getPersonalInfoWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			PersonalInfoWriterPrx prx = PersonalInfoWriterPrxHelper
				.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private PersonalInfoWriterPrx getPersonalInfoWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			PersonalInfoWriterPrx prx = PersonalInfoWriterPrxHelper
				.uncheckedCast(prx0.ice_timeout(10000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		int userId = 35178;
		String link = "joyeru";
		PersonalInfoWriterAdapter adapter = new PersonalInfoWriterAdapter();
		PersonalInfo info = new PersonalInfo();
		info.setLink(link);
		try {
			adapter.setPersonalInfo(userId, info);
		} catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Ok");
		System.exit(0);
	}
}
