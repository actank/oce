package com.xiaonei.xce.passport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.passport.Login.Type;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import passport.AccountInfo;
import passport.AccountType;
import passport.LoginCachePrx;
import passport.LoginCachePrxHelper;
import passport.LoginLogicPrx;
import passport.LoginLogicPrxHelper;
import xce.clusterstate.ReplicatedClusterMCAdapter;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;

public class LoginMobileClientImpl implements Login {

	private static AccountType[] defaultTypes = new AccountType[] { AccountType.Phone, AccountType.Account };
	private static LoginMobileClientImpl _instance = new LoginMobileClientImpl();

	public static LoginMobileClientImpl getInstance() {
		return _instance;
	}

	private LoginCachePrx getLoginCache(int userId) {
		return _proxyCacheById.getProxy(userId);
	}

	private LoginCachePrx getLoginCache(String ticket) {
		return _proxyCacheByTicket.getProxy(ticket);
	}

	private LoginLogicPrx getLoginLogic() {
		return _proxyLogic.getProxy();
	}

	protected LoginMobileClientImpl() {
		init();
		// prevent outside new.
	}

	private class LoginLogicHolder extends ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerMobileClientLoginLogic";
		private static final int _interval = 120;
		
		public LoginLogicHolder() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}
		
		public LoginLogicPrx getProxy(){
	            Ice.ObjectPrx prx0 = getValidProxy(0, -1);
	            if (prx0 != null) {
	            		LoginLogicPrx prx = LoginLogicPrxHelper
	                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(-1));
	                    return prx;
	            } else {
	                    return null;
	            }
		}
	}

	private class LoginCacheHolderById extends ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerMobileClientLoginCache";
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
		private final static String ctr_endpoints = "ControllerMobileClientLoginCache";
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

	private LoginCacheHolderById _proxyCacheById = new LoginCacheHolderById();
	private LoginCacheHolderByTicket _proxyCacheByTicket = new LoginCacheHolderByTicket();

	private LoginLogicHolder _proxyLogic = new LoginLogicHolder();

	@Override
	public boolean destroyById(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			return prx.destroyById(userId);
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
		LoginCachePrx prx = null;
		if(ticket.length() == 22) {
			ticket = sTicket2LTicket(ticket);
		}
		if (ticket.length() == 33) {
			try {
				prx = getLoginCache(ticket);
				return prx.destroyByTicket(ticket);
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
	public String id2Ticket(int userId) {
		LoginCachePrx prx = null;
		try {
			prx = getLoginCache(userId);
			return lTicket2STicket(prx.id2Ticket(userId));
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
	
	public List<Account> loginBatchAccount(List<Login.Type> types, String user, String password, String site) {
	    	List<Account> accounts=new ArrayList<Account>();
		AccountInfo[] accs;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			accs = prx.login(defaultTypes, user, password, site, -1, true);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".login [" + prx
					+ "] user=" + user + " password= " + password + " site=" + site
					+ " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".login [" + prx
	                      + "] user=" + user + " password= " + password + " site=" + site
	  					 + " Ice.TimeoutException");
	    	 throw e;
	    }
		for (AccountInfo ac : accs) {
			if (ac.success == 0) {
				if (site.equals("")||ac.domain.equals(site))
					accounts.add(new Account(ac,lTicket2STicket(ac.ticket)));
			}
		}
		return accounts;
	}

	@Override
	public Account login(String user, String password, String site, int expirePeriod) {
		AccountInfo buf = null;
		AccountInfo[] accs;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			accs = prx.loginDefaultAccount(defaultTypes, user, password, site, expirePeriod, true);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".login [" + prx
					+ "] user=" + user + " password= " + password + " site=" + site
					+ " expirePeriod=" + expirePeriod + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".login [" + prx
	                      + "] user=" + user + " password= " + password + " site=" + site
	  					+ " expirePeriod=" + expirePeriod + " Ice.TimeoutException");
	    	 throw e;
	    }
		for (AccountInfo ac : accs) {
			if (ac.success == 0) {
				if (ac.domain.equals(site))
					return new Account(ac,lTicket2STicket(ac.ticket));
				else
					buf = ac;
			}
		}
		if (buf != null)
			return new Account(buf, lTicket2STicket(buf.ticket));
		else
			return new Account(accs[accs.length - 1], lTicket2STicket(accs[accs.length - 1].ticket));
	}

	@Override
	public Account loginVirtual(List<Type> types, String user, String password,
			String site, int expirePeriod, boolean reuseTicket) {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public int ticket2Id(String ticket) {
		if(ticket.length() == 22) {
			ticket = sTicket2LTicket(ticket);
		}
		LoginCachePrx prx = null;
		if (ticket.length() == 33) {
			try {
				prx = getLoginCache(ticket);
				return prx.ticket2Id(ticket);
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
	public Account login(List<Type> types, String user, String password, String site, int expirePeriod, boolean flag) {
		return login(user, password, site, expirePeriod);
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
	public Account login(List<Type> types, String user, String password, String site, int expirePeriod) {
		return login(user, password, site, expirePeriod);

	}

	@Override
	public Account switchAccount(int id, String pwd, boolean reuse_ticket) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			AccountInfo acinfo = prx.switchAccount(id, pwd, reuse_ticket);
			return new Account(acinfo, lTicket2STicket(acinfo.ticket));
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
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Account id2Account(int id) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			AccountInfo acinfo = prx.id2Account(id);
			return new Account(acinfo, lTicket2STicket(acinfo.ticket));
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
	public String createTicket(int id, int expirePeriod, boolean reuseTicket) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			return lTicket2STicket(prx.createTicket(id, expirePeriod, reuseTicket));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createTicket [" + prx
					+ "] userId=" + id + " expirePeriod=" + expirePeriod + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".createTicket [" + prx
	                      + "] userId=" + id + " expirePeriod=" + expirePeriod + " Ice.TimeoutException");
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
		// TODO Auto-generated method stub
		return null;
	}

	private List<Account> getBindedId(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
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
			result.add(new Account(r, lTicket2STicket(r.ticket)));
		}
		return result;
	}
	
	private List<Account> getId(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
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
			result.add(new Account(r, lTicket2STicket(r.ticket)));
		}
		return result;
	}
	
	private List<Account> getPhone(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
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
			result.add(new Account(r, lTicket2STicket(r.ticket)));
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
	
	private boolean hasBindedId(String account) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			return prx.has(AccountType.BindedId, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasBindedId [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasBindedId [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	private boolean hasId(String account) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			return prx.has(AccountType.Id, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasId [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasId [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	private boolean hasPhone(String account) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			return prx.has(AccountType.Phone, account);
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".hasPhone [" + prx
					+ "] account=" + account + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".hasPhone [" + prx
	                      + "] account=" + account + " Ice.TimeoutException");
	    	 throw e;
	    }
	}
	
	@Override
	public List<Account> getAccount(String account) {
		AccountInfo[] raw;
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
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
			result.add(new Account(r, lTicket2STicket(r.ticket)));
		}
		return result;
	}

	@Override
	public boolean hasAccount(String account) {
		LoginLogicPrx prx = null;
		try {
			prx = getLoginLogic();
			return prx.has(AccountType.Account, account);
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
		Map<Integer, Account> map=new HashMap<Integer,Account>();
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
		// TODO Auto-generated method stub
		for(Type type:types){
			if(has(account,type)){
				return true;
			}
		}
		return false;
	}
	
	private HashMap<Character, String> sChar2SBytes_ = new HashMap<Character, String>();
	private HashMap<Character, String> lChar2LBytes_ = new HashMap<Character, String>();
	private HashMap<String, Character> sBytes2SChar_ = new HashMap<String, Character>();
	private HashMap<String, Character> lBytes2LChar_ = new HashMap<String, Character>();
	private final static String sChars_ = "0123456789abcdef";
	private final static String lChars_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
	private void init() {
	   
		Vector<String> sBytes_ = new Vector<String>();;
		Vector<String> lBytes_ = new Vector<String>();;
		String zeros = "0000000";
		int sCharsLen = sChars_.length();

		for(int i = 0; i < sCharsLen; ++i) {
			String tmp = Integer.toString(i, 2);
			tmp = zeros.subSequence(0, 4 - tmp.length()) + tmp;
			sBytes_.add(tmp);
			sChar2SBytes_.put(sChars_.charAt(i), sBytes_.get(i));
			sBytes2SChar_.put(sBytes_.get(i), sChars_.charAt(i));
		}

		int lCharsLen = lChars_.length();
		for(Integer i = 0; i < lCharsLen; ++i) {
			String tmp = Integer.toString(i, 2);
			tmp = zeros.subSequence(0, 6 - tmp.length()) + tmp;
			lBytes_.add(tmp);
			lChar2LBytes_.put(lChars_.charAt(i), lBytes_.get(i));
			lBytes2LChar_.put(lBytes_.get(i), lChars_.charAt(i));
		}
	}
	private String lTicket2STicket(String lTicket) {
		StringBuffer tmp = new StringBuffer();
		int size = lTicket.length();
		if(size != 33) {
			return "";
		}
		for(int i = 0; i < size; ++i) {
			if(!sChar2SBytes_.containsKey(lTicket.charAt(i))) {
				return "";
			}
			tmp.append(sChar2SBytes_.get(lTicket.charAt(i)));
		}

		int tmp_size = tmp.length();
		StringBuffer result = new StringBuffer();
		for(int i = 0; i < tmp_size; i += 6) {
			result.append(lBytes2LChar_.get(tmp.substring(i, i + 6)));
		}
		return result.toString();
	}
	
	private String sTicket2LTicket(String sTicket) {
		StringBuffer tmp = new StringBuffer();
		int size = sTicket.length();
		if(size != 22) {
			return "";
		}
		for(int i = 0; i < size; ++i) {
			if(!lChar2LBytes_.containsKey(sTicket.charAt(i))) {
				return "";
			}
			tmp.append(lChar2LBytes_.get(sTicket.charAt(i)));
		}
		
		int tmp_size = tmp.length();
		StringBuffer result = new StringBuffer();
		for(int i = 0; i < tmp_size; i += 4) {
			result.append(sBytes2SChar_.get(tmp.substring(i, i + 4)));
		}
		return result.toString();
	}
	public static void main(String[] args) {
		String lTicket = "abcd012bcdef980923eeeffbcde987985";
		System.out.println(lTicket);
		LoginMobileClientImpl wapImpl = LoginMobileClientImpl.getInstance();
		System.out.println(wapImpl.login("guangchao.tian@opi-corp.com", "718c82ce24cfc00237c3be00e0a4e877", "renren.com", -1).getSuccess());
		System.out.println(wapImpl.ticket2Id("d47d5bb61ad6b5e731b23298dc9768b00"));
		String sTicket = wapImpl.lTicket2STicket(lTicket);
		System.out.println(sTicket);
		System.out.println(wapImpl.sTicket2LTicket(sTicket));
		
		sTicket = "A00sdflasdf-_99888AC02";
		System.out.println(sTicket);
		lTicket = wapImpl.sTicket2LTicket(sTicket);
		System.out.println(lTicket);
		System.out.println(wapImpl.lTicket2STicket(lTicket));
		System.exit(0);
	}

}
