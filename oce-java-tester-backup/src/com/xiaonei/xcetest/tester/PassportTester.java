package com.xiaonei.xcetest.tester;

import java.util.Date;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;

public class PassportTester extends TestCase {
	public void testPassport() throws Exception {
		String ticket = AdapterFactory.getPassportAdapter().createTicket(
				142443436, false);
		// String ticket = "6f8abeeb2b340d9c531a6b7e8b9460e1";
		System.out.println(AdapterFactory.getPassportAdapter().verifyTicket(
				ticket));
		// while (true) {
		// super.assertEquals(227366242,
		// AdapterFactory.getPassportAdapter().verifyTicket(ticket).intValue());
		// System.out.println(227366242 + " -> " + ticket);
		// Thread.sleep(1000);
		// }
		// String[] items = {
		// "7ed65f6e3f35a50c42967fd1c5869b60|hostid:228669683" };
		// for (int i = 0; i < items.length; ++i) {
		// String item = items[i];
		// String userId = item.substring(item.indexOf(":") + 1);
		// String ticket = item.substring(0, item.indexOf("|"));
		// System.out.println("Testing " + ticket + " -> " + userId + " ? ");
		// System.out.println("UserId about " + ticket + " -> "
		// + AdapterFactory.getPassportAdapter().verifyTicket(ticket));
		// System.out.println("Ticket about " + userId + " -> "
		// +
		// AdapterFactory.getPassportAdapter().queryTicket(Integer.parseInt(userId)));
		// System.out.println("--------------------------------------------------------\n");
		// }
		// System.out.println("UserId: "
		// + AdapterFactory.getPassportAdapter().verifyTicket(
		// "802fc7987811434ece50694e3c8595dd"));
		// System.out.println("Ticket: "
		// + AdapterFactory.getPassportAdapter().queryTicket(200556978));
	}

	public void testCreatePersistent() {
		int userId = (int) Math.round(Math.random() * Integer.MAX_VALUE);
		System.out.println(AdapterFactory.getPassportAdapter().createTicket(
				userId, true));
	}

	public void testCreateNormal() {
		int userId = (int) Math.round(Math.random() * Integer.MAX_VALUE);
		System.out.println(AdapterFactory.getPassportAdapter().createTicket(
				userId, false));
	}

	public void testVerify() {
		// String ticket=
		// AdapterFactory.getPassportAdapter().createTicket(userId, true);
		String ticket = "a8ad28e63a3a26ce1f8d2e298c3c59c2";
		// while(true){
		System.out.println(AdapterFactory.getPassportAdapter().verifyTicket(
				ticket));
		// }

	}

	public void testQuery() {
		int id = 79512511;
		System.out.println(AdapterFactory.getPassportAdapter().queryTicket(id));
	}

	public void destoryTicket() {
		String ticket = "8fc700dd6681809c8e4623f960121acd";
		AdapterFactory.getPassportAdapter().destroyTicket(ticket);
	}

	public void impersistentTicket() {
		String ticket = "4b568fef90ddd917de5c9bca34966446";
		AdapterFactory.getPassportAdapter().impersistence(ticket);
	}

	public static void main(String[] args) throws Exception {
		String ticket = AdapterFactory.getPassportAdapter().createTicket(
				142443436, false);
		System.out.println(ticket);
		System.in.read();
		for (int i = 0; i < 100; ++i) {
			new VerifyThread(ticket).start();
		}
		while (true) {
			try {
				Thread.sleep(1000);
			} catch (Exception e) {
				
			}
		}
//		for (int i = 0;; i++) {
//			Thread.sleep(1000);
//			int userId = 10020 + i;
//			try {
//				System.out.println(new Date() + "\t" + userId + "\t"
//						+ AdapterFactory.getPassportAdapter().queryTicket(userId));
//			} catch (Ice.TimeoutException e) {
//				System.out.println(new Date() + "\t" + userId + "\tTimeout");
//			} catch (Exception e) {
//				System.out.println(new Date() + "\t" + userId + "\tException");
//			}
//		}
	}
}
class VerifyThread extends Thread {
	public VerifyThread(String ticket) {
		this.ticket = ticket;
	}

	public void run() {
		while (true) {
			try {
				AdapterFactory.getPassportAdapter().verifyTicket(ticket);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private String ticket;
}