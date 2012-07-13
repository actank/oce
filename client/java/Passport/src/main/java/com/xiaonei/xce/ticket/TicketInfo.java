package com.xiaonei.xce.ticket;

import mop.hi.oce.domain.Markable;

public class TicketInfo extends Markable{
	
	private static final String ID = "ID";
	private static final String REALID = "REALID";
	private static final String TYPE = "TYPE";
	private static final String IP = "IP";
	private static final String WEB = "WEB";
	private static final String WAP = "WAP";
	private static final String PLATFORM = "PLATFORM";
	private static final String CLIENT = "CLIENT";
	private static final String MOBILECLIENT = "MOBILECLIENT";
	
	public static enum Type{
		Web,Wap,PlatForm,Client,MobileClient
	}
	
	private long _id;

	private long _realId;

	private Type _type;
	
	private String _ip;
	
	public long get_id() {
		return _id;
	}

	public void set_id(long id) {
		this._id = id;
		mark(ID,String.valueOf(id));
	}
	
	public long get_realId() {
		return _realId;
	}

	public void set_realId(long realId) {
		this._realId = realId;
		mark(REALID,String.valueOf(realId));
	}

	public Type get_type() {
		return _type;
	}

	public void set_type(Type type) {
		this._type = type;
		switch(type) {
		case Web: mark(TYPE,WEB);
		break;
		case Wap: mark(TYPE,WAP);
		break;
		case PlatForm: mark(TYPE,PLATFORM);
		break;
		case Client: mark(TYPE,CLIENT);
		break;
		case MobileClient: mark(TYPE,MOBILECLIENT);
		break;
		default:
			break;
		}
	}

	public String get_ip() {
		return _ip;
	}

	public void set_ip(String ip) {
		this._ip = ip;
		mark(IP,String.valueOf(ip));
	}

}
