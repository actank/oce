package com.xiaonei.xce.friendinput;

import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.services.FriendInput.*;
import com.xiaonei.xce.log.Oce;

public class FriendInputAdapter extends ServiceAdapter {
	private static FriendInputAdapter _instance = new FriendInputAdapter();
	private static final String endpoints = "@FriendInput";

	private Vector<FriendInputManagerPrx> _managers = new Vector<FriendInputManagerPrx>();
	private Vector<FriendInputManagerPrx> _managersOneway = new Vector<FriendInputManagerPrx>();

	protected FriendInputAdapter() {
		super(endpoints, 10, Channel.newGadgetsChannel(endpoints));

	}

	public static FriendInputAdapter getInstance() {
		return _instance;
	}

	protected FriendInputManagerPrx getFriendInputManager(int userId) {
		return FriendInputManagerPrxHelper.uncheckedCast(locateProxy(_managers,
				"Manager", userId, Channel.Invoke.Twoway,
				FriendInputManagerPrxHelper.class, 300));
	}

	protected FriendInputManagerPrx getFriendInputManagerOneway(int userId) {
		return FriendInputManagerPrxHelper.uncheckedCast(locateProxy(
				_managersOneway, "Manager", userId, Channel.Invoke.Oneway,
				FriendInputManagerPrxHelper.class, 300));
	}

	public int[] getFriendInput(int hostId, String condition, int limit) {
		int[] x = null;
		FriendInputManagerPrx prx = null;
		try {
			prx = getFriendInputManager(hostId);
			x = prx.getFriendInput(hostId, condition, limit);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendInput [" + prx
							+ "] userId=" + hostId + " " + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendInput [" + prx
							+ "] userId=" + hostId + " " + e);
		}
		return x;
	}

	public static void main(String[] args) {
		try {

			Scanner cin = new Scanner(System.in);
			String str = cin.nextLine();
			String[] s = str.split(" ");
			for (int i = 0; i < s.length; i++) {
				int[] x = FriendInputAdapter.getInstance().getFriendInput(
						220840375, s[i], 10);
				System.out.println(x.length);
				for (int u : x)
					System.out.println(u);
			}
		} catch (Exception e) {
			System.out.println(e);
		}

		System.exit(0);
	}
}
