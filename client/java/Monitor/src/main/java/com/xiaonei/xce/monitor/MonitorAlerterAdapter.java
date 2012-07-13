package com.xiaonei.xce.monitor;

import monitor.MonitorAlerterPrx;
import monitor.MonitorAlerterPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public class MonitorAlerterAdapter extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerMonitorAlerter";

    private static int _interval = 120;

    private MonitorAlerterAdapter() {
        super(ctr_endpoints, _interval);
    }

    private static MonitorAlerterAdapter _instance = new MonitorAlerterAdapter();

    /**
     * @return
     */
    public static MonitorAlerterAdapter getInstance() {
        return _instance;
    }

    public void alert(String subject, String msg, int type, int interval){
	String prxstr = "NULL Prx";
	try{
		MonitorAlerterPrx prx = getMonitorAlerterTwoway();
		prx.alert(subject,msg,type,interval);	
	}catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".alert " + subject
                            + " Ice.ConnectTimeoutException " + prxstr);
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".alert " + subject
                            + " Ice.TimeoutException " + prxstr);
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".alert " + subject + " " + prxstr, e);
        }	
    }


    public void alertmerely(int type, 
                          String phone, 
                          String email, String subject, String message
                          ) {

        String prxstr = "NULL Prx";
        try{
            MonitorAlerterPrx prx = getMonitorAlerterTwoway();
            prx.alertmerely(type, phone, email, subject, message);	
        }catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                                  this.getClass().getName() + ".justalert " + subject
                                  + " Ice.ConnectTimeoutException " + prxstr);
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                                  this.getClass().getName() + ".justalert " + subject
                                  + " Ice.TimeoutException " + prxstr);
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(
                                  this.getClass().getName() + ".justalert " + subject + " " + prxstr, e);
        }	
    }


    protected MonitorAlerterPrx getMonitorAlerterOneway() {
        Ice.ObjectPrx prx0 = getValidProxy(0, 300);
        if (prx0 != null) {
            MonitorAlerterPrx prx = MonitorAlerterPrxHelper.uncheckedCast(prx0.ice_oneway());
            return prx;
        } else {
            return null;
        }   
    }   


    protected MonitorAlerterPrx getMonitorAlerterTwoway() {
        Ice.ObjectPrx prx0 = getValidProxy(0, 300);
        if (prx0 != null) {
            MonitorAlerterPrx prx = MonitorAlerterPrxHelper.uncheckedCast(prx0.ice_twoway());
            return prx;
        } else {
            return null;
        }
    } 
}

