package com.xiaonei.xce.userpassport;

import xce.userbase.UserPassportManagerPrx;
import xce.userbase.UserPassportManagerPrxHelper;
import java.util.Random;

import java.util.Vector;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

import xce.clusterstate.ReplicatedClusterAdapter;
public class UserPassportAdapter<T extends UserPassportInfo> extends ReplicatedClusterAdapter 
		 {
	private static final String ctr_endpoints = "ControllerUserBase";

	private static final String service_name = "UPM";
	
	private static int _interval = 120;

	protected UserPassportFactory<T> _factory;
	private Random rand = new Random();

	public UserPassportAdapter(UserPassportFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public int getUserIdByEmail(String email) throws NoSuchUserByEmailException{
	    	email = email.replaceAll("'", "");
		TimeCost cost = TimeCost.begin("UserPassportAdapter.getUserIdByEmail email: "+email);
		UserPassportManagerPrx prx = null ;
		try {
			prx = getUserPassport10sManager(getEmailHashCode(email)) ;
			int id = prx.getUserIdByEmail(email);
			if (id == -1) {
				throw new NoSuchUserByEmailException ("NoSuchUserByEmai: "+email);
			}
			return id;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserIdByEmail [" + prx + "] email=" + email + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	private int getEmailHashCode(String email) {
		if (email == null || email.length() == 0) {
			return rand.nextInt();
		}
		int code=0;
		for(int i=0;i<email.length();i++){
			code+=(int)email.charAt(i);
		}
		return code;
	}

	public void setValid(int mod,boolean isValid)
	{ 
		UserPassportManagerPrx prx = null ;
		try {
			prx=getUserPassportManager(mod);
			prx.setValid(isValid);
		}catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setValid [" + prx + "]" +  "Ice.TimeoutException");
			throw e;
		}
	}

	public void createUserPassport(int userId, UserPassportInfo upi) {
		TimeCost cost = TimeCost.begin("UserPassportAdapter.createUserPassport id: "+userId);
		UserPassportManagerPrx prx = null ;
		try {
			if (userId<=0) {
				return;
			}
			if (userId != upi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userpassport.UserPassportAdapter.createUserPassport id diff");
			}
			prx=getUserPassport10sManager(userId);
			prx.createUserPassport(upi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserPassport[ " + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserPassport(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserPassportAdapter.getUserPassport id: "+userId);
		UserPassportManagerPrx prx = null ;
		try {
			T res = _factory.create();
			if (userId<=0) {
				return res;
			}
			prx=getUserPassportManager(userId);
			res.parse(prx.getUserPassport(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserPassport [" + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserPassport(int userId, UserPassportInfo upi, int timeout) {
		TimeCost cost = TimeCost.begin("UserPassportAdapter.setUserPassport id: "+userId);
		UserPassportManagerPrx prx = null ;
		try {
			if (userId<=0) {
				return;
			}
			if (upi.getId()!=0&&userId != upi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userpassport.UserPassportAdapter.setUserPassport id diff -> old: "+userId+" new: "+upi.getId());
				upi.setId(userId);
			}
			prx=getUserPassportManager(userId, timeout);
			prx.setUserPassport(userId,
					upi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserPassport[ " + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	public void setUserPassport(int userId, UserPassportInfo upi) {
		TimeCost cost = TimeCost.begin("UserPassportAdapter.setUserPassport id: "+userId);
		UserPassportManagerPrx prx = null ;
		try {
			if (userId<=0) {
				return;
			}
			if (upi.getId()!=0&&userId != upi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userpassport.UserPassportAdapter.setUserPassport id diff -> old: "+userId+" new: "+upi.getId());
				upi.setId(userId);
			}
			prx=getUserPassportManager(userId);
			prx.setUserPassport(userId,
					upi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserPassport[ " + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserPassportManagerPrx getUserPassport10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserPassportManagerPrx prx = UserPassportManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	protected UserPassportManagerPrx getUserPassportManager(int userId, int timeout) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, timeout);
		if (prx0 != null) {
			UserPassportManagerPrx prx = UserPassportManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(timeout).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	protected UserPassportManagerPrx getUserPassportManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserPassportManagerPrx prx = UserPassportManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserPassportManagerPrx getUserPassportManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserPassportManagerPrx prx = UserPassportManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

}
