package com.xiaonei.talk;

import java.util.Vector;

import talk.wevent.WEventManagerPrx;
import talk.wevent.WEventManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class MessageHandlerAdapter extends ServiceAdapter {
	private static final String endpoints = "@MessageMsgHandler";
	private Vector<MessageHandlerPrx> managersOneway = new Vector<MessageHandlerPrx>();;

	private static MessageHandlerAdapter instance = new MessageHandlerAdapter();

	public static MessageHandlerAdapter instance() {
		return instance;
	}

	MessageHandlerAdapter() {
		super(endpoints, 10, Channel.newTalkChannel(endpoints));

	}
	
	public MessageHandlerPrx getManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Oneway,
				MessageHandlerPrxHelper.class, 300);
	}
	
	public void sendMessage(int from , int to, String msg){
		getManagerOneway(from).sendMessage(from ,to ,msg);
	}

}