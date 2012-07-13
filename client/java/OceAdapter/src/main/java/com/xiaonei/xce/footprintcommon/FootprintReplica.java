package com.xiaonei.xce.footprintcommon;

import com.xiaonei.xce.log.Oce;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.footprint.FootprintInfo;
import xce.footprint.CommonFootprintCachePrx;
import xce.footprint.CommonFootprintCachePrxHelper;
import xce.footprint.CommonInfo;

public class FootprintReplica extends ReplicatedClusterAdapter {
	public FootprintReplica(String ctl, int interval) {
		super(ctl, interval);
	}

	public FootprintResult getAll(int owner, int begin, int nLimit,
			int tLimit) {

		FootprintResult res = null;
		CommonFootprintCachePrx prx = null; 
		try {
			prx = getFootprintManager(owner);
			CommonInfo info = prx.getAll(owner, begin,
					nLimit, tLimit);
			res = new FootprintResult(owner, info.steps);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAll [" + prx.toString() + "]" + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAll [" + prx.toString() + "]" + owner, e);
		}

		return res;
	}

	public int getSize(int owner) {
		int size = 0;
		CommonFootprintCachePrx prx = null; 
		try {
			prx = getFootprintManager(owner);
			size = prx.getSize(owner);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getSize [" + prx.toString() + "]" + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getSize [" + prx.toString() + "]" + owner, e);
		}
		return size;
	}

	public void remove(FootprintInfo fpt) {
		CommonFootprintCachePrx prx = null;
		try {
			prx = getFootprintManager(fpt.owner);
			prx.remove(fpt);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".remove [" + prx.toString() + "]" + fpt.owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".remove [" + prx.toString() + "]" + fpt.owner, e);
		}
	}

	private CommonFootprintCachePrx getFootprintManager(int owner) {
		return getFootprintManager(owner, false);
	}

	private CommonFootprintCachePrx getFootprintManager(int owner,
			boolean isOneWay) {
		Ice.ObjectPrx prx0 = getProxy(owner, 300);

		if (prx0 != null) {
			CommonFootprintCachePrx prx;
			if (!isOneWay) {
				prx = CommonFootprintCachePrxHelper.uncheckedCast(prx0
						.ice_timeout(300).ice_twoway());
			} else {
				prx = CommonFootprintCachePrxHelper.uncheckedCast(prx0
						.ice_timeout(300).ice_oneway());
			}
			return prx;
		} else {
			return null;
		}
	}
}