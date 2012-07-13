package com.xiaonei.xce.windowsliveimporter;

import xce.importer.ImportState;
import xce.importer.WindowsLiveContacts;

import com.live.schema.TLiveContacts;
import com.xiaonei.xce.log.Oce;

public final class Contacts {

	public enum State {
		PENDING, AUTHING, AUTHFAILED, AUTHED, RETRIVING, RETRIVEFAILED, RETRIVED,
	};

	private State translateState(ImportState from) {
		switch (from.value()) {
		case ImportState._PENDING:
			return State.PENDING;
		case ImportState._AUTHING:
			return State.AUTHING;
		case ImportState._AUTHFAILED:
			return State.AUTHFAILED;
		case ImportState._AUTHED:
			return State.AUTHED;
		case ImportState._RETRIVING:
			return State.RETRIVING;
		case ImportState._RETRIVEFAILED:
			return State.RETRIVEFAILED;
		case ImportState._RETRIVED:
			return State.RETRIVED;
		default:
			Oce.getLogger().error(this.getClass().getName() + "translateState wrong " + from.toString());
			//System.err.println("translateState wrong " + from.toString());
			return State.PENDING;
		}
	}

	private final TLiveContacts _contacts;

	private final State _state;

	protected Contacts(WindowsLiveContacts contacts) {
		_contacts = contacts.contacts;
		_state = translateState(contacts.state);
	}

	public TLiveContacts getContacts() {
		return _contacts;
	}

	public State getState() {
		return _state;
	}
}
