package com.xiaonei.xce.minigroupinput;

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

public class MiniGroupInputAdapter extends ServiceAdapter {
	private static MiniGroupInputAdapter _instance = new MiniGroupInputAdapter();
	private static final String endpoints = "@MiniGroupInput";

	private Vector<PageInputManagerPrx> _managers = new Vector<PageInputManagerPrx>();

	protected MiniGroupInputAdapter() {
		super(endpoints, 10, Channel.newGadgetsChannel(endpoints));

	}

	public static MiniGroupInputAdapter getInstance() {
		return _instance;
	}

	protected PageInputManagerPrx getPageInputManager(int userId) {
		return PageInputManagerPrxHelper.uncheckedCast(locateProxy(_managers,
				"Manager", userId, Channel.Invoke.Twoway,
				PageInputManagerPrxHelper.class, 300));
	}

	public int[] getMiniGroupInput(int hostId, String condition, int limit) {
		int[] x = null;
		PageInputManagerPrx prx = null;
		try {
			prx = getPageInputManager(hostId);
			x = prx.getPageInput(hostId, condition, limit);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getMiniGroupInput [" + prx
							+ "] userId=" + hostId + " " + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getMiniGroupInput [" + prx
							+ "] userId=" + hostId + " " + e);
		}
		return x;
	}

	public static void main(String[] args) {
		try {

			int[] x = MiniGroupInputAdapter.getInstance().getMiniGroupInput(220840375,
					"renren", 10);
			System.out.println(x.length);
			for (int u : x)
				System.out.println(u);

		} catch (Exception e) {
			System.out.println(e);
		}

		System.exit(0);
	}
}
