package mop.hi.oce.adapter.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.XceStat;

import mop.hi.oce.adapter.FootprintAdapter;
import mop.hi.oce.domain.model.FootprintView;
import mop.hi.svc.fpt.Footprint;
import mop.hi.svc.fpt.FootprintException;
import mop.hi.svc.fpt.FootprintFilterManagerPrx;
import mop.hi.svc.fpt.FootprintFilterManagerPrxHelper;
import mop.hi.svc.fpt.FootprintManagerPrx;
import mop.hi.svc.fpt.FootprintManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class FootprintAdapterImpl extends ServiceAdapter implements
		FootprintAdapter {
	
	private Vector<FootprintManagerPrx> managers = new Vector<FootprintManagerPrx>();
	private Vector<FootprintManagerPrx> managersOneway = new Vector<FootprintManagerPrx>();
	private FootprintFilterManagerPrx filterManagerOneway;

	private Boolean isReplia = false;
	private ReplicatedClusterAdapter replicaAdapter;
	private FootprintAdapterFactory factory = new FootprintAdapterFactory();

	public FootprintAdapterImpl(String endpoints, int cluster, boolean filter) {
		super(endpoints, cluster, Channel.newOceChannel(endpoints));
		if (filter) {
			filterManagerOneway = channel.locate("FFM", "@FootprintFilter",
					Channel.Invoke.Oneway,
					FootprintFilterManagerPrxHelper.class, 300);
		}
		
		if((replicaAdapter = factory.create(endpoints)) != null) {
			isReplia = true;
		}
	}

	public List<FootprintView> getAll(long hostId) throws FootprintException {
		return getAll(hostId, 0, 6);
	}

	public List<FootprintView> getAll(int hostId) throws FootprintException {
		return getAll(hostId, 0, 6);
	}

	public List<FootprintView> getAll(long hostId, int begin, int limit)
			throws FootprintException {
		Footprint[] footprints = getFootprintManager(hostId).getAll(hostId,
				begin, limit);
		List<FootprintView> result = new ArrayList<FootprintView>();
		for (int i = 0; i < footprints.length; ++i) {
			FootprintView v = new FootprintView();
			v.setFootprint(footprints[i]);
			result.add(v);
		}
		return result;
	}

	public List<FootprintView> getAll(int hostId, int begin, int limit)
			throws FootprintException {
		long hostIdL = hostId;
		return getAll(hostIdL, begin, limit);
	}

	public int getSize(long hostId) throws FootprintException {
		return getFootprintManager(hostId).getSize(hostId);
	}

	public int getSize(int hostId) throws FootprintException {
		long hostIdL = hostId;
		return getSize(hostIdL);
	}

	public void visitHomepage(FootprintView fpt) {
		visitHomepage(fpt, false, "UNKNOWN", false);
	}

	public void visitHomepage(FootprintView fpt, boolean friendFlag) {
		visitHomepage(fpt, friendFlag, "UNKNOWN", false);
	}

	public void visitHomepage(FootprintView fpt, boolean friendFlag, String ref) {
		visitHomepage(fpt, friendFlag, ref, false);
	}

	public void visitHomepage(FootprintView fpt, boolean friendFlag,
			String ref, boolean ignoreLimitFlag) {
		Map<String, String> ctx = new HashMap<String, String>();
		if (friendFlag) {
			ctx.put("friendstatus", "yes");
		} else {
			ctx.put("friendstatus", "no");
		}
		if (ignoreLimitFlag) {
			ctx.put("ignorelimitstatus", "yes");
		} else {
			ctx.put("ignorelimitstatus", "no");
		}
		ctx.put("REF", ref);
		if (filterManagerOneway != null) {
			filterManagerOneway.visitHomepage(fpt.getFootprint(), ctx);
		} else {
			getFootprintManagerOneway(fpt.getHost()).visitHomepage(
					fpt.getFootprint(), ctx);
		}
	}

	public FootprintManagerPrx getFootprintManager(long host) {
		if(isReplia) {
			Ice.ObjectPrx prx0 = replicaAdapter.getProxy((int) host, 300);

			if (prx0 != null) {
				FootprintManagerPrx prx;
				prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(
						300).ice_twoway());
				return prx;
			} else {
				return null;
			}
		}
		
		Long mod = 0L;
		if (cluster > 0) {
			mod = host % cluster;
		}
		return locateProxy(managers, "FM", mod.intValue(),
				Channel.Invoke.Twoway, FootprintManagerPrxHelper.class, 300);
	}

	public FootprintManagerPrx getFootprintManagerOneway(long host) {
		if(isReplia) {
			Ice.ObjectPrx prx0 = replicaAdapter.getProxy((int) host, 300);

			if (prx0 != null) {
				FootprintManagerPrx prx;
				prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(
						300).ice_twoway());
				return prx;
			} else {
				return null;
			}
		}
		
		Long mod = 0L;
		if (cluster > 0) {
			mod = host % cluster;
		}
		return locateProxy(managersOneway, "FM", mod.intValue(),
				Channel.Invoke.Oneway, FootprintManagerPrxHelper.class, 300);
	}

	public class BlogFootprintAdapter extends ReplicatedClusterAdapter {
		private static final String ctr_endpoints = "ControllerBlogFootprint";
		private static final int interval = 120;

		public BlogFootprintAdapter() {
			super(ctr_endpoints, interval);
		}
	}

	public class FootprintAdapterFactory {

		public ReplicatedClusterAdapter create(String type) {
			if (type.equals("@BlogFootprintL")) {
				return new BlogFootprintAdapter();
			} else {
				return null;
			}
		}
	}

	public static void main(String[] args) {
		long hostId = 240668625;
		// FootprintAdapterImpl _adapter = new
		// FootprintAdapterImpl("@TribeFootprintL", 1 , false);
		FootprintAdapterImpl _adapter = new FootprintAdapterImpl(
				"@BlogFootprintL", 1, false);
		try {
			System.out.println(_adapter.getSize(hostId));
		} catch (FootprintException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.exit(0);
	}

}
