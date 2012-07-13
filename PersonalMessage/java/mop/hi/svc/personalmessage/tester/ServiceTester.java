package mop.hi.svc.personalmessage.tester;

import junit.framework.TestCase;
import mop.hi.svc.personalmessage.Email;
import mop.hi.svc.personalmessage.Gossip;
import mop.hi.svc.personalmessage.Message;
import mop.hi.svc.personalmessage.PersonalMessageManagerPrx;
import mop.hi.svc.personalmessage.PersonalMessageManagerPrxHelper;
import Ice.Communicator;
import Ice.InitializationData;
import Ice.Properties;
import Ice.Util;

public class ServiceTester extends TestCase {

	@Override
	protected void setUp() throws Exception {
		Properties properties = Util.createProperties();
		properties.setProperty("Ice.Override.Timeout", String.valueOf(2 * 1000));
		properties.setProperty("Ice.Override.ConnectTimeout", String.valueOf(2 * 1000));
		properties.setProperty("IceGrid.InstanceName", "XiaoNei");

		properties.setProperty("Ice.ThreadPool.Client.Size", "1");
		properties.setProperty("Ice.ThreadPool.Client.SizeMax", "100");
		properties.setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		String locator = "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002";
		properties.setProperty("Ice.Default.Locator", locator);
		InitializationData initData = new InitializationData();
		initData.properties = properties;
		_ic = Ice.Util.initialize(initData);
		_manager = PersonalMessageManagerPrxHelper.checkedCast(_ic.stringToProxy("M@PersonalMessage"));
	}

	private Communicator _ic;
	private PersonalMessageManagerPrx _manager;

	public void testEmail() {
		Email email = new Email();
		email.from = "FROM";
		email.to = new String[] { "TO" };
		email.cc = new String[] { "CC" };
		email.bcc = new String[] { "BCC" };
		email.subject = "SUBJ";
		email.body = "BODY";
		email.isHtml = false;

		_manager.sendEmail(email);
	}

	public void testGossip() {
		Gossip word = new Gossip();
		word.owner = 200353733;
		word.sender = 200353733;
		word.senderName = "bbc";
		word.senderUniv = "1001";
		word.body = "留言内容";
		word.tinyurl = "http://head.xiaonei.com/photos/20070411/0950/tiny55915.jpg";
		word.time = (int) (System.currentTimeMillis() / 1000L);

		_manager.sendGossip(word);
	}

	public void testMessage() {
		Message msg = new Message();
		msg.body = "OBDY";
		msg.saveInSourceOutbox = true;
		msg.saveInTargetInbox = true;
		msg.source = 200353733;
		msg.sourceName = "我";
		msg.subject = "HELLO";
		msg.target = 202911262;
		msg.targetName = "还是我";
		msg.time = (int) (System.currentTimeMillis() / 1000);
		msg.type = 0;
		_manager.sendMessage(msg);
	}
}
