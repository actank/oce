package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.List;

import xce.socialgraph.recommend.blog.BlogRecommendData;
import xce.socialgraph.recommend.blog.BlogRecommendUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * building data of blog recommend
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class BlogRecommendDataBuilder implements DataBuilder {
	private static int BLOG_TITLE_LENGTH = 30;		//For blog recommending, limit of title should be in 30 charactors

	public List<DataItem> getData(int hostId) {
		List<DataItem> resultList = new ArrayList<DataItem>();
		List<BlogRecommendData> blogRecommendList = BlogRecommendUtil.getInstance().getBlogRecommend(hostId, 30);
		
		for (BlogRecommendData data : blogRecommendList) {
			String title = data.getTitle();
			int titleLength = title.length() > BLOG_TITLE_LENGTH ? BLOG_TITLE_LENGTH : title.length();
			String desc = title.substring(0, titleLength);		//slice charactors
			if (titleLength != title.length()) {
				desc += "..."; 		//append "...", when title is cut
			}
			
			Item.Builder item = Item.newBuilder();
			Integer blockId = (int) data.getBlogId();		//convert long blog id to int block
			item.setId(blockId);		//id stored with block id
			item.setField(data.getShareCount());		//field stored with shareCount
			item.setMessage("RCD_BLOG");			//message stored with type identity
			item.addMessages(String.valueOf(data.getShareId()));		//messages at 0 position stored with shareBlogId
			item.addMessages(desc);			//messages at 1 position stored with title
			item.addMessages(String.valueOf(data.getShareUserId()));		//messages at 2 position stored with shareUserId
			
			DefaultDataItem blogItem = new DefaultDataItem(blockId, item);		//uniq id is blog id
			resultList.add(blogItem);
		}
		
		return resultList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.RBLOG;
	}

}
