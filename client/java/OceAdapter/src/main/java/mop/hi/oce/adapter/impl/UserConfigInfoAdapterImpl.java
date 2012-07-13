package mop.hi.oce.adapter.impl;

import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.oce.domain.user.UserConfigInfoFactory;
import mop.hi.svc.model.UserConfigManagerPrx;
import mop.hi.svc.model.UserException;

public class UserConfigInfoAdapterImpl {
	
	private UserConfigInfoReaderAdapterImpl _readerAdapter = null;
	private UserConfigInfoWriterAdapterImpl _writerAdapter = null;

	public UserConfigInfoAdapterImpl() {
		this(new UserConfigInfoFactory());
	}
	
	public UserConfigInfoAdapterImpl(UserConfigInfoFactory factory) {
		_readerAdapter = new UserConfigInfoReaderAdapterImpl(factory);
		_writerAdapter = new UserConfigInfoWriterAdapterImpl();
	}

	public void createUserConfig(int userId, UserConfigInfo uc) {
		_writerAdapter.createUserConfig(userId, uc);
	}

	public UserConfigInfo getUserConfig(int userId) throws UserException, Exception {
		return _readerAdapter.getUserConfig(userId);
	}

	public void setUserConfig(int userId, UserConfigInfo uc) {
		_writerAdapter.setUserConfig(userId, uc);
	}
	
	/**
	 * Description 单元测试需要而增加，把用户的配置信息重新从数据库加载到缓存。
	 */
	public void reloadUserConfig(int userId){
		_writerAdapter.reloadUserConfig(userId);
	}
	
	/**
	 * 安全起见，本接口不再对外开放，如有需要用到，请联系中间层：
	 * renren.xce@renren-inc.com
	 * @param userId
	 * @return null
	 */
	@Deprecated
	public UserConfigManagerPrx getUserConfigManager(int userId){
		return null;
	}
	
	/**
	 * 安全起见，本接口不再对外开放，如有需要用到，请联系中间层：
	 * renren.xce@renren-inc.com
	 * @param userId
	 * @return null
	 */
	@Deprecated
	public UserConfigManagerPrx getUserConfig10sManager(int userId){
		return null;
	}

//	public static void main(String[] args) {
//		System.out.println("args:userId");
//		int userId = 800004444;
//		if (args.length == 1) {
//			userId = Integer.parseInt(args[0]);
//		}
//		UserConfigInfoAdapterImpl userConfigInfo = new UserConfigInfoAdapterImpl();
//		System.out.println("networkAdapter : " + userConfigInfo);
//		try {
//			UserConfigInfo uci = userConfigInfo.getUserConfig(userId);
//			System.out.println(uci);
////			uci.setWantSeeCss(1);
////			userConfigInfo.setUserConfig(userId, uci);
////			uci = userConfigInfo.getUserConfig(userId);
////			System.out.println(uci);
//		} catch (Ice.UnknownUserException e) {
//			System.out.println("[WARN]No such user, userId=" + userId);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//
//		System.exit(0);
//	}
//	
}
