package com.xiaonei.xce.userstate;

import java.util.Vector;

import xce.userbase.CStateCHARITY;
import xce.userbase.CStateINWHITELIST;
import xce.userbase.CStateCHARITYINVITED;
import xce.userbase.CStateEVERVIPMEMBER;
import xce.userbase.CStateCOUPLESSPACE;
import xce.userbase.CStatePAGEMANAGER;
import xce.userbase.CStateEXPERIMENT;
import xce.userbase.CStateJUMPLOGIC;
import xce.userbase.CStateGUIDEDONE;
import xce.userbase.CStateMOBILEBIND;
import xce.userbase.CStateMSNIMME;
import xce.userbase.CStateVEST;
import xce.userbase.CStateDisneyVIP;
import xce.userbase.CStateSELECTED;
import xce.userbase.CStateSOCIALGRAPHGOOD;
import xce.userbase.CStateVIPMEMBER;
import xce.userbase.CStateKEEPUSE;
import xce.userbase.CStateAPPLYSPAMMER;
import xce.userbase.CStateGAMEMANAGER;
import xce.userbase.CStateHIDEADS;
import xce.userbase.CStateDOUBLEACCOUNT;
import xce.userbase.CStateSAMEPASSWORD;
import xce.userbase.CStateLUNARBIRTH;
import xce.userbase.CStateGIFTMEMBER;
import xce.userbase.CStateMOBILEMEMBER;
import xce.userbase.CStateGROUPMEMBER;
import xce.userbase.CStateCHANNELSPOKER;
import xce.userbase.CStateV6;
import xce.userbase.UserStateData;
import xce.userbase.UserStateManagerPrx;
import xce.userbase.UserStateManagerPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;
public final class UserStateAdapter<T extends UserStateInfo> extends ReplicatedClusterAdapter
		{
	private static final String ctr_endpoints = "ControllerUserBase";


	private static final String service_name = "USM";
	
	private static int _interval = 120;

	protected UserStateFactory<T> _factory;

	public UserStateAdapter(UserStateFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	/**
	 * 获取UserState对象
	 * 
	 * @param userId
	 * @return
	 */
	public T getUserState(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.getUserState id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			T result = _factory.create();
			if (userId<=0) {
				return result;
			}
			try {
				prx = getUserStateManager(userId);
				UserStateData data = prx.getUserState(
						userId);
				result.parse(data);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".getUserState [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} catch (Throwable e) {
				Oce.getLogger().error(this.getClass().getName() , e);
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public void createUserState(int userId, UserStateInfo props) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.createUserState id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId <= 0) {
				return;
			}
			if (userId != props.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userstate.UserStateAdapter.createUserState id diff");
			}
			prx = getUserState10sManager(userId);
			prx.createUserState(props.getUpdates());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserState [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设置UserState 此方法不设置state字段，如果需要更改，请使用setXXXOn和setXXXOff方法。
	 * 
	 * @param userId
	 * @param props
	 */
	public void setUserState(int userId, UserStateInfo props) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setUserState id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			if (props.getId()!=0&&userId != props.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userstate.UserStateAdapter.setUserState id diff");
				props.setId(userId);
			}
			prx = getUserStateManager(userId);
			prx.setUserState(userId, props.getUpdates());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserState [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为星级用户
	 * 
	 * @param userId
	 */
	public void setSelectedOn(int userId, int timeout) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setSelectedOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId,timeout);
			prx.setStateOn(userId,
					CStateSELECTED.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setSelectedOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setSelectedOn(int userId) {
		setSelectedOn(userId, 300);
	}

	/**
	 * 设为非星级用户
	 * 
	 * @param userId
	 */
	public void setSelectedOff(int userId, int timeout) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setSelectedOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId, timeout);
			prx.setStateOff(userId,
					CStateSELECTED.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setSelectedOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setSelectedOff(int userId) {
		setSelectedOff(userId, 300);
	}

	/**
	 * 设为DisneyVIP用户
	 * 
	 * @param userId
	 */
	public void setDisneyVipMemberOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setDisneyVipMemberOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOn(userId,
					CStateDisneyVIP.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setDisneyVipMemberOn[" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为非DisneyVIP用户
	 * 
	 * @param userId
	 */
	public void setDisneyVipMemberOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setDisneyVipMemberOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOff(userId,
					CStateDisneyVIP.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setDisneyVipMemberOff[" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 设为VIP用户
	 * 
	 * @param userId
	 */
	public void setVipMemberOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setVipMemberOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOn(userId,
					CStateVIPMEMBER.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setVipMemberOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为非VIP用户
	 * 
	 * @param userId
	 */
	public void setVipMemberOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setVipMemberOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOff(userId,
					CStateVIPMEMBER.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setVipMemberOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 设为曾经VIP用户
	 * 
	 * @param userId
	 */
	public void setEverVipMemberOn(int userId){
		TimeCost cost = TimeCost.begin("UserStateAdapter.setEverVipMemberOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOn(userId,
					CStateEVERVIPMEMBER.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setEverVipMemberOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 设为曾经非VIP用户
	 * 
	 * @param userId
	 */
	public void setEverVipMemberOff(int userId){
		TimeCost cost = TimeCost.begin("UserStateAdapter.setEverVipMemberOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManagerOneway(userId);
			prx.setStateOff(userId,
					CStateEVERVIPMEMBER.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setEverVipMemberOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为MSNIMME用户
	 * 
	 * @param userId
	 */
	public void setMSNIMMEOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setMSNIMMEOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
					CStateMSNIMME.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setMSNIMMEOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为非MSNIMME用户
	 * 
	 * @param userId
	 */
	public void setMSNIMMEOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setMSNIMMEOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOff(userId,
					CStateMSNIMME.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setMSNIMMEOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为手机绑定用户
	 * 
	 * @param userId
	 */
	public void setMobileBindOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setMobileBindOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
					CStateMOBILEBIND.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setMobileBindOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 取消手机绑定用户
	 * 
	 * @param userId
	 */
	public void setMobileBindOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setMobileBindOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOff(userId,
					CStateMOBILEBIND.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setMobileBindOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为持续使用的活跃用户
	 * 
	 * @param userId
	 */
	public void setKeepUseOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setKeepUseOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
					CStateKEEPUSE.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setKeepUseOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 取消持续使用的活跃用户
	 * 
	 * @param userId
	 */
	public void setKeepUseOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setKeepUseOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOff(userId,
					CStateKEEPUSE.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setKeepUseOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 设为已被引导过的用户
	 * 
	 * @param userId
	 */
	public void setGuideDoneOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setGuideDoneOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
					CStateGUIDEDONE.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setGuideDoneOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 设为SocialGraph用户
	 * 
	 * @param userId
	 */
	
	public void setSocialGraphGoodOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setSocialGraphGoodOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId, 
					CStateSOCIALGRAPHGOOD.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setSocialGraphGoodOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 取消SocialGraph用户
	 * 
	 * @param userId
	 */
	public void setSocialGraphGoodOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setSocialGraphGoodOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOff(userId,
					CStateSOCIALGRAPHGOOD.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setSocialGraphGoodOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	/**
	 * 
	 * 
	 * @param userId
	 */
	public void setCharityOn(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setCharityOn id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
					CStateCHARITY.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setCharityOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	/**
	 * 
	 * 
	 * @param userId
	 */
	public void setCharityOff(int userId) {
		TimeCost cost = TimeCost.begin("UserStateAdapter.setCharityOff id: "+userId);
		UserStateManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOff(userId,
					CStateCHARITY.value);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setCharityOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	 public void setCharityInvitedOn(int userId) {
	    TimeCost cost = TimeCost.begin("UserStateAdapter.setCharityInvitedOn id: "+userId);
	    UserStateManagerPrx prx =null;
	    try {
			if (userId<=0) {
				return;
			}
			prx = getUserStateManager(userId);
			prx.setStateOn(userId,
	          CStateCHARITYINVITED.value);
	    } catch (TimeoutException e) 
	    {
	      Oce.getLogger().error(this.getClass().getName() + ".setCharityInvitedOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	  }

	  /**
	   * 
	   * 
	   * @param userId
	   */
	  public void setCharityInvitedOff(int userId) {
	    TimeCost cost = TimeCost.begin("UserStateAdapter.setCharityInvitedOff id: "+userId);
	    UserStateManagerPrx prx =null;
	    try {
			if (userId<=0) {
				return;
			}
			prx=getUserStateManager(userId);
			prx.setStateOff(userId,
	          CStateCHARITYINVITED.value);
	    } catch (TimeoutException e) 
	    {
	      Oce.getLogger().error(this.getClass().getName() + ".setCharityInvitedOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	  }
	  
	  /**
		 * 
		 * 
		 * @param userId
		 */
		public void setVestOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setVestOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateVEST.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setVestOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}

		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setVestOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setVestOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateVEST.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setVestOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		
	
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setApplySpammerOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setApplySpammerOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateAPPLYSPAMMER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setApplySpammerOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setApplySpammerOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setApplySpammerOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateAPPLYSPAMMER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setApplySpammerOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}

		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGameManagerOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGameManagerOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateGAMEMANAGER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGameManagerOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGameManagerOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGameManagerOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateGAMEMANAGER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGameManagerOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	protected UserStateManagerPrx getUserState10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserStateManagerPrx prx = UserStateManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserStateManagerPrx getUserStateManager(int userId, int timeout) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserStateManagerPrx prx = UserStateManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserStateManagerPrx getUserStateManager(int userId) {
		return getUserStateManager(userId, 300);
	}

	protected UserStateManagerPrx getUserStateManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserStateManagerPrx prx = UserStateManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setHideAdsOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setHideAdsOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateHIDEADS.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setHideAdsOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setHideAdsOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setHideAdsOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateHIDEADS.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setHideAdsOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setDoubleAccountOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setDoubleAccountOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateDOUBLEACCOUNT.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setDoubleAccountOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setDoubleAccountOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setDoubleAccountOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateDOUBLEACCOUNT.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setDoubleAccountOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setSamePasswordOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setSamePasswordOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateSAMEPASSWORD.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setSamePasswordOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setSamePasswordOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setSamePasswordOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateSAMEPASSWORD.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setSamePasswordOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}

		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setCouplesSpaceOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setCouplesSpaceOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateCOUPLESSPACE.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setCouplesSpaceOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setCouplesSpaceOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setCouplesSpaceOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateCOUPLESSPACE.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setCouplesSpaceOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setPageManagerOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setPageManagerOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStatePAGEMANAGER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setPageManagerOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setPageManagerOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setPageManagerOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStatePAGEMANAGER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setPageManagerOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setExperimentOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setExperimentOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateEXPERIMENT.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setExperimentOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setExperimentOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setExperimentOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateEXPERIMENT.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setExperimentOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setJumpLogicOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setJumpLogicOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateJUMPLOGIC.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setJumpLogicOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setJumpLogicOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setJumpLogicOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateJUMPLOGIC.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setJumpLogicOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setLunarBirthOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setLunarBirthOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateLUNARBIRTH.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setLunarBirthOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setLunarBirthOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setLunarBirthOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateLUNARBIRTH.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setLunarBirthOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGiftMemberOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGiftMemberOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateGIFTMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGiftMemberOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGiftMemberOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGiftMemberOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateGIFTMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGiftMemberOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setMobileMemberOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setMobileMemberOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateMOBILEMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setMobileMemberOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setMobileMemberOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setMobileMemberOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateMOBILEMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setMobileMemberOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setInWhiteListOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setInWhiteListOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateINWHITELIST.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setInWhiteListOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setInWhiteListOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setInWhiteListOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateINWHITELIST.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setInWhiteListOff [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGroupMemberOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGroupMemberOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateGROUPMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGroupMemberOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setGroupMemberOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setGroupMemberOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateGROUPMEMBER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setGroupMemberOff[" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
	
    /**
		 * 
		 * 
		 * @param userId
		 */
		public void setChannelSpokerOn(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setChannelSpokerOn id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateCHANNELSPOKER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setChannelSpokerOn [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setChannelSpokerOff(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setChannelSpokerOff id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateCHANNELSPOKER.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setChannelSpokerOff[" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
    /**
		 * 
		 * 
		 * @param userId
		 */
		public void setV6On(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setV6On id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOn(userId,
						CStateV6.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setV6On [" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
		/**
		 * 
		 * 
		 * @param userId
		 */
		public void setV6Off(int userId) {
			TimeCost cost = TimeCost.begin("UserStateAdapter.setV6Off id: "+userId);
			UserStateManagerPrx prx =null;
			try {
				if (userId<=0) {
					return;
				}
				prx = getUserStateManager(userId);
				prx.setStateOff(userId,
						CStateV6.value);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".setV6Off[" + prx + "] id=" + userId + " Ice.TimeoutException");
				throw e;
			} finally {
				cost.endFinally();
			}
		}
}

