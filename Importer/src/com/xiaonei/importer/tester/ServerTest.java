package com.xiaonei.importer.tester;

import org.dom4j.DocumentException;

import xce.util.channel.Channel;
import junit.framework.TestCase;

import com.xiaonei.importer.WindowsLive.WindowsLiveProtocal;
import com.xiaonei.importer.netease.NeteaseWorker;
import com.xiaonei.importer.service.ImporterManagerI;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.ImporterManagerPrx;
import com.xiaonei.svc.importer.ImporterManagerPrxHelper;
import com.xiaonei.svc.importer.OwnerInfo;

public class ServerTest extends TestCase {
	static private ImporterManagerI _manager_local=new ImporterManagerI();
	static private ImporterManagerPrx _manager_remote= ImporterManagerPrxHelper.uncheckedCast(Channel.newOceChannel("Importer_212").getCommunicator().stringToProxy("M@Importer"));

	@Override
	protected void setUp() throws Exception {
	}
	
	public void testVerifyMSNAccount() throws Throwable {
		String email = "";
		String password = "";
		
		try {
			System.out.println(_manager_local.verifyMSNAccount(email, password));
		} catch(Exception e) {
			e.printStackTrace();
			return;
		}
	}
	
	public static void main(String[] args) {
		
		String email = "";
		String password = "";
		int userId = 0;
	
		try {
			_manager_local.sendNeteaseRequest(new OwnerInfo(userId, email, password));
		} catch(Exception e) {
			e.printStackTrace();
			return;
		}
		for (int i = 0; ; ++i) {
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			BuddyList result = _manager_local.getNeteaseResponse(new OwnerInfo(userId, email, password));
			System.out.println(result.status.code + " " + result.status.message);
			for (BuddyInfo buddyInfo : result.buddies) {
				System.out.println(buddyInfo.name + ", " + buddyInfo.email + ", " + buddyInfo.status.message);
			}
			System.err.println("#################################################################");
			if (result.status.code >= 200 && result.status.code < 300) {
				break;
			}
		}
	}

	private static final String _response = "<?xml version=\"1.0\" encoding=\"utf-8\"?>" + "<Contacts>" + "	<Contact>"
			+ "		<ID>369d8f9c-8324-4c44-8828-e1e8101417e2</ID>"
			+ "		<WindowsLiveID>bochun.bai@dudu-inc.com</WindowsLiveID>"
			+ "		<AutoUpdateEnabled>true</AutoUpdateEnabled>" + "		<AutoUpdateStatus>Live</AutoUpdateStatus>"
			+ "		<Profiles>" + "			<Personal>" + "				<FirstName>伯纯</FirstName>" + "				<LastName>白</LastName>"
			+ "			</Personal>" + "		</Profiles>" + "		<Emails>" + "			<Email>" + "				<ID>8</ID>"
			+ "				<EmailType>WindowsLiveID</EmailType>" + "				<Address>bochun.bai@dudu-inc.com</Address>"
			+ "				<IsIMEnabled>true</IsIMEnabled>" + "				<IsDefault>false</IsDefault>" + "			</Email>"
			+ "		</Emails>" + "	</Contact>" + "</Contacts>";

	public void testParser() throws DocumentException {
		BuddyInfo[] buddies = WindowsLiveProtocal.getBuddInfos(_response);
		for (BuddyInfo buddy : buddies) {
			System.out.println(buddy.name + " # " + buddy.email);
		}
	}
}
