package mop.hi.oce.adapter;

import mop.hi.oce.domain.feed.FeedPublishHolder;
import mop.hi.oce.domain.feed.FeedQuerySetHolder;
//import mop.hi.svc.dispatch.DispatchException;

public interface FeedAdapter {
	public static final int ALL_TYPE = -1;

	/**
	 * 发布一个Feed
	 * 
	 * @param ev
	 */
	public void dispatch(FeedPublishHolder feed);

	public void dispatchAgain(int startId, int endId);

	public void deliver(FeedPublishHolder feed, int[] acceptors);
	
	public void deliverSupply(int supplier, int acceptor);
	
	/**
	 * 取得Feed的个数
	 * 
	 * @param userId
	 *            用户编号
	 * @param type
	 *            Feed类型
	 * @param unreadOnly
	 *            是否只包括未读条目
	 * @return 符合条件的Feed个数
	 * @throws DispatchException
	 */

	public int getFeedMiniCount(int userId, String type, boolean unknown)
			throws Exception;

	/**
	 * 取得Feed列表
	 * 
	 * @param userId
	 * @param type
	 *            类型，-1代表全部类型。
	 * @param unreadOnly
	 * @param begin
	 * @param limit
	 *            -1代表全部
	 * @return
	 * @throws DispatchException
	 */
	
	public FeedQuerySetHolder getFeedMiniSeq(int userId, String type,
			boolean unknown, int begin, int limit) throws Exception;

	/**
	 * 将符合条件的条目设为已读。
	 * 
	 * @param userId
	 * @param resource
	 * @param type
	 */
	public void setFeedMiniKnown(int userId, int feedId);
	/**
	 * 将用户所有条目都设为已读。
	 * 
	 * @param userId
	 */
	public void setAllFeedMiniKnown(int userId);
}
