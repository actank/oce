package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import xce.socialgraph.recommend.page.PageRecommendData;
import xce.socialgraph.recommend.page.PageRecommendUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * page data builder
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class PageBuilder implements DataBuilder {
	private static int CACHE_SHARES_SIZE = 10;		//show 10 common friends
	private static int DESCRIPTION_LENGTH = 48; 	//For page recommending, limit of description should be in 48 charactors
	
	public List<DataItem> getData(int hostId) {
		List<DataItem> pageItemList = new ArrayList<DataItem>();
		List<PageRecommendData> pageList = PageRecommendUtil.getInstance().getPageRecommend(hostId, -1);
		Collections.shuffle(pageList);
		
		for (PageRecommendData page : pageList) {
			Item.Builder item = Item.newBuilder();
			item.setId(page.getPageId());		//pageId
			item.setField(page.getFansNum());		//fansNum
			item.setMessage(CreateRecommendDataThread.PAGE);		//type
			item.addFields(page.getCommonSize());			//common size
			for (Integer shareId : page.getCommonFriends()) {
				if (item.getFieldsCount() >= CACHE_SHARES_SIZE + 1) {
					break;
				}
				item.addFields(shareId);
			}
			String description = page.getDescription();			//description
			int descLength = description.length() > DESCRIPTION_LENGTH ? DESCRIPTION_LENGTH : description.length();
			String desc = description.substring(0, descLength);		//slice charactors
			item.addMessages(desc);
			
			DefaultDataItem pageItem = new DefaultDataItem((long)page.getPageId(), item);
			pageItemList.add(pageItem);
		}
		
		return pageItemList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.PAGE;
	}
}
