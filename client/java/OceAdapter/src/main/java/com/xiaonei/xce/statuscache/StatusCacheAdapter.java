package com.xiaonei.xce.statuscache;

import java.util.BitSet;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.statuscache.StatusCacheManagerPrx;
import xce.statuscache.StatusCacheManagerPrxHelper;

import com.xiaonei.xce.log.Oce;

public class StatusCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerStatusCache";
	private static int _interval = 120;
	private static int STATUS_CACHE_SIZE = 2147483647;

	private StatusCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static StatusCacheAdapter _instance = new StatusCacheAdapter();

	public static StatusCacheAdapter getInstance() {
		return _instance;
	}

	public boolean getAllStatus(BitSet s) {
		byte[] result = new byte[2147483647 / 8 + 1];
		for (Integer i = 0; i < 210; i++) {
			System.out.println("getStatus from " + i * 10000000 + " to " + (i + 1)
					* 10000000);
			boolean finished = false;
			Integer retrytimes = 0;
			while (!finished) {
				byte[] tmp = new byte[10000000 / 8];
				StatusCacheManagerPrx prx = null;
				try {
					prx = getStatusCacheManagerTwoway();
					tmp = prx.getStatusMinMax(i * 10000000, (i + 1) * 10000000);
					finished = true;
					retrytimes = 0;
				} catch (Ice.ConnectTimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".getAllStatus prx = " + prx
									+ " Ice.ConnectTimeoutException");
				} catch (Ice.TimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".getAllStatus prx = " + prx
									+ " Ice.TimeoutException");
				} catch (java.lang.Throwable e) {
					Oce.getLogger()
							.error(this.getClass().getName() + ".getAllStatus ", e);
				} finally {
					if (finished) {
						// result.insert(result.end(), temp.begin(), temp.end());
						for (Integer j = 0; j < 10000000 / 8; j++) {
							result[i * 10000000 / 8 + j] = tmp[j];
						}
					} else {
						retrytimes++;
						if (retrytimes > 3)
							return false;
					}
				}
			}
			System.out.println("getStatus from " + i * 10000000 + " to " + (i + 1)
					* 10000000 + " END");
		}
		boolean finished = false;
		Integer retrytimes = 0;
		System.out.println("getStatus from 2100000000 to 2147483647");
		while (!finished) {
			byte[] tmp = new byte[10000000 / 8];
			StatusCacheManagerPrx prx = null;
			try {
				prx = getStatusCacheManagerTwoway();
				tmp = prx.getStatusMinMax(2100000000, 2147483647);
				finished = true;
				retrytimes = 0;
			} catch (Ice.ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getAllStatus prx = " + prx
								+ " Ice.ConnectTimeoutException");
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getAllStatus prx = " + prx
								+ " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAllStatus ", e);
			} finally {
				if (finished) {
					// result.insert(result.end(), temp.begin(), temp.end());
					for (Integer j = 0; j < 10000000 / 8; j++) {
						result[21 * 10000000 / 8 + j] = tmp[j];
					}
				} else {
					retrytimes++;
					if (retrytimes > 3)
						return false;
				}
			}
		}
		System.out.println("getStatus from 2100000000 to 2147483647 END");
		for (Integer i = 0; i < STATUS_CACHE_SIZE; ++i) {
			if ((result[i / 8] & (1 << (i % 8))) > 0) {
				s.set(i);
			}
		}
		System.out.println("getAllStatus END");
		return true;
	}

	public boolean isStatusTrue(int userId) {
		boolean status = false;
		StatusCacheManagerPrx prx = null;
		try {
			prx = getStatusCacheManagerTwoway();
			status = prx.isStatusTrue(userId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isStatusTrue prx = " + prx
							+ " userId = " + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isStatusTrue prx = " + prx
							+ " userId = " + userId + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isStatusTrue " + userId + " ", e);
		} finally {
		}
		return status;
	}

	public boolean isGoodUser(int userId) {
		boolean status = false;
		StatusCacheManagerPrx prx = null;
		try {
			prx = getStatusCacheManagerTwoway();
			status = prx.isGoodUser(userId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isGoodUser prx = " + prx + " userId = "
							+ userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isGoodUser prx = " + prx + " userId = "
							+ userId + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isGoodUser " + userId + " ", e);
		} finally {
		}
		return status;
	}

	protected StatusCacheManagerPrx getStatusCacheManagerTwoway() {
		Ice.ObjectPrx prx0 = getValidProxy(0, 10000);
		if (prx0 != null) {
			StatusCacheManagerPrx prx = StatusCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected StatusCacheManagerPrx getStatusCacheManagerOneway() {
		Ice.ObjectPrx prx0 = getValidProxy(0, 10000);
		if (prx0 != null) {
			StatusCacheManagerPrx prx = StatusCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
	}
}
