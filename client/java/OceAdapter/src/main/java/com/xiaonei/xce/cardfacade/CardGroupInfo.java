package com.xiaonei.xce.cardfacade;

import java.util.ArrayList;
import java.util.List;

import xce.cardcache.PartInfo;
import xce.cardcache.PartNewInfo;

public class CardGroupInfo {
	private List<CardShowInfo> exchangeInfos;
	private List<CardShowInfo> addInfos;
	private List<CardShowInfo> receiveInfos;
	private List<CardShowInfo> friendInfos;
	private List<CardShowInfo> importInfos;

	public static List<CardShowInfo> convert(PartInfo[] infos, int type) {
		List<CardShowInfo> shows = new ArrayList<CardShowInfo>(infos.length);
		for (PartInfo info : infos) {
			CardShowInfo show = new CardShowInfo(info, type);
			shows.add(show);
		}
		return shows;
	}

	public static List<CardShowInfo> convert(PartNewInfo[] infos) {
		List<CardShowInfo> shows = new ArrayList<CardShowInfo>(infos.length);
		for (PartNewInfo info : infos) {
			CardShowInfo show = new CardShowInfo(info);
			shows.add(show);
		}
		return shows;
	}

	public List<CardShowInfo> getFriendInfos() {
		return friendInfos;
	}

	public void setFriendInfos(List<CardShowInfo> friendInfos) {
		this.friendInfos = friendInfos;
	}

	public List<CardShowInfo> getImportInfos() {
		return importInfos;
	}

	public void setImportInfos(List<CardShowInfo> importInfos) {
		this.importInfos = importInfos;
	}

	public void setExchangeInfos(List<CardShowInfo> exchangeInfos) {
		this.exchangeInfos = exchangeInfos;
	}

	public List<CardShowInfo> getExchangeInfos() {
		return exchangeInfos;
	}

	public void setReceiveInfos(List<CardShowInfo> receiveInfos) {
		this.receiveInfos = receiveInfos;
	}

	public List<CardShowInfo> getReceiveInfos() {
		return receiveInfos;
	}

	public void setAddInfos(List<CardShowInfo> addInfos) {
		this.addInfos = addInfos;
	}

	public List<CardShowInfo> getAddInfos() {
		return addInfos;
	}
}
