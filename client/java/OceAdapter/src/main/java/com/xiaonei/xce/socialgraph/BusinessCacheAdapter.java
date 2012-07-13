package com.xiaonei.xce.socialgraph;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import xce.distcache.DistCacheAdapter;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 推荐系统对Data Source Cache的读写接口
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class BusinessCacheAdapter {

    public static final String RCD = "RCD";		//Used in home page, RCD data of home recommendation
    public static final String FFUCR = "FFUCR";		//Friend Cluster data
    public static final String FFW = "FFW";		//Common Friends data
    public static final String FFNCW = "FFNCW";		//new commer data
    public static final String FFWR = "FFWR";		//write recommendation data
    public static final String FFAG = "FFAG";		//Auto Group data
    public static final String OFSA = "OFSA";		//OfferFriends of same area
    public static final String BLOG = "BLOG";		//Recommendation of blog
    public static final String BLOGTAG = "BLOGTAG";		//tags of blog
    public static final String BLOGACTION = "BLOGACTION";		//recommendation of blog according to user's action
    public static final String USERFEATURE = "USERFEATURE";			//feature data of people
    public static final String PUK = "PUK";			//Used in profile of recommendation, people you may know
    public static final String HOTPAGE = "HOTPAGE";		//Used in profile of recommendation, recommendation of hot page
    public static final String PAGE = "PAGE";			//Page Recommendation in home
    public static final String RCDBLOG = "RCDBLOG";		//Used by recommendation of home, recommendation of blog
    public static final String RCDVIDEO = "RCDVIDEO";		//Used by recommendation of home, recommendation of video
    public static final String USERAPPLY = "USERAPPLY";		//UserApply data of recent month

    private List<String> bizs;

    private Map<String, DistCacheAdapter> dcAdaptersMap;

    private static BusinessCacheAdapter instance = null;

    public static BusinessCacheAdapter getInstance() {
	if (instance == null) {
	    synchronized (BusinessCacheAdapter.class) {
		if (instance == null) {
		    try {
			instance = new BusinessCacheAdapter();
		    } catch (Exception e) {
			e.printStackTrace();
		    }
		}
	    }
	}
	return instance;
    }

    private BusinessCacheAdapter() {
	Init();
    }

    private void Init() {
	InitBiz();
	dcAdaptersMap = new HashMap<String, DistCacheAdapter>();
	for (String biz : bizs) {
	    try {
		for (int i = 0; i < 10; ++i) {
		    DistCacheAdapter adapter = DistCacheAdapter.connect(biz);
		    if (adapter != null) {
			dcAdaptersMap.put(biz, adapter);
			break;
		    }
		    TimeUnit.MILLISECONDS.sleep(10);
		}
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
    }

    /**
     * 具体的业务名
     */
    private void InitBiz() {
	bizs = new ArrayList<String>();
	bizs.add(RCD);
	bizs.add(FFUCR);
	bizs.add(FFW);
	bizs.add(FFNCW);
	bizs.add(FFWR);
	bizs.add(FFAG);
	bizs.add(OFSA);
	bizs.add(BLOG);
	bizs.add(BLOGTAG);
	bizs.add(BLOGACTION);
	bizs.add(USERFEATURE);
	bizs.add(PUK);
        bizs.add(HOTPAGE);
        bizs.add(PAGE);
        bizs.add(RCDBLOG);
        bizs.add(RCDVIDEO);
        bizs.add(USERAPPLY);
    }

    /**
     * 写接口
     * 
     * @param biz
     * @param key
     * @param data
     * @return
     * @throws IllegalArgumentException
     */
    public boolean set(String biz, long key, byte[] data)
	    throws IllegalArgumentException {
	DistCacheAdapter adapter = dcAdaptersMap.get(biz);
	if (adapter == null) {
	    System.err.println("business:" + biz + " is not valid!!!");
	    return false;
	} else {
	    try {
		adapter.set(key, data);
		return true;
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
	return false;
    }

    /**
     * 读接口
     * 
     * @param biz
     * @param key
     * @return
     * @throws IllegalArgumentException
     */
    public byte[] get(String biz, long key) throws IllegalArgumentException {
	byte[] data = new byte[0];
	DistCacheAdapter adapter = dcAdaptersMap.get(biz);
	if (adapter == null) {
	    System.err.println("business:" + biz + " is not valid!!!");
	} else {
	    try {
		data = adapter.get(key);
		if (data == null) {
		    return new byte[0];
		}
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
	return data;
    }

    // public static void main(String[] args) throws Exception {
    // byte[] bin = new byte[0];
    // int id = 5569;
    // bin =
    // BusinessCacheAdapter.getInstance().get(BusinessCacheAdapter.USERFEATURE,
    // id);
    // bin =
    // BusinessCacheAdapter.getInstance().get(BusinessCacheAdapter.RCD,
    // id);
    // System.out.println(bin == null);
    // Items items = Items.parseFrom(bin);
    // for (Item item : items.getItemsList()) {
    // Item.Builder builder = Item.newBuilder();
    // builder.setId(item.getId());
    // builder.setMessage("FFW");
    // builder.setField(item.getField());
    // for (int i : item.getFieldsList()) {
    // builder.addFields(i);
    // }
    // // System.out.println(builder.build());
    // break;
    // }
    // System.out.println(items);
    // boolean flag =
    // BusinessCacheAdapter.getInstance().set(BusinessCacheAdapter.RCD, id,
    // items.toByteArray());
    // System.out.println("flag:" + flag);
    // System.exit(0);
    // }
}
