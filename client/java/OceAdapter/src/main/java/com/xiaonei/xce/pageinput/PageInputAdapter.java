package com.xiaonei.xce.pageinput;

import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;

import xce.PageInput.PageInputManagerPrx;
import xce.PageInput.PageInputManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;

public class PageInputAdapter extends ServiceAdapter {
	private static PageInputAdapter _instance = new PageInputAdapter();
	private static final String endpoints = "@PageInput";

	private Vector<PageInputManagerPrx> _managers = new Vector<PageInputManagerPrx>();
	private Vector<PageInputManagerPrx> _managersOneway = new Vector<PageInputManagerPrx>();

	protected PageInputAdapter() {
		super(endpoints, 10, Channel.newGadgetsChannel(endpoints));

	}

	public static PageInputAdapter getInstance() {
		return _instance;
	}

	protected PageInputManagerPrx getPageInputManager(int userId) {
		return PageInputManagerPrxHelper.uncheckedCast(locateProxy(_managers,
				"Manager", userId, Channel.Invoke.Twoway,
				PageInputManagerPrxHelper.class, 300));
	}

	public int[] getPageInput(int hostId, String condition, int limit) {
		int[] x = null;
		PageInputManagerPrx prx = null;
		try {
			prx = getPageInputManager(hostId);
			x = prx.getPageInput(hostId, condition, limit);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getPageInput [" + prx
							+ "] userId=" + hostId + " " + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getPageInput [" + prx
							+ "] userId=" + hostId + " " + e);
		}
		return x;
	}

	public static void main(String[] args) {
		try {

			int[] x = PageInputAdapter.getInstance().getPageInput(220840375,
					"双子", 10);
			System.out.println(x.length);
			for (int u : x)
				System.out.println(u);

		} catch (Exception e) {
			System.out.println(e);
		}

		System.exit(0);
	}
}
