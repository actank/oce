package com.renren.xce.socialgraph.updator;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import xce.socialgraph.util.Clogging;

import com.renren.xce.socialgraph.block.BlockFetchWithOwnerFriends;
import com.renren.xce.socialgraph.block.SgBlockFetcher;
import com.renren.xce.socialgraph.builder.BuilderFactory;
import com.renren.xce.socialgraph.common.BlockFetcher;
import com.renren.xce.socialgraph.common.Category;
import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.common.Strategy;
import com.renren.xce.socialgraph.strategy.GeneralStrategy;
import com.renren.xce.socialgraph.strategy.GeneralStrategyWithoutContent;
import com.renren.xce.socialgraph.strategy.UserApplyStrategy;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
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
  public static final String PAGE = "PAGE";
  public static final String INFO = "INFO";
  public static final String RVIDEO = "RCD_VIDEO";
  public static final String RBLOG = "RCD_BLOG";
  
  public static final String AUTO_GROUP = "FFAG";
  public static final String SAME_AREA = "OFSA";
  public static final String BLOG = "BLOG";
  public static final String BLOG_TAG = "BLOGTAG";
  public static final String BLOG_ACTION = "BLOGACTION";
  
	private Integer userId;
	
	public CreateRecommendDataThread(Integer userId) {
		this.userId = userId;
	}

	public void run() {
		try {
			Process(this.userId); 		// process each user
		} catch (Exception e) {
			e.printStackTrace();
			Clogging.Warn("Process::run exception userid:" + this.userId);
		} finally {
			this.userId = null;
		}
	}
	
	/**
	 * process each person 
	 * @param userId
	 */
	private void Process(int userId) {
		Long startTime = System.currentTimeMillis();
		
		Strategy strategy = new UserApplyStrategy();	//use user's apply status for strategy of recommendation
		List<Category> categoryList = strategy.getStrategy(userId);
		
		List<DataBuilder> builderList = new ArrayList<DataBuilder>();  	//builderList for creating data by different type
		for (Category each : categoryList) {
			DataBuilder builder = BuilderFactory.createBuilder(each.getType());
			builderList.add(builder);
		}
		
		if (categoryList.size() != builderList.size()) {
			Clogging.Error("[Process] size of categoryList is not as same as size of builderList!!");
			return;
		}
		
		BlockFetcher blockFetcher = new BlockFetchWithOwnerFriends();	//socialgraph block table and friend list
		List<Long> blockList = blockFetcher.getBlockList(userId);		//fetch block list from specific fetcher
		Set<Long> filterSet = new HashSet<Long>();
		filterSet.addAll(blockList);
		
		Items.Builder result = Items.newBuilder();
		int typeSize = 0;		//number of each type, changing according to size of pre-type 
		
		for (int i = 0; i < categoryList.size(); ++i) {
			typeSize += categoryList.get(i).getNum();			//get size of current type
			
			if (null == builderList.get(i)
					|| categoryList.get(i).getType() != builderList.get(i)
							.getBuilderType()) {
				continue;
			}
			
			int realTypeSize = 0;
			List<DataItem> dataList = builderList.get(i).getData(userId);			//create data
			for (DataItem it : dataList) {				//combine data of the type
				if (filterSet.contains(it.getId())) {
					continue;
				} else {
					if (realTypeSize < typeSize) {
						filterSet.add(it.getId());
						result.addItems(it.getItem());
						++realTypeSize;
					} else {
						break;
					}
				}
			}
			typeSize = typeSize - realTypeSize;		//left remnant to next type 
			dataList = null;			//clean
		}
		
		byte[] resultData = result.build().toByteArray();
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
    
    result = null;			//clean
    resultData = null;
    strategy = null;
    categoryList = null;
    builderList = null;
    blockFetcher = null;
    filterSet = null;
    blockList = null;
	}
	
	/**
	 * Add function of white list
	 * @param userId
	 * @return
	 */
	private boolean isInWhiteList(int userId) {
		boolean result = false;
		
		Set<Integer> whiteSet = new HashSet<Integer>();
		whiteSet.add(229406110);		//李伟
		whiteSet.add(128487631);		//张铁安
		whiteSet.add(222677625);		//高雅楠
		whiteSet.add(723180044);		//秦帅
		whiteSet.add(238111132);		//张楠
		whiteSet.add(44674);			//张叶银
		whiteSet.add(256404211);			//熊军武
		whiteSet.add(231815969);		//袁源
		whiteSet.add(85432256);		//黄晶
		whiteSet.add(142443436);		//洪振亚
		whiteSet.add(299213989);		//李金山
		if (whiteSet.contains(userId)) {
			result = true;
		}
		return result;
	}
	
//  For test
//	private void PrintCategoryList(List<Category> categoryList) {
//		for (Category category : categoryList) {
//			Clogging.Debug(category.getType() + " : " + category.getNum());
//		}
//	}
//	private void PrintItems(Items.Builder result) {
//		for (Item item : result.getItemsList()) {
//			Clogging.Debug(item.getMessage());
//			Clogging.Debug("" + item.getId());
//			Clogging.Debug("" + item.getField());
//			String messagesList = "";
//			for (String mes : item.getMessagesList()) {
//				messagesList += mes + " ";
//			}
//			Clogging.Debug(messagesList);
//			String fieldsList = "";
//			for (Integer fid : item.getFieldsList()) {
//				fieldsList += fid + " ";
//			}
//			Clogging.Debug(fieldsList);
//			
//			Clogging.Debug("---------------------------------------------------------------------");
//		}
//	}
}
