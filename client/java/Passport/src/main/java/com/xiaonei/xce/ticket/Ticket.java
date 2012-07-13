package com.xiaonei.xce.ticket;

import java.util.List;

public interface Ticket {
	
	/**根据参数信息创建票
	 * @param info 生成票需要的相关信息，确保所有字段都被设置
	 * @param expiredTime 过期时间
	 * @return 生成的结果 */
	public String createTicket(TicketInfo info, long expiredTime);
	
	/** 验证票的有效性 */
	public long verifyTicket(String ticket, List<TicketInfo.Type> types);
	
	/** 获取票对应的信息 */
	public TicketInfo getTicketInfo(String ticket);
	
	/** 根据票销毁票 */
	public boolean destroyTicket(String ticket);
	
	/** 根据id销毁票
	＊@param id 要销毁的用户id
	 * @param type 要销毁的票类型*/
	public boolean destroyTicketById(long id, TicketInfo.Type type);

}
