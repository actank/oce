package mop.hi.oce.adapter.impl;

import java.util.Vector;

import mop.hi.oce.adapter.AntispamAdapter;
import mop.hi.svc.spam.AntispamManagerPrx;
import mop.hi.svc.spam.AntispamManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class AntispamAdapterImpl extends ServiceAdapter implements
		AntispamAdapter {
	private static final String endpoints = "@Antispam";
	private Vector<AntispamManagerPrx> managers = new Vector<AntispamManagerPrx>();
	private Vector<AntispamManagerPrx> managersOneway = new Vector<AntispamManagerPrx>();

	public AntispamAdapterImpl() {
		super(endpoints, 1, Channel.newOceChannel(endpoints));
	}

	public void addNewSpam(String text) {
		getAntispamManagerOneway(Math.abs(text.hashCode())).addNewSpam(text);
	}

	/**
	 * 0表示不禁止，1表示禁止Spam检查，2表示禁止频率检查和Spam检查
	 */
	public void stopCheck(int serverId, int stat) {
		getAntispamManagerOneway(serverId).stopCheck(stat);
	}

	public int check(int userid, String text) {
		String tmp = text.replaceAll(" ", "");
		return getAntispamManager(userid).check(userid, text, tmp.hashCode());
	}

	public void changeRepeatslot(int slot) {
		getAntispamManagerOneway(0).changeRepeatslot(slot);
	}

	public void changeTimeslot(int slot) {
		getAntispamManagerOneway(0).changeTimeslot(slot);
	}

	public AntispamManagerPrx getAntispamManager(int uid) {
		return locateProxy(managers, "M", uid, Channel.Invoke.Twoway,
				AntispamManagerPrxHelper.class, 300);
	}

	public AntispamManagerPrx getAntispamManagerOneway(int uid) {
		return locateProxy(managersOneway, "M", uid, Channel.Invoke.Oneway,
				AntispamManagerPrxHelper.class, 300);
	}
}
