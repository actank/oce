package com.xiaonei.xce.ticket;

import java.util.List;

import com.xiaonei.xce.ticket.TicketInfo.Type;

public class TemporaryTicketImpl implements Ticket {

	@Override
	public String createTicket(TicketInfo info, long expiredTime) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public long verifyTicket(String ticket, List<Type> types) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public boolean destroyTicket(String ticket) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public TicketInfo getTicketInfo(String ticket) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean destroyTicketById(long id, Type type) {
		// TODO Auto-generated method stub
		return false;
	}

}
