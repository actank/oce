package mop.hi.svc.personalmessage;

import Ice.Current;

public class PersonalMessageManagerI extends _PersonalMessageManagerDisp {

	private static PersonalMessageManagerI _instance = new PersonalMessageManagerI();

	public static PersonalMessageManagerI getInstance() {
		return _instance;
	}

	public int sendEmail(Email mail, Current __current) {
		return EmailHelper.getInstance().sendEmail(mail);
	}

	public int sendGossip(Gossip word, Current __current) {
		return GossipHelper.getInstance().sendGossip(word);
	}

	public int sendMessage(Message msg, Current __current) {
		return MessageHelper.getInstance().sendMessage(msg);
	}

}
