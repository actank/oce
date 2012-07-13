package com.xiaonei.xce.personalinfo;

import com.xiaonei.xce.log.Oce;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.personalinfo.LinkViewReaderPrx;
import xce.personalinfo.LinkViewReaderPrxHelper;

final class LinkViewReaderAdapter<T extends LinkViewInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerLinkViewReader";
	private static int _interval = 120;
	private LinkViewInfoFactory<T> _factory;
	
	public LinkViewReaderAdapter(LinkViewInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public LinkViewInfo getLinkView(int userId) throws Exception {
		LinkViewInfo result;
		LinkViewReaderPrx prx = null;
		result = _factory.create();
		if (userId <= 0) {
			return result;
		}
		try {
			prx = getLinkViewReaderPrx(userId);
			result.parse(prx.getLinkView(userId));
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getLinkView [" + prx + "] userId=" + userId + " " + e.toString());
			throw e;
		}
		return result;
	}
	
	public LinkViewInfo getLinkViewSync(int userId) throws Exception {
		LinkViewInfo result;
		LinkViewReaderPrx prx = null;
		result = _factory.create();
		if (userId <= 0) {
			return result;
		}
		try {
			prx = getLinkViewReader3sPrx(userId);
			result.parse(prx.getLinkViewSync(userId));
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getLinkView [" + prx + "] userId=" + userId + " " + e.toString());
			throw e;
		}
		return result;
	}
	
	private LinkViewReaderPrx getLinkViewReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			LinkViewReaderPrx prx = LinkViewReaderPrxHelper
				.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private LinkViewReaderPrx getLinkViewReader3sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			LinkViewReaderPrx prx = LinkViewReaderPrxHelper
				.uncheckedCast(prx0.ice_timeout(3000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		int userId = 35178;
		LinkViewReaderAdapter<DefaultLinkViewInfo> adapter
			= new LinkViewReaderAdapter<DefaultLinkViewInfo>(new DefaultLinkViewInfoFactory());

		try {
			LinkViewInfo linkViewInfo = adapter.getLinkView(userId);
			System.out.println("getLinkView(" + userId + ") = " + linkViewInfo);
		} catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Ok");
		System.exit(0);
	}
}
