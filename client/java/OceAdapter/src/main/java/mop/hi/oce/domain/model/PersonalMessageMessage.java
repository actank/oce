package mop.hi.oce.domain.model;

import java.util.Date;

import mop.hi.svc.personalmessage.Message;

public class PersonalMessageMessage {

	private Message _svcModel = new Message();

	public PersonalMessageMessage() {
		setTime(new Date());
		setSaveInSourceOutbox(false);
		setSaveInTargetInbox(true);
	}

	public Message getSvcModel() {
		return _svcModel;
	}

	public String getBody() {
		return _svcModel.body;
	}

	public void setBody(String body) {
		_svcModel.body = body;
	}

	public boolean isSaveInSourceOutbox() {
		return _svcModel.saveInSourceOutbox;
	}

	public void setSaveInSourceOutbox(boolean saveInSourceOutbox) {
		_svcModel.saveInSourceOutbox = saveInSourceOutbox;
	}

	public boolean isSaveInTargetInbox() {
		return _svcModel.saveInTargetInbox;
	}

	public void setSaveInTargetInbox(boolean saveInTargetInbox) {
		_svcModel.saveInTargetInbox = saveInTargetInbox;
	}

	public int getSource() {
		return _svcModel.source;
	}

	public void setSource(int source) {
		_svcModel.source = source;
	}

	public String getSourceName() {
		return _svcModel.sourceName;
	}

	public void setSourceName(String sourceName) {
		_svcModel.sourceName = sourceName;
	}

	public String getSubject() {
		return _svcModel.subject;
	}

	public void setSubject(String subject) {
		_svcModel.subject = subject;
	}

	public int getTarget() {
		return _svcModel.target;
	}

	public void setTarget(int target) {
		_svcModel.target = target;
	}

	public String getTargetName() {
		return _svcModel.targetName;
	}

	public void setTargetName(String targetName) {
		_svcModel.targetName = targetName;
	}

	public Date getTime() {
		return new Date(_svcModel.time * 1000L);
	}

	private void setTime(Date time) {
		_svcModel.time = (int) (time.getTime() / 1000L);
	}

	public int getType() {
		return _svcModel.type;
	}

	public void setType(int type) {
		_svcModel.type = type;
	}

}
