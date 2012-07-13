package xce.socialgraph.reportservice.impl;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.ReportServicePrx;
import xce.socialgraph.ReportServicePrxHelper;
import xce.socialgraph.kFinish;
import xce.socialgraph.kInterrupt;
import xce.socialgraph.kRunning;
import xce.socialgraph.kStart;
import xce.socialgraph.reportservice.ReportService;
import xce.util.channel.Channel;

/**
 * Report Service Client实现
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class ReportServiceImpl extends ReplicatedClusterAdapter implements
	ReportService {

    private static final String endpoints = "ControllerSocialGraphReportService";

    private static int interval = 120;

    private static final int TIME_OUT = 2000;

    private static ReportService instance = null;

    private ReportServiceImpl() {
	super(endpoints, interval, Channel.newSocialGraphChannel(endpoints));
    }

    public static ReportService getInstance() {
	if (instance == null) {
	    synchronized (ReportServiceImpl.class) {
		if (instance == null) {
		    try {
			instance = new ReportServiceImpl();
		    } catch (Exception e) {
			e.printStackTrace();
		    }
		}
	    }
	}
	return instance;
    }

    private ReportServicePrx getReportServicePrxTwoWay() {
	Ice.ObjectPrx prx0 = getValidProxy(0, TIME_OUT);
	if (prx0 != null) {
	    ReportServicePrx prx = ReportServicePrxHelper.uncheckedCast(prx0
		    .ice_timeout(TIME_OUT).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    private ReportServicePrx getReportServicePrxOneWay() {
	Ice.ObjectPrx prx0 = getValidProxy(0, TIME_OUT);
	if (prx0 != null) {
	    ReportServicePrx prx = ReportServicePrxHelper.uncheckedCast(prx0
		    .ice_timeout(TIME_OUT).ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

    @Override
    public String applyJobId(String biz) {
	try {
	    return getReportServicePrxTwoWay().applyJobId(biz);
	} catch (Exception e) {
	    e.printStackTrace();
	}
	return "";
    }

    @Override
    public void finish(String jobId, String msg) {
	try {
	    getReportServicePrxTwoWay().reportJob(jobId, kFinish.value, msg);
	} catch (Exception e) {
	    e.printStackTrace();
	}

    }

    @Override
    public void interrupt(String jobId, String msg) {
	try {
	    getReportServicePrxTwoWay().reportJob(jobId, kInterrupt.value, msg);
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }

    @Override
    public void running(String jobId, String msg) {
	try {
	    getReportServicePrxTwoWay().reportJob(jobId, kRunning.value, msg);
	} catch (Exception e) {
	    e.printStackTrace();
	}

    }

    @Override
    public void start(String jobId, String msg) {
	try {
	    getReportServicePrxTwoWay().reportJob(jobId, kStart.value, msg);
	} catch (Exception e) {
	    e.printStackTrace();
	}

    }

    @Override
    public void startAndRunning(String jobId, String msg) {
	try {
	    getReportServicePrxTwoWay().reportJob(jobId, kStart.value, "");
	    getReportServicePrxTwoWay().reportJob(jobId, kRunning.value, msg);
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }
}
