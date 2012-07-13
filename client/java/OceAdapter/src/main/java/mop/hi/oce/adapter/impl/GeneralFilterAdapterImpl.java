package mop.hi.oce.adapter.impl;

import java.util.Map;

import mop.hi.oce.adapter.GeneralFilterAdapter;
import mop.hi.oce.domain.model.GeneralVisitInformation;
import mop.hi.svc.gf.GeneralFilterManagerPrx;
import mop.hi.svc.gf.GeneralFilterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class GeneralFilterAdapterImpl extends ServiceAdapter implements
		GeneralFilterAdapter {
	private static final String endpoints = "@GeneralFilter";
	private GeneralFilterManagerPrx manager;

	public int filterControl(Map cmd, int type) {
		return manager.filterControl(cmd, type);
	}

	public int passFilter(GeneralVisitInformation info) {
		return manager.passFilter(info.getGeneralInformation());
	}

	public GeneralFilterAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("GFM", Channel.Invoke.Twoway,
				GeneralFilterManagerPrxHelper.class, 300);
	}
}
