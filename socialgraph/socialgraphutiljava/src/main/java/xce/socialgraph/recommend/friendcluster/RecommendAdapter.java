package xce.socialgraph.recommend.friendcluster;

import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 好友簇推荐数据接口的适配器 <br/>
 * 做推荐系统中数据源的数据，封装一层，以方便写业务的人使用
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class RecommendAdapter {

    private static RecommendAdapter instance = null;

    private RecommendAdapter() {

    }

    public static RecommendAdapter getInstance() {
	if (instance == null) {
	    synchronized (RecommendAdapter.class) {
		if (instance == null) {
		    instance = new RecommendAdapter();
		}
	    }
	}
	return instance;
    }

    /**
     * 取好友簇推荐列表接口
     * 
     * @param hostId
     * @return
     */
    public List<Recommend> getRecommendList(int hostId) {
	List<Recommend> rcdList = new ArrayList<Recommend>();
	byte[] binData = BusinessCacheAdapter.getInstance()
		.get("FFUCR", hostId);
	if (binData == null || binData.length == 0) {
	    return rcdList;
	}
	try {
	    Items items = Items.parseFrom(binData);
	    for (Item item : items.getItemsList()) {
		int host = hostId;
		int rcdId = item.getId();
		int commonFriendCount = item.getField();
		List<Integer> commonFriendList = item.getFieldsList();
		Recommend recommend = new Recommend(host, rcdId,
			commonFriendCount, commonFriendList);
		rcdList.add(recommend);
	    }
	} catch (InvalidProtocolBufferException e) {
	    e.printStackTrace();
	}
	return rcdList;
    }
}
