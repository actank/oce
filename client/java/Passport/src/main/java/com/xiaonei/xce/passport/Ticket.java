package com.xiaonei.xce.passport;

import java.util.Map;

public interface Ticket {
	public static final String IP = "IP";
	public static final String AGENT = "Agent";

	/** 为指定的userId创建一张T票。用于Web的身份验证。
	 * 重复调用本方法会无视m的值，产生相同的值。
	 * 如果需要不同的票，请先调用destroyTicket方法。
	 * @param userId 指定的用户ID
	 * @param m 生成票需要的其他标识信息，推荐用Ticket.IP和Ticket.AGENT来作为key。
	 * @return 生成的结果 */
	public String createTicket(int userId, Map<String, String> m);

	/** 查询一个用户的票，不会在Web展示逻辑。
	 * 主要用于从其他验证通道（例如校内通）跳转Web时的内部接口。 */
	public String queryTicket(int userId);

	/** 验证票的有效性 */
	public int verifyTicket(String ticket, Map<String, String> m);

	/** 销毁票。销毁后再调用createTicket将产生不同的值。 */
	public boolean destroyTicket(String ticket);

}
