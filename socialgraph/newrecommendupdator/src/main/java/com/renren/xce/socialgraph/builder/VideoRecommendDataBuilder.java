package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.List;

import xce.socialgraph.recommend.video.VideoRecommendData;
import xce.socialgraph.recommend.video.VideoRecommendUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * building data of video recommend
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class VideoRecommendDataBuilder implements DataBuilder {
	private static int VIDEO_TITLE_LENGTH = 30;		//For video recommending, limit of title should be in 30 charactors
	
	public List<DataItem> getData(int hostId) {
		List<DataItem> resultList = new ArrayList<DataItem>();
		List<VideoRecommendData> videoRecommendList = VideoRecommendUtil.getInstance().getVideoRecommend(hostId, 30);
		for (VideoRecommendData data : videoRecommendList) {
			String title = data.getTitle();
			int titleLength = title.length() > VIDEO_TITLE_LENGTH ? VIDEO_TITLE_LENGTH : title.length();
			String desc = title.substring(0, titleLength);		//slice charactors
			if (titleLength != title.length()) {
				desc += "..."; 		//append "...", when title is cut
			}
			
			Item.Builder item = Item.newBuilder();
			Integer blockId = (int) data.getVideoId();		//convert long video id to int block
			item.setId(blockId);		//id stored with block id
			item.setField(data.getShareCount());		//field stored with shareCount
			item.setMessage("RCD_VIDEO");			//message stored with type identity
			item.addMessages(String.valueOf(data.getShareId()));		//messages at 0 position stored with shareVideoId
			item.addMessages(desc);			//messages at 1 position stored with title
			item.addMessages(String.valueOf(data.getShareUserId()));		//messages at 3 position stored with shareUserId
		
			DefaultDataItem videoItem = new DefaultDataItem(blockId, item);		//uniq id is video id
			resultList.add(videoItem);
		}
		
		return resultList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.RVIDEO;
	}
}
