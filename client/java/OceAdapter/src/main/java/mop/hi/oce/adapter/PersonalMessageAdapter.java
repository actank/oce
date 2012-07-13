package mop.hi.oce.adapter;

import mop.hi.oce.domain.model.PersonalMessageEmail;
import mop.hi.oce.domain.model.PersonalMessageGossip;
import mop.hi.oce.domain.model.PersonalMessageMessage;

public interface PersonalMessageAdapter {
	public int sendGossip(PersonalMessageGossip word);

	public int sendMessage(PersonalMessageMessage msg);

	public int sendEmail(PersonalMessageEmail mail);
}
