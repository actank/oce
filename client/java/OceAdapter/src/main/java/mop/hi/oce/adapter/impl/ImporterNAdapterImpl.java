package mop.hi.oce.adapter.impl;

import java.util.Map;
import java.util.Vector;

import mop.hi.oce.adapter.ImporterNAdapter;
import mop.hi.oce.domain.model.importer.ImporterBuddyList;
import mop.hi.oce.domain.model.importer.ImporterOwner;
import mop.hi.svc.fpt.FootprintManagerPrx;
import mop.hi.svc.passport.PassportManagerPrx;
import mop.hi.svc.passport.PassportManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.svc.importer.ImporterManagerPrx;
import com.xiaonei.svc.importer.ImporterManagerPrxHelper;

public class ImporterNAdapterImpl extends ServiceAdapter implements
		ImporterNAdapter {
	private static final String endpoints = "@Importer_";

	private ImporterManagerPrx manager;
	private ImporterManagerPrx managerOneway;
	
	private Vector<ImporterManagerPrx> managers = new Vector<ImporterManagerPrx>();
	private Vector<ImporterManagerPrx> managersOneway = new Vector<ImporterManagerPrx>();

	public ImporterBuddyList getMSNResponse(ImporterOwner owner) {
		if (true) {
			com.xiaonei.svc.importer.BuddyList svcBuddyList = getManager(owner.userId)
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
//			managerOneway.sendMSNRequest(owner.toSvcOwner());
			getManagerOneway(owner.userId).sendMSNRequest(owner.toSvcOwner());
		} else {

		}
	}

	public void sendGMailRequest(ImporterOwner owner) {
		if (true) {
//			managerOneway.sendGMailRequest(owner.toSvcOwner());
			getManagerOneway(owner.userId).sendGMailRequest(owner.toSvcOwner());
		} else {

		}
	}

	public void sendNeteaseRequest(ImporterOwner owner) {
		if (true) {
//			managerOneway.sendNeteaseRequest(owner.toSvcOwner());
			getManagerOneway(owner.userId).sendNeteaseRequest(owner.toSvcOwner());
		} else {

		}
	}

	public ImporterBuddyList getNeteaseResponse(ImporterOwner owner) {
//		return new ImporterBuddyList(manager.getNeteaseResponse(owner
//				.toSvcOwner()));
		return new ImporterBuddyList(getManager(owner.userId).getNeteaseResponse(owner
				.toSvcOwner()));
	}

	public ImporterBuddyList getGMailResponse(ImporterOwner owner) {
//		return new ImporterBuddyList(manager.getGMailResponse(owner
//				.toSvcOwner()));
		return new ImporterBuddyList(getManager(owner.userId).getGMailResponse(owner
				.toSvcOwner()));
	}

	public ImporterBuddyList getResponse(ImporterOwner owner) {
		return getMSNResponse(owner);
	}

	public void sendRequest(ImporterOwner owner) {
		sendMSNRequest(owner);
	}

	public ImporterBuddyList getCSVResponse(ImporterOwner owner) {
//		com.xiaonei.svc.importer.BuddyList svcBuddyList = manager
//		.getCSVResponse(owner.toSvcOwner());
		com.xiaonei.svc.importer.BuddyList svcBuddyList = getManager(owner.userId)
		.getCSVResponse(owner.toSvcOwner());
		
		return new ImporterBuddyList(svcBuddyList);
	}

	public void sendCSVRequest(ImporterOwner owner, Map<String, String> contacts) {
		if (true) {
//			managerOneway.sendCSVRequest(owner.toSvcOwner(), contacts);
			getManagerOneway(owner.userId).sendCSVRequest(owner.toSvcOwner(), contacts);
		} else {

		}
	}

	public ImporterBuddyList get126Response(ImporterOwner owner) {
//		return new ImporterBuddyList(manager.get126Response(owner.toSvcOwner()));
		return new ImporterBuddyList(getManager(owner.userId).get126Response(owner.toSvcOwner()));
	}

	public void send126Request(ImporterOwner owner) {
//		managerOneway.send126Request(owner.toSvcOwner());
		getManagerOneway(owner.userId).send126Request(owner.toSvcOwner());
	}

	public ImporterBuddyList getSinaResponse(ImporterOwner owner) {
//		return new ImporterBuddyList(manager
//				.getSinaResponse(owner.toSvcOwner()));
		return new ImporterBuddyList(getManager(owner.userId)
				.getSinaResponse(owner.toSvcOwner()));
	}

	public ImporterBuddyList getYahooResponse(ImporterOwner owner) {
//		return new ImporterBuddyList(manager.getYahooResponse(owner
//				.toSvcOwner()));
		return new ImporterBuddyList(getManager(owner.userId).getYahooResponse(owner
				.toSvcOwner()));
	}

	public void sendSinaRequest(ImporterOwner owner) {
//		managerOneway.sendSinaRequest(owner.toSvcOwner());
		getManagerOneway(owner.userId).sendSinaRequest(owner.toSvcOwner());
	}

	public void sendYahooRequest(ImporterOwner owner) {
//		managerOneway.sendYahooRequest(owner.toSvcOwner());
		getManagerOneway(owner.userId).sendYahooRequest(owner.toSvcOwner());
	}

	public boolean verifyMSNAccount(String liveId, String livePassword) {
//		return manager.verifyMSNAccount(liveId,livePassword);
		return manager.verifyMSNAccount(liveId,livePassword);
	}

	public ImporterManagerPrx getManager(int userId) {
		return locateProxy(managers, "M", userId,
				Channel.Invoke.Twoway, ImporterManagerPrxHelper.class, 300000);
	}

	public ImporterManagerPrx getManagerOneway(int userId) {
		return locateProxy(managersOneway, "M", userId,
				Channel.Invoke.Oneway, ImporterManagerPrxHelper.class, 300);
	}
	
	public ImporterNAdapterImpl() {
		super(endpoints, 30, Channel.newImporterChannel(endpoints));
		manager = locateProxy("M", Channel.Invoke.Twoway,
				ImporterManagerPrxHelper.class, 300000);
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				ImporterManagerPrxHelper.class, 3000);
	}
	
	public static void main(String[] args) {
		// ImporterOwner owner = new ImporterOwner(222525090, "han.zhang@opi-corp.com", "saladin");		
		ImporterOwner owner = new ImporterOwner(222525090, "tissy@sina.com", "1qaz2wsx");		
		ImporterAdapterImpl _adapter = new ImporterAdapterImpl();
		_adapter.sendMSNRequest(owner);
		
		try {
			Thread.sleep(10000);
		} catch (Exception e) {
			e.printStackTrace();
		}	
		
		ImporterBuddyList list = _adapter.getMSNResponse(owner);
		System.out.println(list.users.size());
		System.out.println(list.buddies.length);
		System.out.println(list.status.message);
		System.out.println(list.status.code);
		System.exit(0);
		
	}
}
