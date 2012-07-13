package mop.hi.oce.adapter.impl;

import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.svc.model.UserConfigWriterPrx;
import mop.hi.svc.model.UserConfigWriterPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.Oce;

public class UserConfigInfoWriterAdapterImpl extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserConfigWriter";

	private static int _interval = 1;

	public UserConfigInfoWriterAdapterImpl() {
		super(ctr_endpoints, _interval);
	}

	public void createUserConfig(int userId, UserConfigInfo uc) {
		TimeCost cost = TimeCost.begin("UserConfigInfoWriterAdapterImpl.createUserConfig id: "+userId);
		UserConfigWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserConfig10sWriter(userId);
			prx.createUserConfig(uc.demark());
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".createUserConfig [" + prx  + "]  id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserConfig(int userId, UserConfigInfo uc) {
		TimeCost cost = TimeCost.begin("UserConfigInfoWriterAdapterImpl.setUserConfig id: "+userId);
		UserConfigWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserConfigWriter(userId);
			prx.setUserConfig(userId, uc.demark());
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setUserConfig [" + prx  + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	public void reloadUserConfig(int userId){
		TimeCost cost = TimeCost.begin("UserConfigInfoReaderAdapterImpl.reloadUserConfig id: "+userId);
		UserConfigWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserConfigWriter(userId);
			prx.reload(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadUserConfig [" + prx  + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	protected UserConfigWriterPrx getUserConfig10sWriter(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 10*1000);
		if (prx0 != null) {
			UserConfigWriterPrx prx = UserConfigWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserConfigWriterPrx getUserConfigWriter(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserConfigWriterPrx prx = UserConfigWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		int userId = 800000444;
//		int basicConfig = 18181883;
//		int browseConfig = 19992000;
//		String displayMenuList = "xiaonei/renren&nuomi";
//		
//		
//		UserConfigInfoWriterAdapterImpl userConfigInfoWriter = new UserConfigInfoWriterAdapterImpl();
//		System.out.println("networkAdapter : " + userConfigInfoWriter);
//		try {
//			UserConfigInfo uc = new UserConfigInfo();
//			uc.setId(userId);
//			uc.setBasicConfig(basicConfig);
//			uc.setBrowseConfig(browseConfig);
//			uc.setDisplayMenuList(displayMenuList);
////			userConfigInfoWriter.createUserConfig(userId, uc);
//			userConfigInfoWriter.setUserConfig(userId, uc);
////			userConfigInfoWriter.reloadUserConfig(userId);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		System.out.println("Set OK!");
//		System.exit(0);
//	}
//	
}
