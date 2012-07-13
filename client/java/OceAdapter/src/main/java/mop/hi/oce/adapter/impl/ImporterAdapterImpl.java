package mop.hi.oce.adapter.impl;

import java.util.Map;

import mop.hi.oce.adapter.ImporterAdapter;
import mop.hi.oce.domain.model.importer.ImporterBuddyList;
import mop.hi.oce.domain.model.importer.ImporterOwner;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.svc.importer.ImporterManagerPrx;
import com.xiaonei.svc.importer.ImporterManagerPrxHelper;

public class ImporterAdapterImpl extends ServiceAdapter implements
		ImporterAdapter {
	private static final String endpoints = "@Importer";
	private ImporterManagerPrx manager;

	private ImporterManagerPrx managerOneway;

	public ImporterBuddyList getMSNResponse(ImporterOwner owner) {
		if (true) {
			com.xiaonei.svc.importer.BuddyList svcBuddyList = manager
					.getMSNResponse(owner.toSvcOwner());
			return new ImporterBuddyList(svcBuddyList);
		} else {
			com.xiaonei.svc.importer.BuddyList svcBuddyList = new com.xiaonei.svc.importer.BuddyList();
			svcBuddyList.owner = new com.xiaonei.svc.importer.OwnerInfo();
			svcBuddyList.owner.email = "dummy@dudu-inc.com";
			svcBuddyList.owner.password = "Dummy";

			svcBuddyList.buddies = new com.xiaonei.svc.importer.BuddyInfo[2];
			svcBuddyList.buddies[0] = new com.xiaonei.svc.importer.BuddyInfo();
			svcBuddyList.buddies[0].name = "Friend A";
			svcBuddyList.buddies[0].email = "a@dudu-inc.com";
			svcBuddyList.buddies[0].status = new com.xiaonei.svc.importer.BuddyInfoStatus();
			svcBuddyList.buddies[0].status.code = 200;
			svcBuddyList.buddies[0].status.message = "OK";
			svcBuddyList.buddies[1] = new com.xiaonei.svc.importer.BuddyInfo();
			svcBuddyList.buddies[1].name = "Friend B";
			svcBuddyList.buddies[1].email = "b@dudu-inc.com";
			svcBuddyList.buddies[1].status = new com.xiaonei.svc.importer.BuddyInfoStatus();
			svcBuddyList.buddies[1].status.code = 200;
			svcBuddyList.buddies[1].status.message = "OK";

			svcBuddyList.status = new com.xiaonei.svc.importer.BuddyListStatus();
			svcBuddyList.status.code = 200;
			svcBuddyList.status.message = "OK";

			return new ImporterBuddyList(svcBuddyList);
		}
	}

	public void sendMSNRequest(ImporterOwner owner) {
		if (true) {
			managerOneway.sendMSNRequest(owner.toSvcOwner());
		} else {

		}
	}

	public void sendGMailRequest(ImporterOwner owner) {
		if (true) {
			managerOneway.sendGMailRequest(owner.toSvcOwner());
		} else {

		}
	}

	public void sendNeteaseRequest(ImporterOwner owner) {
		if (true) {
			managerOneway.sendNeteaseRequest(owner.toSvcOwner());
		} else {

		}
	}

	public ImporterBuddyList getNeteaseResponse(ImporterOwner owner) {
		return new ImporterBuddyList(manager.getNeteaseResponse(owner
				.toSvcOwner()));
	}

	public ImporterBuddyList getGMailResponse(ImporterOwner owner) {
		return new ImporterBuddyList(manager.getGMailResponse(owner
				.toSvcOwner()));
	}

	public ImporterBuddyList getResponse(ImporterOwner owner) {
		return getMSNResponse(owner);
	}

	public void sendRequest(ImporterOwner owner) {
		sendMSNRequest(owner);
	}

	public ImporterBuddyList getCSVResponse(ImporterOwner owner) {
		com.xiaonei.svc.importer.BuddyList svcBuddyList = manager
				.getCSVResponse(owner.toSvcOwner());
		return new ImporterBuddyList(svcBuddyList);
	}

	public void sendCSVRequest(ImporterOwner owner, Map<String, String> contacts) {
		if (true) {
			managerOneway.sendCSVRequest(owner.toSvcOwner(), contacts);
		} else {

		}
	}

	public ImporterBuddyList get126Response(ImporterOwner owner) {
		return new ImporterBuddyList(manager.get126Response(owner.toSvcOwner()));
	}

	public void send126Request(ImporterOwner owner) {
		managerOneway.send126Request(owner.toSvcOwner());
	}

	public ImporterBuddyList getSinaResponse(ImporterOwner owner) {
		return new ImporterBuddyList(manager
				.getSinaResponse(owner.toSvcOwner()));
	}

	public ImporterBuddyList getYahooResponse(ImporterOwner owner) {
		return new ImporterBuddyList(manager.getYahooResponse(owner
				.toSvcOwner()));
	}

	public void sendSinaRequest(ImporterOwner owner) {
		managerOneway.sendSinaRequest(owner.toSvcOwner());
	}

	public void sendYahooRequest(ImporterOwner owner) {
		managerOneway.sendYahooRequest(owner.toSvcOwner());
	}

	public boolean verifyMSNAccount(String liveId, String livePassword) {
		return manager.verifyMSNAccount(liveId,livePassword);
	}

	public ImporterAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("M", Channel.Invoke.Twoway,
				ImporterManagerPrxHelper.class, 3000);
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				ImporterManagerPrxHelper.class, 300);
	}
}
