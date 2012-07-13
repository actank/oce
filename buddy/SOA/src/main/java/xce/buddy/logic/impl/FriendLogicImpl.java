package xce.buddy.logic.impl;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.buddy.BuddyApply;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.buddy.BuddyBlock;
import mop.hi.oce.domain.buddy.BuddyRelation;
import xce.buddy.FriendApply;
import xce.buddy.FriendApplyResult;
import xce.buddy.Operation;
import xce.buddy.logic.FriendLogic;
import xce.buddy.logic.async.AddFriendApplyMessage;
import xce.buddy.logic.async.AddFriendMessage;
import xce.buddy.logic.async.BlockMessage;
import xce.buddy.logic.async.DelFriendMessage;
import xce.buddy.logic.util.FriendUtil;

import com.xiaonei.admin.common.logic.user.NodeUserCommonLogic;
import com.xiaonei.antispam.AntiSpamAdapter;
import com.xiaonei.antispam.model.CheckResult;
import com.xiaonei.antispam.model.CheckType;
import com.xiaonei.antispam.security.Security;
import com.xiaonei.platform.component.friends.dao.FriendRecommendedDAO;
import com.xiaonei.platform.component.friends.dao.HotFriendDAO;
import com.xiaonei.platform.component.friends.logic.HotFriendConstants;
import com.xiaonei.platform.component.friends.logic.HotFriendLogic;
import com.xiaonei.platform.component.friends.model.HotFriend;
import com.xiaonei.platform.core.cache.MemCacheFactory;
import com.xiaonei.platform.core.cache.MemCacheKeys;
import com.xiaonei.platform.core.cache.MemCacheManager;
import com.xiaonei.platform.core.model.User;
import com.xiaonei.platform.core.model.UserConfigInfo;
import com.xiaonei.platform.core.model.WUserCache;
import com.xiaonei.platform.core.opt.permission.Permission;
import com.xiaonei.platform.core.opt.permission.PermissionFactory;
import com.xiaonei.platform.core.opt.permission.define.Module;
import com.xiaonei.platform.core.opt.permission.impl.RelationExpImpl;
import com.xiaonei.platform.core.opt.permission.source.Status;
import com.xiaonei.platform.core.opt.permission.strategy.Ret;
import com.xiaonei.sns.platform.core.opt.ice.impl.SnsAdapterFactory;
import com.xiaonei.xce.buddy.model.Constants;
import com.xiaonei.xce.buddyadapter.buddycount.BuddyCountAdapter;
import com.xiaonei.xce.buddyadapter.buddyhotcache.BuddyHotCacheAdapter;
import com.xiaonei.xce.buddyadapter.buddyhotcache.HotFriendData;
import com.xiaonei.xce.friendcountcache.FriendCountCacheAdapter;
import com.xiaonei.xce.storm.XceStormAdapter;
import com.xiaonei.xce.storm.XceStormListener;
import com.xiaonei.xce.usercount.UserCountAdapter;

/**
 * 好友相关的逻辑实现
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class FriendLogicImpl implements FriendLogic {

	private static final int TODAY_ADD_APPLY_MAX = 100;

	private static ExecutorService threadPool = Executors.newCachedThreadPool();

	public FriendLogicImpl() {
		initXceStorm();
	}

	private void initXceStorm() {
		nodeUsers = new ConcurrentHashMap<Integer, Integer>();
		nodeUserListener = new XceStormNodeUserListenerImpl();
		try {
			XceStormAdapter.getInstance().subscribe(
					XceStormNodeUserListenerImpl.key, nodeUserListener);
		} catch (Exception e) {
			// TODO: Logger 一下
		}

	}

	/**
	 * 发送好友申请接口
	 * 
	 * 步骤: <br/>
	 * Step1:取相关的两个id的User对象<br/>
	 * Step2:判断spammer<br/>
	 * Step3:判断是否超过好友申请数限制<br/>
	 * Step4:隐私判断<br/>
	 * Step5:内容过antispam<br/>
	 * Step6:调用BuddyCore服务 addApply rpc method <br/>
	 * Step7:异步发webpager, email<br/>
	 * 
	 * @param friendApply
	 * @return
	 */
	@Override
	public FriendApplyResult addFriendApply(FriendApply friendApply) {
		// friend apply logic start...
		FriendApplyResult friendApplyResult = new FriendApplyResult();
		friendApplyResult.applicant = friendApply.applicant;
		friendApplyResult.accepter = friendApply.accepter;
		User applicantUser = null;
		User accepterUser = null;
		try {
			applicantUser = SnsAdapterFactory.getUserAdapter().get(
					friendApply.applicant);
			accepterUser = SnsAdapterFactory.getUserAdapter().get(
					friendApply.accepter);
		} catch (Exception e) {
			// TODO: Logger 一下
		}

		if (applicantUser == null || accepterUser == null) {
			friendApplyResult.result = Constants.NO_USER;
			return friendApplyResult;
		}

		/** 判断spammer开始 */
		if (applicantUser.getUserDangerInfo().isNotAcceptBuddyApplication()
				|| accepterUser.getUserDangerInfo()
						.isNotAcceptBuddyApplication()) {
			friendApplyResult.result = Constants.FAKE;
			return friendApplyResult;
		}

		if (applicantUser.getUserState().isApplySpammer()
				|| accepterUser.getUserState().isApplySpammer()) {
			friendApplyResult.result = Constants.FAKE;
			return friendApplyResult;
		}
		/** 判断spammer结束 */

		/** 当天发送好友申请好限制 开始 */
		int todayAddApplyCount = 0;
		try {
			todayAddApplyCount = BuddyCountAdapter.getInstance().get(
					applicantUser.getId(), Operation.AddFriendApply.value());
		} catch (Exception e) {
			// TODO: Logger 一下
		}
		if (todayAddApplyCount > TODAY_ADD_APPLY_MAX) {
			friendApplyResult.result = Constants.OPERATION_FULL;
			return friendApplyResult;
		}
		/** 当天发送好友申请好限制 结束 */

		/** 隐私判断 开始 */
		UserConfigInfo applicantUserConfig = null;
		UserConfigInfo accepterUserConfig = null;
		try {
			applicantUserConfig = SnsAdapterFactory.getUserConfigInfoAdapter()
					.getUserConfig(friendApply.applicant);
			accepterUserConfig = SnsAdapterFactory.getUserConfigInfoAdapter()
					.getUserConfig(friendApply.accepter);
		} catch (Exception e) {
			// TODO: Logger 一下
		}
		if (applicantUserConfig == null || accepterUserConfig == null) {
			friendApplyResult.result = Constants.INVALID_USER;
			return friendApplyResult;
		}

		Permission permission = PermissionFactory.getInstance()
				.getPermissionByModule(Module.MODULE_REQUESTFRIEND);

		Status guester = new Status(applicantUser.getId(),
				applicantUser.getId(), applicantUser.getStage(),
				applicantUser.getUniv(), applicantUser.getAuth(),
				applicantUser.getStatus(),
				applicantUserConfig.getRequestFriendConfig());

		Status owner = new Status(accepterUser.getId(), accepterUser.getId(),
				accepterUser.getStage(), accepterUser.getUniv(),
				accepterUser.getAuth(), accepterUser.getStatus(),
				accepterUserConfig.getRequestFriendConfig());

		int path = RelationExpImpl.getInstance()
				.getRelationPath(guester, owner);
		guester.setFromToPath(path);
		// 由于隐私系统需要,所以只能调用user服务的过期方法!!!
		guester.setSelected(applicantUser.getSelected());
		Ret ret = permission.hasInsert(guester, owner);
		if (!ret.hasRight()) {
			friendApplyResult.result = Constants.OPERATION_PROHIBITION;
			return friendApplyResult;
		}
		/** 隐私判断 结束 */

		// 处理一下content长度
		String why = friendApply.content;
		if (why != null) {
			why = (why.length() < 45) ? why : "";
			friendApply.content = why;
		} else {
			friendApply.content = "";
		}

		// 处理一下是否包含url
		if (Security.haveURL(friendApply.content)) {
			friendApplyResult.result = Constants.INVALID_CONTENT;
			return friendApplyResult;
		}

		/** antispam 开始 */
		CheckResult cr = AntiSpamAdapter.getInstance().checkAndFilter(
				applicantUser.getId(), CheckType.FRIEND_APPLICATION_TYPE,
				friendApply.content, "");
		int m_type = cr.getFlag();
		switch (m_type) {
		case CheckResult.CONTENT_NEED_AUDIT:
			friendApply.content = cr.getSafePureContent();
			break;
		case CheckResult.SAFE:
			friendApply.content = cr.getSafePureContent();
			break;
		case CheckResult.SYSTEM_ERROR:
			friendApply.content = cr.getSafePureContent();
			break;
		case CheckResult.PROHIBITED:
			friendApply.content = cr.getSafePureContent();
			friendApplyResult.result = Constants.FAKE;
			return friendApplyResult;
		default:
			break;
		}
		/** antispam 结束 */

		// 条件都走过了,下面可以真正发了
		/** 调用BuddyCore服务 开始 */
		BuddyApplyWithInfo buddyApply = new BuddyApplyWithInfo(
				friendApply.applicant, friendApply.accepter);
		buddyApply.setTime(new Date(friendApply.timestamp));
		buddyApply.setStatFrom(friendApply.from);
		buddyApply.setGroupName("");
		buddyApply.setWhy(friendApply.content);
		try {
			AdapterFactory.getBuddyCoreAdapter().addApply(buddyApply,
					buddyApply.getStatFrom());
			friendApplyResult.result = Constants.SUCCESS_ADD_FRIEND_APPLY;
		} catch (Exception e) {
			// TODO: Logger 一下
		}
		/** 调用BuddyCore服务 结束 */

		/** 千万别忘记了当天发好友申请数加1 */
		try {
			BuddyCountAdapter.getInstance().inc(applicantUser.getId(),
					Operation.AddFriendApply.value(), 1);
		} catch (Exception e) {
			// TODO: Logger 一下
		}

		// 异步发信通知
		threadPool.execute(new AddFriendApplyMessage(applicantUser,
				accepterUser, friendApply));

		return friendApplyResult;
	}

	/**
	 * 接受好友申请接口 <br/>
	 * 步骤: <br/>
	 * Step1:取相关的两个id的User对象<br/>
	 * Step2:判断spammer<br/>
	 * Step3:判断双方的好友数<br/>
	 * Step4:调用BuddyCore服务 addFriend rpc method <br/>
	 * Step5:异步发通知,email<br/>
	 * 
	 * @param friendApply
	 * @return
	 */
	@Override
	public FriendApplyResult acceptFriendApply(FriendApply friendApply) {
		FriendApplyResult friendApplyResult = new FriendApplyResult();
		friendApplyResult.applicant = friendApply.applicant;
		friendApplyResult.accepter = friendApply.accepter;
		User applicantUser = null;
		User accepterUser = null;
		try {
			applicantUser = SnsAdapterFactory.getUserAdapter().get(
					friendApply.applicant);
			accepterUser = SnsAdapterFactory.getUserAdapter().get(
					friendApply.accepter);
		} catch (Exception e) {
			// TODO: Logger 一下
		}

		if (applicantUser == null || accepterUser == null) {
			friendApplyResult.result = Constants.NO_USER;
			return friendApplyResult;
		}

		/** 判断spammer开始 */
		if (applicantUser.getUserDangerInfo().isNotAcceptBuddyApplication()
				|| accepterUser.getUserDangerInfo()
						.isNotAcceptBuddyApplication()) {
			friendApplyResult.result = Constants.FAKE;
			// 删除此条好友申请
			BuddyApply buddyApply = new BuddyApply(applicantUser.getId(),
					accepterUser.getId());
			try {
				SnsAdapterFactory.getBuddyCoreAdapter().denyApply(buddyApply);
			} catch (Exception e) {
				// TODO: Logger 一下
			}

			return friendApplyResult;
		}
		/** 判断spammer结束 */

		/** 判断双方的好友数 开始 */
		boolean applicantReachFlag = reachMaxFriendCount(applicantUser);
		if (!applicantReachFlag) {
			friendApplyResult.result = Constants.APPLICANT_FRIEND_LIST_FULL;
			return friendApplyResult;
		}
		boolean recipientReachFlag = reachMaxFriendCount(accepterUser);
		if (!recipientReachFlag) {
			friendApplyResult.result = Constants.ACCEPTER_FRIEND_LIST_FULL;
			return friendApplyResult;
		}
		/** 判断双方的好友数 结束 */

		/** 调用BuddyCore服务 开始 */
		try {
			BuddyApply buddyApply = new BuddyApply(applicantUser.getId(),
					accepterUser.getId());
			SnsAdapterFactory.getBuddyCoreAdapter().acceptApply(buddyApply);
			friendApplyResult.result = Constants.SUCCESS_ACCEPT_FRIEND_APPLY;
		} catch (Exception e) {
			// TODO: Logger 一下
		}
		/** 调用BuddyCore服务 结束 */

		// 异步发通知,email
		threadPool.execute(new AddFriendMessage(applicantUser, accepterUser,
				friendApply));

		return friendApplyResult;
	}

	/** 结点用户本地缓存 开始 */

	private ConcurrentHashMap<Integer, Integer> nodeUsers = null;

	private XceStormListener nodeUserListener = null;

	private class XceStormNodeUserListenerImpl extends XceStormListener {

		XceStormNodeUserListenerImpl() {
			Map<Integer, Integer> map = NodeUserCommonLogic.getInstance()
					.getAllNodeUser();
			if (map != null) {
				nodeUsers.putAll(map);
			}
		}

		static final String key = "node_user_reload";

		private static final long serialVersionUID = 8987591551968122359L;

		@Override
		public void handle(Map<String, String> context) {
			try {
				nodeUsers.clear();
				Map<Integer, Integer> map = NodeUserCommonLogic.getInstance()
						.getAllNodeUser();
				if (map != null) {
					nodeUsers.putAll(map);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * 判断用户是否达到好友数上线
	 * 
	 * @param user
	 * @return
	 * @throws Exception
	 */
	private boolean reachMaxFriendCount(User user) {

		int friendCount = FriendCountCacheAdapter.getInstance().getFriendCount(
				user.getId());
		int maxCount = 0;
		if (nodeUsers.containsKey(user.getId())) {
			maxCount = nodeUsers.get(user.getId());
			return friendCount >= maxCount;
		} else {
			boolean isVip = user.isVipMember();
			if (isVip)
				return friendCount >= Constants.VIP_MAX_FRIEND_COUNT;
			else
				return friendCount >= Constants.NOT_VIP_MAX_FRIEND_COUNT;
		}

	}

	/**
	 * 加入黑名单<br/>
	 * 步骤:<br/>
	 * Step1:查看是否有权限加黑<br/>
	 * Step2:做加黑操作<br/>
	 * Step3:解除所有关系<br/>
	 * Step4:异步操作
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	@Override
	public int addBlock(int host, int guest) {
		try {
			BuddyBlock buddyBlock = new BuddyBlock(host, guest);
			SnsAdapterFactory.getBuddyCoreAdapter().addBlock(buddyBlock);
			delAllFriendRelation(host, guest);
			threadPool.execute(new BlockMessage(host, guest));
			return Constants.SUCCESS;
		} catch (Exception e) {
			// TODO: Logger
		}
		return Constants.ERROR;
	}

	/**
	 * 移出黑名单
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	@Override
	public int removeBlock(int host, int guest) {
		try {
			BuddyBlock buddyBlock = new BuddyBlock(host, guest);
			SnsAdapterFactory.getBuddyCoreAdapter().removeBlock(buddyBlock);
			return Constants.SUCCESS;
		} catch (Exception e) {
			// TODO: Logger
		}
		return Constants.ERROR;
	}

	/**
	 * 忽略所有好友申请
	 * 
	 * @param host
	 * @return
	 */
	@Override
	public int denyAllFriendApply(int host) {
		try {
			SnsAdapterFactory.getBuddyCoreAdapter().denyAllApply(host);
			return Constants.SUCCESS;
		} catch (Exception e) {
			// TODO: Logger
		}
		return Constants.ERROR;
	}

	/**
	 * 忽略好友申请
	 * 
	 * @param fa
	 * @return
	 */
	@Override
	public int denyFriendApply(FriendApply fa) {
		try {
			BuddyApply buddyApply = new BuddyApply(fa.applicant, fa.accepter);
			SnsAdapterFactory.getBuddyCoreAdapter().denyApply(buddyApply);
			return Constants.SUCCESS;
		} catch (Exception e) {
			// TODO: Logger
		}
		return Constants.ERROR;
	}

	/**
	 * 删除好友 步骤:<br/>
	 * Step1:删除所有关系<br/>
	 * Step2:当天删除好友操作计数加1<br/>
	 * Step3:异步操作<br/>
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	@Override
	public int deleteFriend(int host, int guest) {
		// 删除所有关系
		delAllFriendRelation(host, guest);
		try {
			// 当天删除好友操作计数加1
			BuddyCountAdapter.getInstance().inc(host,
					Operation.DeleteFriend.value(), 1);
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			threadPool.execute(new DelFriendMessage(host, guest));
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.SUCCESS;
	}

	private void delAllFriendRelation(int hostId, int friendId) {
		try {
			// 解除特别好友
			HotFriend hf = HotFriendDAO.getInstance().getByHostNGuest(hostId,
					friendId);
			if (hf != null) {
				HotFriendDAO.getInstance().delete(hf.getId());
				MemCacheFactory fac = MemCacheFactory.getInstance();
				MemCacheManager mgr = fac
						.getManagerByPoolName(MemCacheKeys.pool_profile);
				mgr.delete(HotFriendConstants.key + hostId);
			}
			// 双向解除
			HotFriend hf1 = HotFriendDAO.getInstance().getByHostNGuest(
					friendId, hostId);
			if (hf1 != null) {
				HotFriendDAO.getInstance().delete(hf1.getId());
				MemCacheFactory fac = MemCacheFactory.getInstance();
				MemCacheManager mgr = fac
						.getManagerByPoolName(MemCacheKeys.pool_profile);
				mgr.delete(HotFriendConstants.key + friendId);
			}

			try {
				// 删除hotfriend memcached中数据
				// TODO:写一个HotFriend服务
				HotFriendLogic.getInstance().delCache(hostId);
				HotFriendLogic.getInstance().delCache(friendId);
			} catch (Exception e) {
				e.printStackTrace();
			}

			// 解除好友关系
			BuddyRelation br = null;
			try {
				br = AdapterFactory.getBuddyCoreAdapter().getRelation(hostId,
						friendId);
				AdapterFactory.getBuddyCoreAdapter().removeFriend(br);
			} catch (Exception e) {
				e.printStackTrace();
			}

			MemCacheManager mcm = MemCacheFactory.getInstance()
					.getManagerByPoolName(MemCacheKeys.pool_default);
			try {
				mcm.delete(MemCacheKeys.home_friends + hostId);
				mcm.delete(MemCacheKeys.home_friends + friendId); // 增加双向操作
			} catch (Exception e) {
				e.printStackTrace();
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * 推荐好友接口实现<br/>
	 * matchmaker向targetUserId推荐recommendIdList，如果isOneWay为false，则为双向推荐
	 * 前提，targetUserId和recommendIdList都是matchmaker好友
	 */
	@Override
	public int recommendFriend(int matchmaker, int targetUserId,
			List<Integer> recommendIdList, boolean isOneWay, int type) {
		if (matchmaker == targetUserId) { // 自己不能向自己推荐
			return Constants.ERROR;
		}

		List<Integer> matchmakerFriends = null;
		try {
			matchmakerFriends = SnsAdapterFactory.getBuddyByIdCacheAdapter()
					.getFriendListAsc(matchmaker);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		if (!matchmakerFriends.contains(matchmaker)) { // 判断targetUserId与自己是否为好友关系
			return Constants.ERROR;
		}
		recommendIdList.retainAll(matchmakerFriends); // 去掉recommendIdList中不是自己好友

		List<Integer> userIdList = new ArrayList<Integer>();
		userIdList.add(matchmaker);
		userIdList.add(targetUserId);
		userIdList.addAll(recommendIdList);

		Map<Integer, WUserCache> userCaches = null;
		try {
			userCaches = SnsAdapterFactory.getUserCacheAdapter()
					.getUserCacheMap(userIdList);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		WUserCache matchmakerUserCache = userCaches.get(matchmaker);
		WUserCache targetUserCache = userCaches.get(targetUserId);
		if (matchmakerUserCache == null || targetUserCache == null) {
			return Constants.ERROR;
		}

		int targetCount = 0;
		for (Integer recommendId : recommendIdList) {
			WUserCache recommendUserCache = userCaches.get(recommendId);
			if (recommendUserCache == null) {
				continue;
			}
			if (isOneWay) {
				try {
					FriendRecommendedDAO.getInstance().addRecommendedOneWay(
							targetUserCache.getId(), targetUserCache.getName(),
							targetUserCache.getUnivName(),
							targetUserCache.getTiny_Url(),
							recommendUserCache.getId(),
							recommendUserCache.getName(),
							recommendUserCache.getUnivName(),
							recommendUserCache.getTiny_Url(),
							matchmakerUserCache.getId(),
							matchmakerUserCache.getName(), type);
					++targetCount;
				} catch (Exception e) {
					e.printStackTrace();
				}
			} else {
				try {
					FriendRecommendedDAO.getInstance().addRecommended(
							targetUserCache.getId(), targetUserCache.getName(),
							targetUserCache.getUnivName(),
							targetUserCache.getTiny_Url(),
							recommendUserCache.getId(),
							recommendUserCache.getName(),
							recommendUserCache.getUnivName(),
							recommendUserCache.getTiny_Url(),
							matchmakerUserCache.getId(),
							matchmakerUserCache.getName(), type);
					UserCountAdapter.getInstance().inc(
							recommendUserCache.getId(),
							UserCountAdapter.RECOMMENDED_FRIEND, 1);
					++targetCount;
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		try {
			UserCountAdapter.getInstance().inc(targetUserCache.getId(),
					UserCountAdapter.RECOMMENDED_FRIEND, targetCount);
		} catch (Exception e) {
			e.printStackTrace();
		}

		return Constants.SUCCESS;
	}

	@Override
	public int denyRecommendFriend(int host, int guest) {
		try {
			FriendRecommendedDAO.getInstance().deleteRecommended(host, guest);
		} catch (SQLException e) {
			e.printStackTrace();
			return Constants.ERROR;
		}
		return Constants.SUCCESS;
	}

	@Override
	public int denyAllRecommendFriend(int host) {
		try {
			FriendRecommendedDAO.getInstance().deleteAllRecommended(host);
		} catch (SQLException e) {
			e.printStackTrace();
			return Constants.ERROR;
		}
		return Constants.SUCCESS;
	}

	/** 为了避免在一个线程中多次取同一个id的User对象 */
	private static ThreadLocal<User> threadLocalUser = new ThreadLocal<User>();

	private static boolean hasHotFriendRight(int uid) {
		User user = null;
		try {
			user = SnsAdapterFactory.getUserAdapter().get(uid);
			threadLocalUser.set(user);
			return user.isStarUser();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	@Override
	public int addHotFriend(int host, int friend) {
		// 1.判断host是否是星级用户
		// 2.判断host与friend是否为好友关系
		// 3.rpc to hotfriendcache
		if (!hasHotFriendRight(host)) {
			return Constants.INVALID_USER;
		}

		if (!FriendUtil.isFriend(host, friend)) {
			return Constants.FAIL;
		}

		try {
			HotFriendData data = new HotFriendData(friend,
					HotFriendData.DEFAULT_MEMO);
			BuddyHotCacheAdapter.getInstance().addHotFriend(host, data);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		return Constants.SUCCESS;
	}

	@Override
	public int deleletHotFriend(int host, int friend) {
		// 1.判断host是否是星级用户
		// 2.判断host与friend是否为好友关系
		// 3.rpc to hotfriendcache
		if (!hasHotFriendRight(host)) {
			return Constants.INVALID_USER;
		}

		if (!FriendUtil.isFriend(host, friend)) {
			return Constants.FAIL;
		}

		try {
			HotFriendData data = new HotFriendData(friend,
					HotFriendData.DEFAULT_MEMO);
			BuddyHotCacheAdapter.getInstance().deleteHotFriend(host, data);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		return Constants.SUCCESS;
	}

	@Override
	public int updateHotFriendMemo(int host, int friend, String memo) {
		// 1.判断host是否是星级用户
		// 2.判断host与friend是否为好友关系
		// 3.rpc to hotfriendcache
		if (!hasHotFriendRight(host)) {
			return Constants.INVALID_USER;
		}

		if (!FriendUtil.isFriend(host, friend)) {
			return Constants.FAIL;
		}

		try {
			HotFriendData data = new HotFriendData(friend, memo);
			BuddyHotCacheAdapter.getInstance().addHotFriend(host, data);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		return Constants.SUCCESS;
	}

	@Override
	public int setHotFriend(int host, List<Integer> friends) {
		// 1.判断host是否是星级用户，星级用户最多10个特别好友，非星级6个!
		// 2.判断host与friens的好友关系，过滤掉friends列表中非好友的friend_id
		// 3.组装HotFriendData列表,处理一下position,按列表自然顺序
		// 4.rpc to hotfriendcache

		if (friends == null) {
			return Constants.ERROR;
		}

		if (!hasHotFriendRight(host)) {
			return Constants.INVALID_USER;
		}
		int maxHotFriendCount = FriendUtil.maxHotFriendCount(threadLocalUser
				.get());
		Map<Integer, Boolean> isFriendMap = FriendUtil.isFriend(host, friends);
		List<HotFriendData> dataList = new ArrayList<HotFriendData>();
		int position = 0;
		for (int i = 0; i < friends.size() && position < maxHotFriendCount; i++, position++) {
			int friend = friends.get(i);
			if (isFriendMap.containsKey(friend) && isFriendMap.get(friend)) {
				HotFriendData data = new HotFriendData(friend, position,
						System.currentTimeMillis(), HotFriendData.DEFAULT_MEMO);
				dataList.add(data);
			}
		}
		try {
			BuddyHotCacheAdapter.getInstance().setHotFriend(host, dataList);
		} catch (Exception e) {
			e.printStackTrace();
			return Constants.ERROR;
		}

		return Constants.SUCCESS;
	}

}
