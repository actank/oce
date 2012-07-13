package com.xiaonei.xce.footprintunite;

import java.sql.Date;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.model.FootprintNewView;
import mop.hi.oce.domain.model.FootprintResult;

import com.xiaonei.xce.globalid.GlobalId;

import xce.footprint.FootprintFacadeManagerPrx;
import xce.footprint.FootprintFacadeManagerPrxHelper;
import xce.footprint.FootprintInfo;
import xce.footprint.HomeFootprintCachePrx;
import xce.footprint.HomeFootprintCachePrxHelper;
import xce.footprint.HomeInfo;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;

import com.xiaonei.xce.footprint.FootprintNewAdapter;
import com.xiaonei.xce.footprintunite.HomeFootprintResult;

import com.xiaonei.xce.log.Oce;

public class FootprintUniteAdapter extends ReplicatedClusterAdapter {

	private Channel channel;
	private FootprintFacadeAdapter facade;
	private static final String ctr_endpoints = "ControllerFootprintUnite";
	private static int _interval = 120;
	private int _cluster = 10;
	private Object ownerId;

	public FootprintUniteAdapter() {
		super(ctr_endpoints, _interval);
		facade = new FootprintFacadeAdapter();
	}

	public HomeFootprintResult getAll(int owner) {
		return getAll(owner, 0, 6, 0);
	}

	public HomeFootprintResult getAll(int owner, int begin, int limit) {
		return getAll(owner, begin, limit, 0);
	}

	public HomeFootprintResult getAll(int owner, int begin, int nLimit,
			int tLimit) {

		HomeFootprintResult res = null;
		HomeFootprintCachePrx prx = null;
		try {
			prx = getHomeFootprintManager(owner);
			HomeInfo info = prx.getAll(owner, begin,
					nLimit, tLimit);
			res = new HomeFootprintResult(owner, info.steps, info.visitcount);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAll [" + prx.toString() + "]" + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			//Oce.getLogger().error(
		//			this.getClass().getName() + ".getAll [" + prx.toString() + "]" + owner, e);
		}

		return res;
	}

	public int getSize(int owner) {
		int size = 0;
		HomeFootprintCachePrx prx = null;
		try {
			prx = getHomeFootprintManager(owner);
			size = prx.getSize(owner);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getSize [" + prx + "]" + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getSize [" + prx.toString() + "]" + owner, e);
		}
		return size;
	}

	public int getUserVisitCount(int owner) {
		HomeFootprintCachePrx prx = null;
		try {
			prx = getHomeFootprintManager(owner);
			return prx.getUserVisitCount(owner);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getUserVisitCount [" + prx.toString() + "]" + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getUserVisitCount [" + prx.toString() + "]" + owner,
					e);
		}
		return 0;
	}

	public void setUserVisitCount(int owner, int count) {
		HomeFootprintCachePrx prx = null; 
		try {
			prx = getHomeFootprintManager(owner);
			prx.setUserVisitCount(owner, count);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setUserVisitCount [" + prx.toString() + "]"  + owner
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setUserVisitCount [" + prx.toString() + "]"  + owner,
					e);
		}
	}

	public void remove(FootprintView fpt) {
		HomeFootprintCachePrx prx = null;
		try {
			prx = getHomeFootprintManager(fpt.getOwner());
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

	public void visit(FootprintView fpt) {

		try {
			if (facade != null) {
				facade.visit(fpt.getFootprint());
			} else {
				HomeFootprintCachePrx prx=null;
				try{
					prx=getHomeFootprintManager(fpt.getOwner());
					prx.visit(fpt.getFootprint());
				}catch (Ice.TimeoutException e)
				{
					Oce.getLogger().error(
							this.getClass().getName() + ".visit [" + prx + "] id+ "  + fpt.getOwner()
									+ " Ice.TimeoutException");
				}
			}
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visit [" + facade.toString() + "]"  + fpt.getOwner()
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visit [" + facade.toString() + "]"  + fpt.getOwner(), e);
		}
	}

	public void visitBatch(ArrayList<FootprintView> fpts) {

		try {
			if (facade != null) {
				ArrayList<FootprintInfo> list = new ArrayList<FootprintInfo>(
						fpts.size());
				for (FootprintView view : fpts) {
					list.add(view.getFootprint());
				}
				facade.visitBatch(list);
			} else {
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
					HomeFootprintCachePrx prx=null;
					try{
						prx=getHomeFootprintManager(entry.getKey(), true);
						prx.visitBatch(entry.getValue().toArray(
												new FootprintInfo[entry.getValue()
														.size()]));
					}catch (Ice.TimeoutException e)
					{
						Oce.getLogger().error(
								this.getClass().getName() + ".visitBatch [" + prx + "] id+ "  + entry.getKey()
										+ " Ice.TimeoutException");
					}
				}

			}
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitBatch [" + facade.toString() + "]"
							+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitBatch [" + facade.toString() + "]" + e);
		}
	}

	public HomeFootprintResult visitAndGet(FootprintView fpt) {

		HomeFootprintResult res = null;

		HomeInfo info = null;
		try {
			if (facade != null) {
				info = facade.visitAndGet(fpt.getFootprint());
			} else {
				HomeFootprintCachePrx prx=null;
				try{
					prx=getHomeFootprintManager(fpt.getOwner());
					info = prx.visitAndGet(
						fpt.getFootprint(), 0, 9, 0);
				}catch (Ice.TimeoutException e)
				{
					Oce.getLogger().error(
							this.getClass().getName() + ".visitAndGet [" + prx + "] id+ "  + fpt.getOwner()
									+ " Ice.TimeoutException");
				}
			}
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitAndGet [" + facade.toString() + "]"
							+ fpt.getOwner() + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visitAndGet [" + facade.toString() + "]"
							+ fpt.getOwner(), e);
		}
		if (info != null)
			res = new HomeFootprintResult(fpt.getOwner(), info.steps,
					info.visitcount);
		return res;
	}

	public HomeFootprintCachePrx getHomeFootprintManager(int owner) {
		return getHomeFootprintManager(owner, false);
	}

	public HomeFootprintCachePrx getHomeFootprintManager(int owner,
			boolean isOneWay) {

		Ice.ObjectPrx prx0 = getProxy(owner, 300);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			HomeFootprintCachePrx prx;
			if (!isOneWay) {
				prx = HomeFootprintCachePrxHelper.uncheckedCast(prx0
						.ice_timeout(300).ice_twoway());
			} else {
				prx = HomeFootprintCachePrxHelper.uncheckedCast(prx0
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
		int owner=0;
		int begin=0;
		int limit = 0;
		FootprintUniteAdapter test = AdapterFactory.getHomeFootprintAdapter();
		test.setUserVisitCount(261702304, 3405);
		System.exit(0);
		int ugcid;
		int visitor = 0;
		//int owner =223500512;
		// test.getUserVisitCount(owner);
		// int timestamp = (int) (System.currentTimeMillis() / 1000L);
		GlobalId gid = GlobalId.fromAlbum(1008);
		// // GlobalId gid = GlobalId.fromBlog(ugcId);
		// // GlobalId gid = GlobalId.fromAlbum(ugcId);
		//
		// FootprintView fpt = new FootprintView(owner, visitor, timestamp,
		// gid);
		// test.visit(fpt);
		int ownerId = 282097083;
		// int guestId = 10; // 特别好友
		//
//		System.out.println("VisitCount of " + ownerId + " : "
//				+ test.getUserVisitCount(ownerId));
		// 设置人气(慎用！！)
		// test.setUserVisitCount(ownerId, 9999);
		// System.exit(0);

		// 取人气
		// System.out.println("VisitCount of " + ownerId + " : "
		// + test.getUserVisitCount(ownerId));
		// test.setUserVisitCount(ownerId, 9000);
		// System.out.println("VisitCount of " + ownerId + " : "
		// + test.getUserVisitCount(ownerId));
		// System.exit(0);
		// 莫文蔚, 蔡健雅, SHE, 游鸿明, 周迅, 张信哲, 光良, 陈奕迅, 李宇春
		//int[] guests = new int[] { 600002664, 600002252, 600002300, 600002216,
		//		600002221, 600002349, 600002233, 600002342, 600002235 };
		int []guests = new int []{281703832};
		ArrayList<FootprintInfo> fpts = new ArrayList<FootprintInfo>();
		for (int guest : guests) {
			FootprintInfo fpt = new FootprintInfo();
			fpt.owner = ownerId;
			fpt.gid = ownerId;
			fpt.timestamp = (int) (System.currentTimeMillis() / 1000L);
			fpt.visitor = guest;
			FootprintView view = new FootprintView(fpt.owner, fpt.visitor,
					(int) (System.currentTimeMillis() / 1000L), GlobalId
							.fromBlog(468823778));
			test.visit(view);
			//fpts.add(fpt);
			//facade.visitBatch(fpts);
			// test.remove(view);
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
		// test.visitHomepage(fpnv, false, "UNKNOWN");
		// result = test.visitHomepageAndGet(fpnv, false, "UNKNOWN");

		// System.exit(0);
		//
//		System.out.println("VisitCount of " + ownerId + " : "
//				+ test.getUserVisitCount(ownerId));

		// 取脚印
		// HomeFootprintResult result = test.getAll(ownerId, 0, 96);
		// System.out.println("Visit Count of " + ownerId + " is :"
		// + result.visitcount);

		System.out.println(test.getSize(ownerId) + " footprint(s)\n[");
		HomeFootprintResult result = test.getAll(ownerId, 0, 96);
		System.out.println(result.getVisitCount());
		for (FootprintView view : result.getFootprintViews()) {
			System.out.println(view.getVisitor() + " " + view.getOwner() + " "
					+ view.getTime());
		}
		System.out.println("]\n");
	}
}
