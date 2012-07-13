import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import xce.socialgraph.recommend.DbUtil;
import xce.socialgraph.recommend.FriendListUtil;
import xce.socialgraph.recommend.commonfriends.CommonFriend;
import xce.socialgraph.recommend.commonfriends.CommonFriendsUtil;
import xce.socialgraph.recommend.friendcluster.Recommend;
import xce.socialgraph.recommend.friendcluster.RecommendAdapter;
import xce.socialgraph.util.Clogging;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 创建用户推荐数据线程
 * @author zhangnan
 */
public class CreateRecommendDataThread implements Runnable {
  public static final String USER_FEATURE = "USERFEATURE";		//数据源
  public static final String RECOMMEND_DATA = "RCD";
  public static final String FREINDS_CLUSTER = "FFUCR";
  public static final String COMMON_FRIENDS = "FFW";
  public static final String NEW_COMMER = "FFNCW";
  public static final String RECONNECT = "FFWR";
  public static final String AUTO_GROUP = "FFAG";
  public static final String SAME_AREA = "OFSA";
  public static final String BLOG = "BLOG";
  public static final String BLOG_TAG = "BLOGTAG";
  public static final String BLOG_ACTION = "BLOGACTION";
  
  public static final String USER_TYPE_FAVOR = "FAVOR";		//用户属性
  public static final String USER_TYPE_CHARACTER = "CHARACTER";
	
//	private static int CF_SAVE_THRESHOLD_DISTCACHE = 24;		//首页展示24个推荐位
	private static int CF_SAVE_COMMON_SIZE = 12;		//二度好友预占用12个推荐位
	private static int CF_SAVE_CLUSTER_SIZE = 12;		//好友簇预占用12个推荐位
	private static int CACHE_SHARES_SIZE = 10;		//共同好友只展示10个
	
	private int userId = 0;		//用户id
	
	public CreateRecommendDataThread(int userId) {
		this.userId = userId;
	}

	/**
	 * 区分不同类型用户创建不同推荐数据
	 */
	public void run() {
		Long startTime = System.currentTimeMillis();
		byte[] featureData = BusinessCacheAdapter.getInstance().get(USER_FEATURE, userId);
		try {
			int type = 1;
			if (null != featureData && featureData.length != 0) {
				Items userFeature = Items.parseFrom(featureData);
				for (Item each : userFeature.getItemsList()) {
					if (USER_TYPE_FAVOR.equals(each.getMessage())) {
						type = each.getField();
						break;
					}
				}
			}
			
			byte[] resultData = null;
			if (1 == type) {	//愿意使用右侧推荐位
//				resultData = getDataByFavorStrategy();
				resultData = getDataByUnFavorStrategy();		//改用新策略，全部用户使用随即数据
			} else {		//不愿意使用右侧推荐位
				resultData = getDataByUnFavorStrategy();
			}
			boolean insert = false;
			if (null != resultData && resultData.length != 0) {
				insert = BusinessCacheAdapter.getInstance().set(RECOMMEND_DATA, userId, resultData);
			}
			Long endTime = System.currentTimeMillis();
			if (insert) {
				Clogging.Debug("userId:" + userId + " Success resultData size:"
						+ resultData.length + " cost_time:" + (endTime - startTime) + "ms");
			} else {
				Clogging.Debug("userId:" + userId + " Fail resultData size:"
						+ resultData.length + " cost_time:" + (endTime - startTime) + "ms");
			}
			resultData = null;
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		featureData = null;
	}
	
	/**
	 * 喜欢使用右侧推荐位类型用户的推荐策略
	 * @return
	 */
	private byte[] getDataByFavorStrategy() {
		List<CommonFriend> commList = CommonFriendsUtil.getInstance().getCommonFriends(userId, -1);
		List<Recommend> friendClusterList = RecommendAdapter.getInstance().getRecommendList(userId);
		
		return getRecommendResult(commList, friendClusterList);
	}
	
	/**
	 * 不喜欢右侧推荐位类型用户的推荐策略
	 * @return
	 */
	private byte[] getDataByUnFavorStrategy() {
		List<CommonFriend> commList = CommonFriendsUtil.getInstance().getCommonFriends(userId, -1);
		List<Recommend> friendClusterList = RecommendAdapter.getInstance().getRecommendList(userId);
		Collections.shuffle(commList);		//加入随机策略
		Collections.shuffle(friendClusterList);		//加入随机策略
		
		return getRecommendResult(commList, friendClusterList);
	}
	
	/**
	 * 依据二度好友和好友簇数据生成推荐数据
	 * @param commList
	 * @param friendClusterList
	 * @return
	 */
	private byte[] getRecommendResult(List<CommonFriend> commList, List<Recommend> friendClusterList) {
		byte[] data = null;
		
		int clusterSize = friendClusterList.size();		//整合二度好友和好友簇数据的推荐个数
		int leftSize = 0;
		if (clusterSize < CF_SAVE_CLUSTER_SIZE) {
			leftSize = CF_SAVE_CLUSTER_SIZE - clusterSize;
		} else {
			clusterSize = CF_SAVE_CLUSTER_SIZE;
		}
		int commSize = CF_SAVE_COMMON_SIZE + leftSize;
		
		Set<Long> filterSet = new HashSet<Long>();	//过滤集合，过滤block表中数据，和各个数据源中的重复数据
		List<Long> blockList = DbUtil.getInstance().getSgBlockList(userId);
		filterSet.addAll(blockList);

		List<Integer> friendList = FriendListUtil.getInstance().getFriendList(userId, -1);	//过滤好友列表
		for (Integer each : friendList) {
			filterSet.add((long)each);
		}
		
		Items.Builder result = Items.newBuilder();
		for (int i = 0; i < commSize && i < commList.size(); ++i) {		//处理二度好友数据
			if (filterSet.contains((long)commList.get(i).getUserId())) {
				continue;
			} else {
				filterSet.add((long) commList.get(i).getUserId());		//添加进入过滤集合
			}
			Item.Builder item = Item.newBuilder();
			item.setId(commList.get(i).getUserId());
			item.setField(commList.get(i).getsharesCount());	
			item.setMessage(COMMON_FRIENDS);		//数据源种类
			for (Integer shareId : commList.get(i).getShares()) {
				if (item.getFieldsCount() >= CACHE_SHARES_SIZE) {
					break;
				}
				item.addFields(shareId);
			}
			result.addItems(item);
		}
		
		for (int i = 0; i < clusterSize && i < friendClusterList.size(); ++i) {		//处理好友簇数据
			if (filterSet.contains((long)friendClusterList.get(i).getRcdId())) {		//已有该用户推荐
				continue;
			}
			Item.Builder item = Item.newBuilder();
			item.setId(friendClusterList.get(i).getRcdId());
			item.setField(friendClusterList.get(i).getCommonFriendCount());
			item.setMessage(FREINDS_CLUSTER);		//数据源种类
			for (Integer shareId : friendClusterList.get(i).getCommonFriendList()) {
				if (item.getFieldsCount() >= CACHE_SHARES_SIZE) {
					break;
				}
				item.addFields(shareId);
			}
			result.addItems(item);
		}
		data = result.build().toByteArray();
		commList = null;
		friendClusterList = null;
		
		return data;
	}
	
	@SuppressWarnings("unused")
	private void PrintItems(Items items) {
		for (Item item : items.getItemsList()) {
			Clogging.Debug(item.getId() + ":" + item.getField() + ":" + item.getMessage());
			String total = "";
			for (Integer shareId : item.getFieldsList()) {
				total += shareId + " ";
			}
			Clogging.Debug(total);
		}
	}
}
