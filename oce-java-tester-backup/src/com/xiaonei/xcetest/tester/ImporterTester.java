package com.xiaonei.xcetest.tester;

import java.util.HashMap;
import java.util.Map;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.model.importer.ImporterBuddyInfo;
import mop.hi.oce.domain.model.importer.ImporterBuddyList;
import mop.hi.oce.domain.model.importer.ImporterOwner;

public class ImporterTester extends TestCase {
	public void testDummy() throws InterruptedException {

		ImporterOwner owner = new ImporterOwner(1, "cpl_2398@hotmail.com",
				"2211418");
		System.err.println(owner.email + ", " + owner.password);
		AdapterFactory.getImporterAdapter().sendMSNRequest(owner);

		for (; ; ) {
			ImporterBuddyList result = AdapterFactory.getImporterAdapter()
					.getMSNResponse(owner);
			System.err.println(result.status.code + " "
					+ result.status.message);
			if (result.status.code >= 200 && result.status.code < 300) {
				for (ImporterBuddyInfo info : result.buddies) {
					System.out.println(info.name + " " + info.email);
				}
				//break;
			} else {

			}
			Thread.sleep(1000);
		}
	}
	
	public void test126() throws InterruptedException {

		ImporterOwner owner = new ImporterOwner(1, "kangweino1",
				"kangwei");
		AdapterFactory.getImporterAdapter().send126Request(owner);

		for (; ; ) {
			ImporterBuddyList result = AdapterFactory.getImporterAdapter()
					.get126Response(owner);
			System.err.println(result.status.code + " "
					+ result.status.message);
			if (result.status.code >= 200 && result.status.code < 300) {
				for (ImporterBuddyInfo info : result.buddies) {
					System.out.println(info.name + " " + info.email);
				}
				//break;
			} else {

			}
			Thread.sleep(1000);
		}
	}
	
	public void testSina() throws InterruptedException {

		ImporterOwner owner = new ImporterOwner(1, "xntest",
				"test123");
		AdapterFactory.getImporterAdapter().sendSinaRequest(owner);

		for (; ; ) {
			ImporterBuddyList result = AdapterFactory.getImporterAdapter()
					.getSinaResponse(owner);
			System.err.println(result.status.code + " "
					+ result.status.message);
			if (result.status.code >= 200 && result.status.code < 300) {
				for (ImporterBuddyInfo info : result.buddies) {
					System.out.println(info.name + " " + info.email);
				}
				//break;
			} else {

			}
			Thread.sleep(1000);
		}
	}

	public void testGmail() throws InterruptedException {
		while (true) {
			try {
				ImporterOwner owner = new ImporterOwner(59967982, "importetxn",
						"123test456");
				AdapterFactory.getImporterAdapter().sendGMailRequest(owner);

				for (int i = 0; i < 50; ++i) {
					ImporterBuddyList result = AdapterFactory
							.getImporterAdapter().getGMailResponse(owner);
					System.err.println(result.status.code + " "
							+ result.status.message);
					if (result.status.code >= 200 && result.status.code < 300) {
						for (ImporterBuddyInfo info : result.buddies) {
							System.out.println(info.name + " " + info.email);
						}
						//break;
					} else {

					}
					
					Thread.sleep(1000);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public void testCSV() throws InterruptedException {
		ImporterOwner owner = new ImporterOwner(202911262,
				"xiaonei-test@hotmail.com", "tester!");
		Map<String, String> contacts = new HashMap<String, String>();
		contacts.put("baibochun@dudu-inc.com", "dudu");
		contacts.put("baibochun@msn.com", "msn");
		AdapterFactory.getImporterAdapter().sendCSVRequest(owner, contacts);
		for (int i = 0; i < 50; ++i) {
			try {
				ImporterBuddyList result = AdapterFactory.getImporterAdapter()
						.getCSVResponse(owner);
				if (result.status.code >= 200 && result.status.code < 300) {
					for (ImporterBuddyInfo info : result.buddies) {
						System.out.println(info.name + " " + info.email + " "
								+ info.getStatus().code + " "
								+ info.getStatus().message);
						if (info.getStatus().code == 1) {
							System.out
									.println(result.users.get(info.email).userId
											+ "& "
											+ result.users.get(info.email).userName);
						}
					}
					// break;
				} else {
					System.err.println(result.status.code + "# "
							+ result.status.message);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
			Thread.sleep(1000);
		}
	}
}
