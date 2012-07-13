package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.model.PersonalMessageGossip;
import mop.hi.oce.domain.model.PersonalMessageMessage;

public class PersonalMessageTester extends TestCase {
	public void testGossip(){
		PersonalMessageGossip gossip = new PersonalMessageGossip();
		gossip.setOwner(1003);
		gossip.setBody("BODY");
		gossip.setSender(22);
		gossip.setSenderName("senderName");
		gossip.setTinyurl("tinyurl");
		AdapterFactory.getPersonalMessageAdapter().sendGossip(gossip);
	}
	
	public void testMessage(){
		PersonalMessageMessage msg = new PersonalMessageMessage();
		msg.setBody("BBB");
		msg.setSaveInSourceOutbox(true);
		msg.setSaveInTargetInbox(true);
		msg.setSource(200353733);
		msg.setSourceName("33_TO_62");
		msg.setSubject("CCC");
		msg.setTarget(202911262);
		msg.setTargetName("62_IN_33");
		msg.setType(0);
		AdapterFactory.getPersonalMessageAdapter().sendMessage(msg);
	}
}
