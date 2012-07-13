package com.xiaonei.xce.passport;

import java.util.List;

public interface Login {
	public static enum Type {
		Account, Phone, Id, BindedId;
	}

	@Deprecated
	public Account login(List<Login.Type> types, String user, String password, String site, int expirePeriod);

	/** 推荐使用的登录接口 */
	public Account login(List<Login.Type> types, String user, String password, String site, int expirePeriod, boolean reuseTicket);
	
	/** 个人用户可以以Page管理员登录使用的登录接口 */
	public Account loginVirtual(List<Login.Type> types, String user, String password, String site, int expirePeriod, boolean reuseTicket);
	
	/** 批量登录，用于多帐号同密码的方式。
	 * 主要用于kaixin和renren的双帐号情况。 */
	public List<Account> loginBatchAccount(List<Login.Type> types, String user, String password, String site);

	/** 简单的登录接口，Web和Wap的实现不同 */
	public Account login(String user, String password, String site, int expirePeriod);

	/** 不使用用户名／密码，直接用id生成票。依手机app需要提供，其他需求不要使用。 */
	public String createTicket(int id, int expirePeriod, boolean reuseTicket);
	
	/** 销毁票 */
	public boolean destroyByTicket(String ticket);

	/** 销毁票 */
	public boolean destroyById(int userId);

	/** 由票取id，最大量的调用次数。 */
	public int ticket2Id(String ticket);

	/** 由id取用户资料，管理员后台使用 */
	public Account id2Account(int id);

	/** 由id取票，如果未登录则返回空串 */
	public String id2Ticket(int userId);

	/** 检查帐号是否存在，只检查帐号，如果要检查手机绑定等，请单独联系 */
	public boolean hasAccount(String account);

	/** 由帐号名获取帐号信息 */
	public List<Account> getAccount(String account);

	/** 获取“跨站提示确认”标志信息 */
	public boolean getStatus(int userId);
	
	/** 设置“跨站提示确认”标志信息 */
	public void setStatus(int userId);
	
	/** 由帐号及类型获取帐号信息*/
	public List<Account> get(String account, Type type);
	
	/** 从一批类型中获取帐号信息*/
	public List<Account> get(String account, List<Type> types);
	
	/** 由Id获取所有可以切换的帐号信息*/
	public List<Account> getById(int id);
	
	/** 检查类型对应的帐号是否存在*/
	public boolean has(String account, Type type);
	
	/** 从一批类型中检查对应的帐号是否存在*/
	public boolean has(String account, List<Type> types);

  /** 根据userid切换到相应的帐号上*/
  public Account switchAccount(int id, String pwd, boolean reuse_ticket);
  /** 根据pageId切换到相应的帐号上, 第一个参数为从哪个id切到page，第二个为要切到的id*/
  public Account switchAccountIncVirtualId(int sourceId, int desId, String pwd, boolean reuse_ticket);
}
