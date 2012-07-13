package com.xiaonei.xce.personalinfo;

import com.xiaonei.xce.log.Oce;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.personalinfo.Link2UserIdReaderPrx;
import xce.personalinfo.Link2UserIdReaderPrxHelper;

final class Link2UserIdReaderAdapter<T extends LinkViewInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerLink2UserIdReader";
	private static int _interval = 120;
	private LinkViewInfoFactory<T> _factory;

	public Link2UserIdReaderAdapter(LinkViewInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public LinkViewInfo getLinkView(String link) throws Exception {
		LinkViewInfo result;
		Link2UserIdReaderPrx prx = null;
		result = _factory.create();
		if (link == null || link == "") {
			return result;
		}
		try {
			prx = getLink2UserIdReaderPrx(0);
			result.parse(prx.getLinkView(link));
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getLinkView [" + prx + "] link=" + link + " " + e.toString());
			throw e;
		}
		return result;
	}
	
	public LinkViewInfo getLinkViewSync(String link) throws Exception {
		LinkViewInfo result;
		Link2UserIdReaderPrx prx = null;
		result = _factory.create();
		if (link == "" || link == "") {
			return result;
		}
		try {
			prx = getLink2UserIdReader3sPrx(0);
			result.parse(prx.getLinkViewSync(link));
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getLinkViewSync [" + prx + "] link=" + link + " " + e.toString());
			throw e;
		}
		return result;
	}

	public int getUserId(String link) throws Exception {
		int result = 0;
		Link2UserIdReaderPrx prx = null;
		if (link == null || link == "") {
			return result;
		}
		try {
			prx = getLink2UserIdReaderPrx(0);
			result = prx.find(link);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getUserId [" + prx + "] link=" + link + " " + e.toString());
			throw e;
		}
		return result;
	}
	
	public int getUserIdSync(String link) throws Exception {
		int result = 0;
		Link2UserIdReaderPrx prx = null;
		if (link == null || link == "") {
			return result;
		}
		try {
			prx = getLink2UserIdReader3sPrx(0);
			result = prx.locate(link);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() 
				+ ".getUserIdSync [" + prx + "] link=" + link + " " + e.toString());
			throw e;
		}
		return result;
	}	

	private Link2UserIdReaderPrx getLink2UserIdReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			Link2UserIdReaderPrx prx = Link2UserIdReaderPrxHelper
				.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	private Link2UserIdReaderPrx getLink2UserIdReader3sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			Link2UserIdReaderPrx prx = Link2UserIdReaderPrxHelper
				.uncheckedCast(prx0.ice_timeout(10000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		String link = "joyeru";
		Link2UserIdReaderAdapter<DefaultLinkViewInfo> adapter
			= new Link2UserIdReaderAdapter<DefaultLinkViewInfo>(new DefaultLinkViewInfoFactory());

		try {
			LinkViewInfo linkViewInfo = adapter.getLinkView(link);
			System.out.println("getLinkView(\"" + link + "\") = " + linkViewInfo);
			int userId = adapter.getUserIdSync(link);
			System.out.println("getUserId(\"" + link + "\") = " + userId);
		} catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Ok");
		System.exit(0);
	}
}
