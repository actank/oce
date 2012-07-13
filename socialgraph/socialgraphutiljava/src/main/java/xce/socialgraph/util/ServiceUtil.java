package xce.socialgraph.util;

import xce.socialgraph.reportservice.ReportService;
import xce.socialgraph.reportservice.impl.ReportServiceImpl;

/**
 * 取Serivce的工具类 (类比SocialGraphAdapter.class)
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class ServiceUtil {

    /**
     * job report service interface
     * 
     * @return
     */
    public static ReportService getReportService() {
	return ReportServiceImpl.getInstance();
    }

}
