package mop.hi.oce.domain.model;

import java.util.Date;

import mop.hi.svc.personalmessage.Gossip;

public class PersonalMessageGossip {
	private Gossip _svcModel = new Gossip();

	public PersonalMessageGossip() {
		setTime(new Date());
	}

	public int getSender() {
		return _svcModel.sender;
	}

	public void setSender(int sender) {
		_svcModel.sender = sender;
	}

	public String getSenderName() {
		return _svcModel.senderName;
	}

	public void setSenderName(String senderName) {
		_svcModel.senderName = senderName;
	}

	public String getSenderUniv() {
		return _svcModel.senderUniv;
	}

	public void setSenderUniv(String senderUniv) {
		_svcModel.senderUniv = senderUniv;
	}

	public int getOwner() {
		return _svcModel.owner;
	}

	public void setOwner(int owner) {
		_svcModel.owner = owner;
	}

	public String getTinyurl() {
		return _svcModel.tinyurl;
	}

	public void setTinyurl(String tinyurl) {
		_svcModel.tinyurl = tinyurl;
	}

	public String getBody() {
		return _svcModel.body;
	}

	public void setBody(String body) {
		_svcModel.body = body;
	}

	public Date getTime() {
		return new Date(_svcModel.time * 1000L);
	}

	private void setTime(Date time) {
		_svcModel.time = (int) (time.getTime() / 1000L);
	}

	public Gossip getSvcModel() {
		return _svcModel;
	}

}
