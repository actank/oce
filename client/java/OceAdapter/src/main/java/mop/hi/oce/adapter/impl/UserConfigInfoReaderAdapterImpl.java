package mop.hi.oce.adapter.impl;

import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.oce.domain.user.UserConfigInfoFactory;
import mop.hi.svc.model.UserConfigReaderPrx;
import mop.hi.svc.model.UserConfigReaderPrxHelper;
import mop.hi.svc.model.UserException;
import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.Oce;

public class UserConfigInfoReaderAdapterImpl extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserConfigReader";

	private static int _interval = 1;

	private UserConfigInfoFactory factory;

	public UserConfigInfoReaderAdapterImpl() {
		this(new UserConfigInfoFactory());
	}

	public UserConfigInfoReaderAdapterImpl(UserConfigInfoFactory factory) {
		super(ctr_endpoints, _interval);
		this.factory = factory;
	}

	public UserConfigInfo getUserConfig(int userId) throws UserException,
			Exception {
		TimeCost cost = TimeCost.begin("UserConfigInfoReaderAdapterImpl.getUserConfig id: "+userId);
		UserConfigReaderPrx prx = null;
		try {
			UserConfigInfo res;

			res = factory.create();
			if (userId <= 0) {
				return res;
			}
			try{
				prx = getUserConfigReader(userId) ;
				res.parse(prx.getUserConfig(userId));
			}catch(Ice.TimeoutException e){
				Oce.getLogger().error(this.getClass().getName() + ".getUserConfig [" + prx  + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			}
			return res;
		} finally {
			cost.endFinally();
		}
	}
	
//	/**
//	 * 修改缓存内容，仅供调试测试使用。
//	 * 如有需要set请使用UserConfigInfoAdapterImpl::setUserConfig
//	 * @param userId
//	 * @param uc
//	 */
//	public void setUserConfig(int userId, UserConfigInfo uc) {
//		TimeCost cost = TimeCost.begin("UserConfigInfoReaderAdapterImpl.setUserConfig id: "+userId);
//		UserConfigReaderPrx prx = null;
//		try {
//			if (userId <= 0) {
//				return;
//			}
//			prx = getUserConfigReader(userId);
//			prx.setUserConfig(userId, uc.demark());
//		} catch (Ice.TimeoutException e) {
//			Oce.getLogger().error(this.getClass().getName() + ".setUserConfig [" + prx  + "] id=" + userId + " Ice.TimeoutException");
//			throw e;
//		} finally {
//			cost.endFinally();
//		}
//	}
//	
	protected UserConfigReaderPrx getUserConfig10sReader(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 10*1000);
		if (prx0 != null) {
			UserConfigReaderPrx prx = UserConfigReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserConfigReaderPrx getUserConfigReader(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserConfigReaderPrx prx = UserConfigReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		int userId = 800000444;
////		int userId = 399916058;
//		UserConfigInfoReaderAdapterImpl userConfigInfoReader = new UserConfigInfoReaderAdapterImpl();
//		System.out.println("networkAdapter : " + userConfigInfoReader);
//		try {
//			UserConfigInfo uci = userConfigInfoReader.getUserConfig(userId);
//			System.out.println("Before Set\n---------------\n" + uci);
////			System.out.println("\n-----\n" + (uci.getPhotoConfig() + 1) + "\n-----\n");
////			uci.setPhotoConfig((uci.getPhotoConfig() + 1));
////			userConfigInfoReader.setUserConfig(userId, uci);
////			System.out.println("After Set\n---------------\n" + uci);
//		} catch (Ice.UnknownUserException e) {
//			System.out.println("[WARN]No such user, userId=" + userId);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		System.exit(0);
//	}
//	  
}
