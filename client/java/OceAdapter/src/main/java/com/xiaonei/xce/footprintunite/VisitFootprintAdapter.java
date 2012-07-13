package com.xiaonei.xce.footprintunite;

import java.sql.Date;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.util.Map.Entry;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.model.FootprintNewView;
import mop.hi.oce.domain.model.FootprintResult;

import com.xiaonei.services.FriendInput.FriendInputManagerPrx;
import com.xiaonei.services.FriendInput.FriendInputManagerPrxHelper;
import com.xiaonei.xce.globalid.GlobalId;

import xce.footprint.FootprintFacadeManagerPrx;
import xce.footprint.FootprintFacadeManagerPrxHelper;
import xce.footprint.FootprintInfo;
import xce.footprint.HomeFootprintCachePrx;
import xce.footprint.HomeFootprintCachePrxHelper;
import xce.footprint.HomeInfo;
import xce.footprint.UgcFootprintCachePrx;
import xce.footprint.UgcFootprintCachePrxHelper;
import xce.footprint.UgcInfo;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.footprint.FootprintNewAdapter;
import com.xiaonei.xce.footprintunite.HomeFootprintResult;

import com.xiaonei.xce.log.Oce;

public class VisitFootprintAdapter extends ReplicatedClusterAdapter {

	private FootprintFacadeAdapter facade;
	private static final String ctr_endpoints = "ControllerVisitFootprint";
	private static int _interval = 120;
	private int _cluster = 10;

	public VisitFootprintAdapter() {
		super(ctr_endpoints, _interval);
		facade = new FootprintFacadeAdapter();
	}
	
	public VisitFootprintResult getAll(int owner) {
		return getAll(owner, 0, 6, 0);
	}

	public VisitFootprintResult getAll(int owner, int begin, int limit) {
		return getAll(owner, begin, limit, 0);
	}

	public VisitFootprintResult getAll(int owner, int begin, int nLimit,
			int tLimit) {
		VisitFootprintResult res = null;
		UgcFootprintCachePrx prx = null;
		try {
			prx = getVisitFootprintManager(owner);
			UgcInfo info = prx.getAll(owner, begin,
					nLimit, tLimit);
			res = new VisitFootprintResult(owner, info.steps);
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
		UgcFootprintCachePrx prx = null;
		try {
			prx = getVisitFootprintManager(owner);
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

	public void remove(FootprintView fpt) {
		UgcFootprintCachePrx prx = null;
		try {
			prx = getVisitFootprintManager(fpt.getOwner());
			// System.out.println("visitor:"+fpt.getVisitor()
			// +" owner: "+fpt.getOwner());
			prx.remove(fpt.getFootprint());
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".remove [" + prx.toString() + "]" + fpt.getOwner()
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".remove [" + prx.toString() + "]" + fpt.getOwner(), e);
		}
	}

	private void visit(FootprintView fpt) {
		UgcFootprintCachePrx prx = null;
		try {
			prx = getVisitFootprintManager(fpt.getOwner());
			prx.visit(fpt.getFootprint());
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visit [" + prx.toString() + "]" + fpt.getOwner()
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visit [" + prx.toString() + "]" + fpt.getOwner(), e);
		}
	}

	private void visitBatch(ArrayList<FootprintView> fpts) {
		UgcFootprintCachePrx prx = null;
		try {
			HashMap<Integer, ArrayList<FootprintInfo>> bucket = new HashMap<Integer, ArrayList<FootprintInfo>>();
			for (FootprintView fpt : fpts) {

				ArrayList<FootprintInfo> list = bucket.get(fpt.getOwner()
						% _cluster);
				if (list == null) {
					list = new ArrayList<FootprintInfo>();
				}
				list.add(fpt.getFootprint());
			}			
			for (Entry<Integer, ArrayList<FootprintInfo>> entry : bucket
					.entrySet()) {
				prx = getVisitFootprintManager(entry.getKey(), true);
				prx.visitBatch(
						entry.getValue().toArray(
								new FootprintInfo[entry.getValue().size()]));
			}

		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitBatch [" + prx.toString() + "]"
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitBatch [" + prx.toString() + "]" + e);
		}
	}

	private UgcFootprintCachePrx getVisitFootprintManager(int owner) {
		return getVisitFootprintManager(owner, false);
	}

	private UgcFootprintCachePrx getVisitFootprintManager(int owner,
			boolean isOneWay) {
//		Ice.ObjectPrx prx0 = locateProxy(_managers, "UFC", owner,
//				Channel.Invoke.Twoway, UgcFootprintCachePrxHelper.class, 300);
		Ice.ObjectPrx prx0 = getProxy(owner, 300);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			UgcFootprintCachePrx prx;
			if (!isOneWay) {
				prx = UgcFootprintCachePrxHelper.uncheckedCast(prx0
						.ice_timeout(300).ice_twoway());
			} else {
				prx = UgcFootprintCachePrxHelper.uncheckedCast(prx0
						.ice_timeout(300).ice_oneway());
			}
			return prx;
		} else {
			return null;
		}
	}

	/*
	 * public void reloadCache(int host) {public FootprintResult
	 * visitHomepageAndGet(FootprintNewView fpt, boolean friendflag, String
	 * ref); int mod = host % _cluster; int category = 0; ObjectCachePrx objPrx
	 * = channel.locate("SC", "@FootprintN"+mod, Channel.Invoke.Twoway,
	 * ObjectCachePrxHelper.class, 300); objPrx.reloadObject(category, host); }
	 */

	public static void main(String[] args) {

		int hostId = 0;
		int owner = 0;
		int begin = 0;
		int limit = 0;
		VisitFootprintAdapter test = AdapterFactory.getVisitFootprintAdapter();
		GlobalId gid = GlobalId.fromAlbum(1008);
		// // GlobalId gid = GlobalId.fromBlog(ugcId);
		// // GlobalId gid = GlobalId.fromAlbum(ugcId);

		int ownerId = 220840375;
		int[] guests = new int[] { 123, 456, 789 };
		ArrayList<FootprintInfo> fpts = new ArrayList<FootprintInfo>();
		for (int guest = 1; guest <= 30; guest++) {
			FootprintInfo fpt = new FootprintInfo();
			fpt.owner = ownerId;
			fpt.gid = ownerId;
			fpt.timestamp = (int) (System.currentTimeMillis() / 1000L);
			fpt.visitor = guest;
			FootprintView view = new FootprintView(fpt.owner, fpt.visitor,
					(int) (System.currentTimeMillis() / 1000L), GlobalId
							.fromBlog(468823778));
			test.visit(view);
			// fpts.add(fpt);
			// facade.visitBatch(fpts);
			//test.remove(view);
		}
		try {
			Thread.sleep(10);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// test.remove(view);
		// System.out.println(fpt.visitor);
		// break;
		// test.visitVisitpage(fpnv, false, "UNKNOWN");
		// result = test.visitVisitpageAndGet(fpnv, false, "UNKNOWN");

		// System.exit(0);
		//
		// System.out.println("VisitCount of " + ownerId + " : "
		// + test.getUserVisitCount(ownerId));

		// 取脚印
		// VisitFootprintResult result = test.getAll(ownerId, 0, 96);
		// System.out.println("Visit Count of " + ownerId + " is :"
		// + result.visitcount);

		System.out.println(test.getSize(ownerId) + " footprint(s)\n[");
		VisitFootprintResult result = test.getAll(ownerId, 0, 96);
		for (FootprintView view : result.getFootprintViews()) {
			System.out.println(view.getVisitor() + " " + view.getOwner() + " "
					+ view.getTime());
		}
		System.out.println("]\n");
	}
}
