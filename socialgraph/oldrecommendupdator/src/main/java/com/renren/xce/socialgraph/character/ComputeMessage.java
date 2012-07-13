package com.renren.xce.socialgraph.character;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import xce.socialgraph.recommend.DbUtil;
import xce.socialgraph.recommend.commonfriends.CommonFriend;
import xce.socialgraph.recommend.commonfriends.CommonFriendsUtil;
import xce.socialgraph.recommend.recommendbyinfo.RecommendByInfoUtil;
import xce.socialgraph.util.Clogging;

import com.google.protobuf.InvalidProtocolBufferException;
import com.renren.xce.socialgraph.userfeature.async.UserFeatureMessage;
import com.xiaonei.platform.core.model.WUserCache;
import com.xiaonei.sns.platform.core.opt.ice.impl.SnsAdapterFactory;
import com.xiaonei.vip.dao.item.ItemDAO;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 个性化策略的计算程序
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class ComputeMessage implements Runnable {

	private final int RCD_COUNT = 30;

	private int userId;

	private Map<Integer, Item> rcdDataMap;

	private List<Integer> ffwRcdList;
	private List<Integer> ffucrRcdList;

	private List<Integer> univRcdList;
	private List<Integer> highSchoolRcdList;
	private List<Integer> juniorSchoolRcdList;
	private List<Integer> eleSchoolRcdList;

	private Set<Integer> blockSet;

	private Map<Integer, WUserCache> userCacheMap;

	private boolean newStrategy;

	// 维度
	private List<Dimension> dimensions;

	public ComputeMessage(int userId, boolean ns) {
		this.userId = userId;
		rcdDataMap = new HashMap<Integer, Item>();
		dimensions = new ArrayList<Dimension>();
		ffwRcdList = new ArrayList<Integer>();
		ffucrRcdList = new ArrayList<Integer>();
		univRcdList = new ArrayList<Integer>();
		highSchoolRcdList = new ArrayList<Integer>();
		juniorSchoolRcdList = new ArrayList<Integer>();
		eleSchoolRcdList = new ArrayList<Integer>();
		blockSet = new HashSet<Integer>();
		blockSet.add(userId);
		userCacheMap = new HashMap<Integer, WUserCache>();
		this.newStrategy = ns;
	}

	public int getUserId() {
		return userId;
	}

	public boolean isNewStrategy() {
		return newStrategy;
	}

	public void run() {
		// 1.取userfeature，计算取不同类型的数据的比例
		// 2.取数据. remove block
		// 3.装RCD数据 写RCD cache
		long begin = System.currentTimeMillis();
		stepOne();
		long s1 = System.currentTimeMillis();
		// Clogging.Debug("stepOne timecost:" + (s1 - begin));
		stepTwo();
		long s2 = System.currentTimeMillis();
		// Clogging.Debug("stepTwo timecost:" + (s2 - s1));
		stepThree();
		long s3 = System.currentTimeMillis();
		// Clogging.Debug("stepThree timecost:" + (s3 - s2));
		Clogging.Info("userId:" + userId + "\ttimecost:" + (s3 - begin) + "ms");
	}

	private void stepOne() {
		if (!isNewStrategy()) {
			return;
		}
		byte[] bin = new byte[0];
		bin = BusinessCacheAdapter.getInstance().get(
				BusinessCacheAdapter.USERFEATURE, userId);
		double totalScore = 0;
		try {
			Items userfeatureItems = Items.parseFrom(bin);

			for (Item item : userfeatureItems.getItemsList()) {
				if (UserFeatureMessage.isTarget(item.getMessage())) {
					if (item.getId() != 0) {
						totalScore += item.getId();
						dimensions.add(new Dimension(item.getId(), item
								.getMessage()));
					}
				}
			}
		} catch (InvalidProtocolBufferException e) {
			Clogging.Error(e.toString());
		}
		if (dimensions.isEmpty() || totalScore == 0) {
			return;
		}
		// 排序
		Collections.sort(dimensions, new Comparator<Dimension>() {
			public int compare(Dimension o1, Dimension o2) {
				// 降序 score由高到低
				return o1.getScore() >= o2.getScore() ? -1 : 1;
			}
		});
		for (Dimension d : dimensions) {
			int rcdCount = (int) (RCD_COUNT * d.getScore() / totalScore);
			d.setRcdCount(rcdCount);
		}
	}

	private void stepTwo() {
		// 取sg_block，FFW FFUCR数据源， 取use cache 分性别
		// block
		long begin = System.currentTimeMillis();
		List<Long> list = DbUtil.getInstance().getSgBlockList(userId);
		for (Long l : list) {
			blockSet.add((int) l.longValue());
		}
		long sb = System.currentTimeMillis();
		// Clogging.Debug("get block timecost:" + (sb - begin));
		// FFW
		try {
			List<CommonFriend> cfList = CommonFriendsUtil.getInstance()
					.getCommonFriends(userId, -1);
			for (CommonFriend cf : cfList) {
				if (!blockSet.contains(cf.getUserId())) {
					Item.Builder itemBuilder = Item.newBuilder();
					itemBuilder.setId(cf.getUserId());
					itemBuilder.setMessage("FFW");
					itemBuilder.setField(cf.getsharesCount());
					for (int i : cf.getShares()) {
						itemBuilder.addFields(i);
					}
					Item rcdItem = itemBuilder.build();
					rcdDataMap.put(rcdItem.getId(), rcdItem);
					ffwRcdList.add(rcdItem.getId());
				}
			}
		} catch (Exception e) {
			Clogging.Error(e.toString());
		}
		long sb2 = System.currentTimeMillis();
		// Clogging.Debug("get ffw timecost:" + (sb2 - sb));
		// FFUCR
		byte[] bin = new byte[0];
		try {
			bin = BusinessCacheAdapter.getInstance().get(
					BusinessCacheAdapter.FFUCR, userId);
			Items ffwItems = Items.parseFrom(bin);
			for (Item item : ffwItems.getItemsList()) {
				if (!blockSet.contains(item.getId())) {
					Item.Builder itemBuilder = Item.newBuilder();
					itemBuilder.setId(item.getId());
					itemBuilder.setMessage("FFUCR");
					itemBuilder.setField(item.getField());
					for (int i : item.getFieldsList()) {
						itemBuilder.addFields(i);
					}
					Item rcdItem = itemBuilder.build();
					rcdDataMap.put(rcdItem.getId(), rcdItem);
					ffucrRcdList.add(rcdItem.getId());
				}
			}
		} catch (Exception e) {
			Clogging.Error(e.toString());
		}

		long sb3 = System.currentTimeMillis();
		// Clogging.Debug("get ffucr timecost:" + (sb3 - sb2));
		if (!isNewStrategy()) {
			return;
		}

		// 取UserCache
		Set<Integer> idSet = new HashSet<Integer>();
		idSet.add(userId);
		idSet.addAll(ffwRcdList);
		idSet.addAll(ffucrRcdList);
		try {
			userCacheMap = SnsAdapterFactory.getUserCacheAdapter()
					.getUserCacheMap(new ArrayList<Integer>(idSet));
		} catch (Exception e) {
			Clogging.Error(e.toString());
		}

		// 按资料推荐
		List<Integer> datumRcdList = RecommendByInfoUtil.getInstance()
				.getUnivFriends(userId);
		// 大学
		univRcdList.addAll(datumRcdList);
		if (!univRcdList.isEmpty()) {
			for (Integer id : univRcdList) {
				Item.Builder builder = Item.newBuilder();
				builder.setId(id);
				builder.setMessage("UNIV");
				rcdDataMap.put(id, builder.build());
			}
		}
		// 高中
		datumRcdList.clear();
		datumRcdList = RecommendByInfoUtil.getInstance().getHighSchoolFriends(
				userId);
		highSchoolRcdList.addAll(datumRcdList);
		if (!highSchoolRcdList.isEmpty()) {
			for (Integer id : highSchoolRcdList) {
				Item.Builder builder = Item.newBuilder();
				builder.setId(id);
				builder.setMessage("HIGHSCHOOL");
				rcdDataMap.put(id, builder.build());
			}
		}
		// 初中
		datumRcdList.clear();
		datumRcdList = RecommendByInfoUtil.getInstance()
				.getJuniorSchoolFriends(userId);
		juniorSchoolRcdList.addAll(datumRcdList);
		if (!juniorSchoolRcdList.isEmpty()) {
			for (Integer id : juniorSchoolRcdList) {
				Item.Builder builder = Item.newBuilder();
				builder.setId(id);
				builder.setMessage("JUNIORSCHOOL");
				rcdDataMap.put(id, builder.build());
			}
		}
		// 小学
		datumRcdList.clear();
		datumRcdList = RecommendByInfoUtil.getInstance()
				.getElementarySchoolFriends(userId);
		eleSchoolRcdList.addAll(datumRcdList);
		if (!eleSchoolRcdList.isEmpty()) {
			for (Integer id : eleSchoolRcdList) {
				Item.Builder builder = Item.newBuilder();
				builder.setId(id);
				builder.setMessage("ELESCHOOL");
				rcdDataMap.put(id, builder.build());
			}
		}

	}

	private void stepThree() {
		// 默认策略
		if (dimensions.isEmpty() || !isNewStrategy()) {
			List<Item> rcdItemList = new ArrayList<Item>();
			int loop = 0;
			for (Entry<Integer, Item> entry : rcdDataMap.entrySet()) {
				rcdItemList.add(entry.getValue());
				++loop;
				if (loop >= RCD_COUNT) {
					break;
				}
			}
			Collections.shuffle(rcdItemList);
			Items.Builder builder = Items.newBuilder();
			for (Item item : rcdItemList) {
				builder.addItems(item);
			}
			Items rcdItems = builder.build();
			BusinessCacheAdapter.getInstance().set(BusinessCacheAdapter.RCD,
					userId, rcdItems.toByteArray());
			return;
		}

		// 个性化策略
		List<Item> rcdItemList = new ArrayList<Item>();
		WUserCache hostUserCache = userCacheMap.get(userId);
		for (Dimension dimension : dimensions) {
			String dname = dimension.getName();
			int rcount = dimension.getRcdCount();
			if (UserFeatureMessage.GENDER.equals(dname)) {

				int loop = 0;
				List<Integer> removeList = new ArrayList<Integer>();
				for (Entry<Integer, Item> entry : rcdDataMap.entrySet()) {
					WUserCache guestUserCache = userCacheMap
							.get(entry.getKey());
					if (guestUserCache == null) {
						continue;
					}

					if (!guestUserCache.getGender().equals(
							hostUserCache.getGender())) {
						rcdItemList.add(entry.getValue());
						removeList.add(entry.getKey());
						++loop;
						if (loop >= rcount) {
							break;
						}
					}

				}

				ffwRcdList.removeAll(removeList);
				ffucrRcdList.removeAll(removeList);
				for (Integer id : removeList) {
					rcdDataMap.remove(id);
				}

			} else if (UserFeatureMessage.BIRTH.equals(dname)) {

			} else if (UserFeatureMessage.CITY.equals(dname)) {

			} else if (UserFeatureMessage.ELE_SCHOOL.equals(dname)) {
				List<Integer> removeList = new ArrayList<Integer>();
				int loop = 0;
				for (Integer rid : eleSchoolRcdList) {
					Item item = rcdDataMap.get(rid);
					if (item != null) {
						rcdItemList.add(item);
						removeList.add(rid);
					}
					++loop;
					if (loop >= rcount) {
						break;
					}
				}
				eleSchoolRcdList.removeAll(removeList);
				for (Integer id : removeList) {
					rcdDataMap.remove(id);
				}
			} else if (UserFeatureMessage.HIGH_SCHOOL.equals(dname)) {
				List<Integer> removeList = new ArrayList<Integer>();
				int loop = 0;
				for (Integer rid : highSchoolRcdList) {
					Item item = rcdDataMap.get(rid);
					if (item != null) {
						rcdItemList.add(item);
						removeList.add(rid);
					}
					++loop;
					if (loop >= rcount) {
						break;
					}
				}
				highSchoolRcdList.removeAll(removeList);
				for (Integer id : removeList) {
					rcdDataMap.remove(id);
				}

			} else if (UserFeatureMessage.UNIV.equals(dname)) {
				List<Integer> removeList = new ArrayList<Integer>();
				int loop = 0;
				for (Integer rid : univRcdList) {
					Item item = rcdDataMap.get(rid);
					if (item != null) {
						rcdItemList.add(item);
						removeList.add(rid);
					}
					++loop;
					if (loop >= rcount) {
						break;
					}
				}
				univRcdList.removeAll(removeList);
				for (Integer id : removeList) {
					rcdDataMap.remove(id);
				}

			} else if (UserFeatureMessage.WORKPLACE.equals(dname)) {

			}
		}

		if (rcdItemList.size() < RCD_COUNT) {
			List<Integer> removeList = new ArrayList<Integer>();
			for (Entry<Integer, Item> entry : rcdDataMap.entrySet()) {
				removeList.add(entry.getKey());
				rcdItemList.add(entry.getValue());
				if (rcdItemList.size() >= RCD_COUNT) {
					break;
				}
			}
		}
		Collections.shuffle(rcdItemList);
		Items.Builder itemsbuilder = Items.newBuilder();
		for (Item item : rcdItemList) {
			itemsbuilder.addItems(item);
		}
		Items rcdItems = itemsbuilder.build();
		// Clogging.Info(rcdItems.toString());
		BusinessCacheAdapter.getInstance().set(BusinessCacheAdapter.RCD,
				userId, rcdItems.toByteArray());
	}

	private class Dimension {
		double score;
		String name;
		int rcdCount;

		public Dimension(double score, String name) {
			this.score = score;
			this.name = name;
		}

		public double getScore() {
			return score;
		}

		public void setScore(double score) {
			this.score = score;
		}

		public String getName() {
			return name;
		}

		public void setName(String name) {
			this.name = name;
		}

		public int getRcdCount() {
			return rcdCount;
		}

		public void setRcdCount(int rcdCount) {
			this.rcdCount = rcdCount;
		}

		public String toString() {
			StringBuffer sb = new StringBuffer();
			sb.append("score:").append(score).append("\t");
			sb.append("name:").append(name).append("\t");
			sb.append("rcdCount:").append(rcdCount);
			return sb.toString();
		}
	}

	public static void main(String[] args) {
		new ComputeMessage(27640369, true).run();
		System.exit(0);
	}
}
