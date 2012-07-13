package com.xiaonei.xce.passport;

import passport.TransferManagerPrx;
import passport.TransferManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractRandomProxyHolder;
import Ice.ObjectPrx;
import xce.clusterstate.ReplicatedClusterMCAdapter;

public class TransferImpl implements Transfer {

	private static TransferImpl _instance = new TransferImpl();

	public static TransferImpl getInstance() {
		return _instance;
	}

	private class TransferManagerHolder extends ReplicatedClusterMCAdapter {

		private final static String ctr_endpoints = "ControllerTransfer";
		private static final int _interval = 120;
		
		public TransferManagerHolder() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}

		public TransferManagerPrx getProxy(){
	            Ice.ObjectPrx prx0 = getValidProxy(0, -1);
	            if (prx0 != null) {
	            		TransferManagerPrx prx = TransferManagerPrxHelper
	                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(-1));
	                    return prx;
	            } else {
	                    return null;
	            }
		}

	}
	
	private TransferImpl() {

	}
	
	private TransferManagerHolder _holder = new TransferManagerHolder();
	
	@Override
	public String createTicket(int userId, int time) {
		return _holder.getProxy().createTicket(userId, time);
	}

	@Override
	public int verifyTicket(String ticket) {
		return _holder.getProxy().verifyTicket(ticket);
	}

}
