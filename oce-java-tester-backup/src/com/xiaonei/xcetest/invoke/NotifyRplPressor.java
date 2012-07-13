package com.xiaonei.xcetest.invoke;

import java.util.Random;

import mop.hi.oce.adapter.AdapterFactory;

public class NotifyRplPressor extends Thread {

	private int n;

	public NotifyRplPressor(int n) {
		this.n = n;
	}

	public void run() {
		Random r = new Random();
		while (true) {
			int userId = Math.abs(r.nextInt() % 840000);
			try {
				AdapterFactory.getReplyEventAdapter().getEventCount(userId, -1,
						false);
			} catch (Exception e) {
				System.out.println(e + " at " + n);
			}
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		for (int i = 0; i < 100; ++i) {
			new NotifyRplPressor(i).start();
		}
	}

}
