package com.xiaonei.xce.passport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import passport.AccountInfo;
import passport.AccountType;
import passport.LoginCachePrx;
import passport.LoginCachePrxHelper;
import passport.LoginLogicPrx;
import passport.LoginLogicPrxHelper;
import xce.clusterstate.ReplicatedClusterMCAdapter;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

public class LoginWebImpl implements Login {

	private static List<Type> defaultTypes = new ArrayList<Type>();
	static {
		defaultTypes.add(Type.Account);
		defaultTypes.add(Type.Phone);
		defaultTypes.add(Type.Id);
	}

	private static LoginWebImpl _instance = new LoginWebImpl();

	public static LoginWebImpl getInstance() {
		return _instance;
	}

	private LoginCachePrx getLoginCache(int userId) {
		return _proxyCacheById.getProxy(userId);
	}

	private LoginCachePrx getLoginCache(String ticket) {
		return _proxyCacheByTicket.getProxy(ticket);
	}

	private LoginLogicPrx getLoginLogic(String account) {
		return _proxyLogicByAccount.getProxy(account);
	}

	private LoginLogicPrx getLoginLogic(Integer id) {
		return _proxyLogicById.getProxy(id);
	}

	protected LoginWebImpl() {
		// prevent outside new.
	}

	private class LoginCacheHolderById extends ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerLoginCache";
		private static final int _interval = 120;
		public LoginCacheHolderById() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}
		
		public LoginCachePrx getProxy(int userId) {
			 Ice.ObjectPrx prx0 = getValidProxy(userId, -1);
	            if (prx0 != null) {
	                    LoginCachePrx prx = LoginCachePrxHelper
	                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(-1));
	                    return prx;
	            } else {
	                    return null;
	            }
		}
	}

	private class LoginCacheHolderByTicket extends ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerLoginCache";
		private static final int _interval = 120;
		public LoginCacheHolderByTicket() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}
		public LoginCachePrx getProxy(String ticket) {
			 Ice.ObjectPrx prx0 = getValidProxy(ticket.charAt(32) - '0', -1);
	            if (prx0 != null) {
	                    LoginCachePrx prx = LoginCachePrxHelper
	                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(-1));
	                    return prx;
	            } else {
	                    return null;
	            }
		}
	}

	private static final AtomicInteger sequence = new AtomicInteger();

	private class LoginLogicHolderByAccount extends ReplicatedClusterMCAdapter {
//		private static final String ctr_endpoints = System.getProperty("TestLoginLogic","ControllerLoginLogic");
		private static final int _interval = 120;
		public LoginLogicHolderByAccount() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), System.getProperty("TestLoginLogic","ControllerLoginLogic"), _interval);
		}
		
		public LoginLogicPrx getProxy(String key) {
			int current = sequence.getAndIncrement();
			Ice.ObjectPrx prx0 = getValidProxy(current, 3000);
            if (prx0 != null) {
                    LoginLogicPrx prx = LoginLogicPrxHelper
                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(3000));
                    return prx;
            } else {
                    return null;
            }
		}
	}

	private class LoginLogicHolderById extends ReplicatedClusterMCAdapter {
	//	private static final String ctr_endpoints = System.getProperty("TestLoginLogic","ControllerLoginLogic");
		private static final int _interval = 120;
		public LoginLogicHolderById() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), System.getProperty("TestLoginLogic","ControllerLoginLogic"), _interval);
		}
		
		public LoginLogicPrx getProxy(int userid){
			Ice.ObjectPrx prx0 = getValidProxy(userid, 3000);
            if (prx0 != null) {
                    LoginLogicPrx prx = LoginLogicPrxHelper
                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(3000));
                    return prx;
            } else {
                    return null;
            }
		}
	}

	private LoginCacheHolderById _proxyCacheById = new LoginCacheHolderById();
	private LoginCacheHolderByTicket _proxyCacheByTicket = new LoginCacheHolderByTicket();

	private LoginLogicHolderByAccount _proxyLogicByAccount = new LoginLogicHolderByAccount();
	private LoginLogicHolderById _proxyLogicById = new LoginLogicHolderById();

	@Override
	public boolean destroyById(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			return prx.destroyById(userId, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".destroyById [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".destroyById [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public boolean destroyByTicket(String ticket) {
		if (ticket.length() == 33) {
			LoginCachePrx prx = null;
			try {
				prx = getLoginCache(ticket);
				return prx.destroyByTicket(ticket, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
			} catch(ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".destroyByTicket [" + prx
						+ "] ticket=" + ticket + " Ice.ConnectTimeoutException");
				throw e;
		     } catch (TimeoutException e) {
		    	 Oce.getLogger().error(
		    			 this.getClass().getName() + ".destroyByTicket [" + prx
		                      + "] ticket=" + ticket + " Ice.TimeoutException");
		    	 throw e;
		    }
		}
		else
			return false;
	}



	@Override
	public boolean getStatus(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			return prx.getStatus(userId);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getStatus [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getStatus [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public void setStatus(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			prx.setStatus(userId);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setStatus [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".setStatus [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public String id2Ticket(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			return prx.id2Ticket(userId, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".id2Ticket [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".id2Ticket [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	public List<Account> loginBatchAccount(List<Login.Type> types, String user, String password, String site){
	    AccountType[] pTypes = new AccountType[types.size()];
		for (int i = 0; i < types.size(); ++i) {
			switch (types.get(i)) {
			case Account:
				pTypes[i] = AccountType.Account;
				break;
			case Phone:
				pTypes[i] = AccountType.Phone;
				break;
			case Id:
				pTypes[i] = AccountType.Id;
				break;
			case BindedId:
				pTypes[i] = AccountType.BindedId;
				break;
			default:
				return null;
			}
		}
		TimeCost timecost = TimeCost.begin("invoke_login " + user);
		AccountInfo[] accs;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(user);
			accs = prx.login(pTypes, user, password, site, -1, true);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".login [" + prx
					+ "] userId=" + user + " password=" + password + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".login [" + prx
	                      + "] userId=" + user + " password=" + password + " Ice.TimeoutException");
	    	 throw e;
	    }
		timecost.endFinally();
		List<Account> results=new ArrayList<Account>();
		for (AccountInfo ac : accs) {
			if (ac.success == 0) {
				if (site.equals("")||ac.domain.equals(site)) {
					results.add(new Account(ac));
				}
			}
		}
		return results;
	}

	@Override
	public Account login(List<Type> types, String user, String password, String site, int expirePeriod, boolean reuseTicket) {
		AccountType[] pTypes = new AccountType[types.size()];
		for (int i = 0; i < types.size(); ++i) {
			switch (types.get(i)) {
			case Account:
				pTypes[i] = AccountType.Account;
				break;
			case Phone:
				pTypes[i] = AccountType.Phone;
				break;
			case Id:
				pTypes[i] = AccountType.Id;
				break;
			case BindedId:
				pTypes[i] = AccountType.BindedId;
				break;
			default:
				return null;
			}
		}
		AccountInfo buf = null;
		TimeCost timecost = TimeCost.begin("invoke_login " + user);
		AccountInfo[] accs;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(user);
			accs = prx.loginDefaultAccount(pTypes, user, password, site, expirePeriod, reuseTicket);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".login [" + prx
					+ "] userId=" + user + " password=" + password + " expirePeriod=" + expirePeriod +
					" reuseTicket=" + reuseTicket + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".login [" + prx
	                      + "] userId=" + user + " password=" + password + " expirePeriod=" + expirePeriod +
	  					" reuseTicket=" + reuseTicket + " Ice.TimeoutException");
	    	 throw e;
	    }
		timecost.endFinally();
		for (AccountInfo ac : accs) {
			if (ac.success == 0) {
				if (ac.domain.equals(site))
					return new Account(ac);
				else
					buf = ac;
			}
		}
		if (buf != null)
			return new Account(buf);
		else
			return new Account(accs[accs.length - 1]);
	}

	@Override
	public Account loginVirtual(List<Type> types, String user, String password,
			String site, int expirePeriod, boolean reuseTicket) {
		AccountType[] pTypes = new AccountType[types.size()];
		for (int i = 0; i < types.size(); ++i) {
			switch (types.get(i)) {
			case Account:
				pTypes[i] = AccountType.Account;
				break;
			case Phone:
				pTypes[i] = AccountType.Phone;
				break;
			case Id:
				pTypes[i] = AccountType.Id;
				break;
			case BindedId:
				pTypes[i] = AccountType.BindedId;
				break;
			default:
				return null;
			}
		}
		AccountInfo buf = null;
		TimeCost timecost = TimeCost.begin("invoke_login " + user);
		AccountInfo[] accs;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(user);
			accs = prx.loginVirtual(pTypes, user, password, site, expirePeriod, reuseTicket);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".login [" + prx
					+ "] userId=" + user + " password=" + password + " expirePeriod=" + expirePeriod +
					" reuseTicket=" + reuseTicket + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".login [" + prx
	                      + "] userId=" + user + " password=" + password + " expirePeriod=" + expirePeriod +
	  					" reuseTicket=" + reuseTicket + " Ice.TimeoutException");
	    	 throw e;
	    }
		timecost.endFinally();
		for (AccountInfo ac : accs) {
			if (ac.success == 0) {
				if (ac.domain.equals(site))
					return new Account(ac);
				else
					buf = ac;
			}
		}
		if (buf != null)
			return new Account(buf);
		else
			return new Account(accs[accs.length - 1]);
	}
	
	@Override
	public Account switchAccount(int id, String pwd, boolean reuse_ticket) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			return new Account(prx.switchAccount(id, pwd, reuse_ticket, TraceMe.createStackTraceContext(TraceMe.getStackTrack())));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".switchAccount [" + prx
					+ "] userId=" + id + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".switchAccount [" + prx
	                      + "] userId=" + id + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public Account switchAccountIncVirtualId(int sourceId, int desId, String pwd,
			boolean reuse_ticket) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(sourceId);
			return new Account(prx.switchAccountIncVirtualId(sourceId, desId, pwd, reuse_ticket,TraceMe.createStackTraceContext(TraceMe.getStackTrack())));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".switchAccount [" + prx
					+ "] sourceId=" + sourceId + " desId=" + desId  + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
						this.getClass().getName() + ".switchAccount [" + prx
						+ "] sourceId=" + sourceId + " desId=" + desId  + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	@Override
	public Account id2Account(int id) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			return new Account(prx.id2Account(id, TraceMe.createStackTraceContext(TraceMe.getStackTrack())));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".id2Account [" + prx
					+ "] userId=" + id + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".id2Account [" + prx
	                      + "] userId=" + id + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public int ticket2Id(String ticket) {
		if (ticket.length() == 33)
		{
			LoginCachePrx prx = null;
			try {
				prx = getLoginCache(ticket);
				return prx.ticket2Id(ticket, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
			} catch(ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".ticket2Id [" + prx
						+ "] ticket=" + ticket + " Ice.ConnectTimeoutException");
				throw e;
		     } catch (TimeoutException e) {
		    	 Oce.getLogger().error(
		    			 this.getClass().getName() + ".ticket2Id [" + prx
		                      + "] ticket=" + ticket + " Ice.TimeoutException");
		    	 throw e;
		    }
		}
		else
			return -1;
	}

	@Override
	public Account login(List<Type> types, String user, String password, String site, int expirePeriod) {
		return login(types, user, password, site, expirePeriod, true);
	}

	@Override
	public Account login(String user, String password, String site, int expirePeriod) {
		return login(defaultTypes, user, password, site, expirePeriod, true);
	}

	@Override
	public String createTicket(int id, int expirePeriod, boolean reuseTicket) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			return prx.createTicket(id, expirePeriod, reuseTicket);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createTicket [" + prx
					+ "] userId=" + id + " expirePeriod=" + expirePeriod + " reuseTicket=" + 
					reuseTicket + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".createTicket [" + prx
	                      + "] userId=" + id + " expirePeriod=" + expirePeriod + " reuseTicket=" + 
					reuseTicket + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public List<Account> get(String account, Type type) {
		if(Type.Account==type){
			return getAccount(account);
		} else if(Type.Phone==type) {
			return getPhone(account);
		} else if(Type.Id==type){
			return getId(account);
		} else if(Type.BindedId==type){
			return getBindedId(account);
		} else {
			return new ArrayList<Account>();
		}
	}

	@Override
	public List<Account> getById(int id) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			raw = prx.getById(id);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAccount [" + prx
					+ "] id=" + id + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getAccount [" + prx
	                      + "] id=" + id + " Ice.TimeoutException");
	    	 throw e;
	    }
		List<Account> result = new ArrayList<Account>();
		for (AccountInfo r : raw) {
			result.add(new Account(r));
		}
		return result;
	}
	
	@Override
	public boolean has(String account, Type type) {
		if(Type.Account==type){
			return hasAccount(account);
		} else if(Type.Phone==type) {
			return hasPhone(account);
		} else if(Type.Id==type){
			return hasId(account);
		} else if(Type.BindedId==type){
			return hasBindedId(account);
		} else {
			return false;
		}
	}
	
	private boolean hasBindedId(String id) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			return prx.has(AccountType.BindedId, id, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasBindedId [" + prx
					+ "] userId=" + id + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasBindedId [" + prx
	                      + "] userId=" + id + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	private boolean hasId(String id) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(id);
			return prx.has(AccountType.Id, id, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasId [" + prx
					+ "] userId=" + id + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasId [" + prx
	                      + "] userId=" + id + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	private boolean hasPhone(String phone) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(phone);
			return prx.has(AccountType.Phone, String.valueOf(phone), TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasPhone [" + prx
					+ "] phone=" + phone + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasPhone [" + prx
	                      + "] phone=" + phone + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	@Override
	public List<Account> getAccount(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(account);
			raw = prx.get(AccountType.Account, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAccount [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getAccount [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
		List<Account> result = new ArrayList<Account>();
		for (AccountInfo r : raw) {
			result.add(new Account(r));
		}
		return result;
	}
	
	private List<Account> getBindedId(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(account);
			raw = prx.get(AccountType.BindedId, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBindedId [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getBindedId [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
		List<Account> result = new ArrayList<Account>();
		for (AccountInfo r : raw) {
			result.add(new Account(r));
		}
		return result;
	}
	
	private List<Account> getId(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(account);
			raw = prx.get(AccountType.Id, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getId [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getId [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
		List<Account> result = new ArrayList<Account>();
		for (AccountInfo r : raw) {
			result.add(new Account(r));
		}
		return result;
	}
	
	private List<Account> getPhone(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(account);
			raw = prx.get(AccountType.Phone, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getPhone [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".getPhone [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
		List<Account> result = new ArrayList<Account>();
		for (AccountInfo r : raw) {
			result.add(new Account(r));
		}
		return result;
	}

	@Override
	public boolean hasAccount(String account) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic(account);
			return prx.has(AccountType.Account, account, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasAccount [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasAccount [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	@Override
	public List<Account> get(String account, List<Type> types) {
		Map<Integer,Account> map=new HashMap<Integer,Account>();
		for(Type type:types){
			List<Account> tempaccounts = get(account,type);
			for(Account a:tempaccounts){
				if(!map.containsKey(a.getId())){
					map.put(a.getId(), a);
				}
			}
		}
		if(map.size()>1){
			map.remove(-1);
		}
		return new ArrayList<Account>(map.values());
	}

	@Override
	public boolean has(String account, List<Type> types) {
		for(Type type:types){
			if(has(account,type)){
				return true;
			}
		}
		return false;
	}
	
}
