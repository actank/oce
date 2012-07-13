package com.xiaonei.xce.shareindexlogic;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.lang.Math;
import Ice.ObjectPrx;
import java.lang.String;

import com.xiaonei.search5.logicserver.IndexUpdateData;
import com.xiaonei.search5.logicserver.IndexUpdateLogicPrx;
import com.xiaonei.search5.logicserver.IndexUpdateLogicPrxHelper;
import com.xiaonei.search5.logicserver.BusinessType;


import xce.util.channel.Channel;
import xce.clusterstate.ReplicatedClusterAdapter;

import xce.util.tools.MathUtil;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public class ShareIndexLogicAdapter extends  ReplicatedClusterAdapter {
  
  public static final int THREAD_SIZE = 8;

  private static final String sil_endpoints = "ControllerShareIndexLogic";

  private static int _interval = 120;

  private Vector<IndexUpdateLogicPrx> managers = new Vector<IndexUpdateLogicPrx>(); 
  
  private ShareIndexLogicAdapter()  {

    super(sil_endpoints, _interval);

  }

  private static ShareIndexLogicAdapter _instance = new ShareIndexLogicAdapter();

  public static ShareIndexLogicAdapter getInstance()  {
    return _instance;
  }


  protected IndexUpdateLogicPrx getShareIndexLogicManagerOneway(int index)  {
//    Ice.ObjectPrx prx0 = getValidProxy(index, 300);
    @SuppressWarnings("unchecked")
    List<Ice.ObjectPrx> prx1 = getAllProxy(index);
    if (prx1 != null && prx1.size() > 0) {
      IndexUpdateLogicPrx prx = IndexUpdateLogicPrxHelper.uncheckedCast(prx1.get(0).ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }
  }



 public void update(IndexUpdateData info)  {
    try  {
      IndexUpdateLogicPrx tmp = ShareIndexLogicAdapter.getInstance().getShareIndexLogicManagerOneway(0);
      if(tmp != null)  {
           tmp.update(info);
        }  else  {
           return;
        }
    } catch(Exception e)  {
      e.printStackTrace();
    }
  }

  protected boolean verifyProxy(ObjectPrx proxy) {
      return IndexUpdateLogicPrxHelper.uncheckedCast(proxy).isValid();
  } 
  
  public static void main(String[] args)  {
    IndexUpdateData info0 = new IndexUpdateData();
    IndexUpdateData info1 = new IndexUpdateData();
    IndexUpdateData info2 = new IndexUpdateData();
    int RANGE = 1000000000;

    info0.type = BusinessType.Share;
    info0.operationType = 0;
    info0.index = new HashMap<String, String>(); 
    info0.index.put("md5", "0998989897878");
    info0.index.put("url", "http://www.renren.com/home?id=25701953");
    info0.index.put("type", "2");
    info0.index.put("resourceId", "234343544");
    info0.index.put("resourceUserId", "25701953");
    info0.index.put("thumbUrl", "mythumburl");
    info0.index.put("title", "火龙果的故事");
    info0.index.put("summary", "bug bug bug");
    info0.index.put("commentCount", "20");
    info0.index.put("viewCount", "30");
    info0.index.put("zanCount", "34");
    info0.index.put("shareCount", "43");
    info0.index.put("collectionCount", "23");
    info0.index.put("status", "2");
    info0.index.put("creationData", "20101022");
    
    info1.type = BusinessType.Share;
    info1.operationType = 0;
    info1.index = new HashMap<String, String>(); 
    info1.index.put("md5", "12121212121121");
    info1.index.put("url", "http://www.renren.com/home?id=25701953");
    info1.index.put("type", "2");
    info1.index.put("resourceId", "234343544");
    info1.index.put("resourceUserId", "25701953");
    info1.index.put("thumbUrl", "mythumburl");
    info1.index.put("title", "艳二雄和蛋蛋的故事");
    info1.index.put("summary", "bug bug bug");
    info1.index.put("commentCount", "20");
    info1.index.put("viewCount", "30");
    info1.index.put("zanCount", "34");
    info1.index.put("shareCount", "43");
    info1.index.put("collectionCount", "23");
    info1.index.put("status", "2");
    info1.index.put("creationData", "20101022");

    info2.type = BusinessType.Share;
    info2.operationType = 0;
    info2.index = new HashMap<String, String>(); 
    info2.index.put("md5", "232323232311231");
    info2.index.put("url", "http://www.renren.com/home?id=25701953");
    info2.index.put("type", "2");
    info2.index.put("resourceId", "234343544");
    info2.index.put("resourceUserId", "25701953");
    info2.index.put("thumbUrl", "mythumburl");
    info2.index.put("title", "中山舰");
    info2.index.put("summary", "bug bug bug");
    info2.index.put("commentCount", "20");
    info2.index.put("viewCount", "30");
    info2.index.put("zanCount", "34");
    info2.index.put("shareCount", "43");
    info2.index.put("collectionCount", "23");
    info2.index.put("status", "2");
    info2.index.put("creationData", "20101022");

    for(int i = 0; i < 1000; i++)  {
    System.out.println("info0 title: "+ info0.index.get("title") + " info1 title: "+ info1.index.get("title") + " info2 title: "+ info2.index.get("title"));
    long id0 = (long)(Math.random()*RANGE);
    int userid0 = (int)(Math.random()*RANGE);
    info0.index.put("shareId", Long.toString(id0));
    info0.index.put("shareUserId", Integer.toString(userid0));
    
    ShareIndexLogicAdapter.getInstance().update(info0);
    System.out.println("1");
    try  {    
      System.out.println("2");
      Thread.sleep(500);
      System.out.println("3");
    }  catch(Exception e)  {
     e.printStackTrace();
    } 
      System.out.println("4");
   
    long id1 = (long)(Math.random()*RANGE); 
    int userid1 = (int)(Math.random()*RANGE);
    info1.index.put("shareId", Long.toString(id1));
    info1.index.put("shareUserId", Integer.toString(userid1));

    ShareIndexLogicAdapter.getInstance().update(info1);
    try  { 
      System.out.println("5");
      Thread.sleep(200); 
      System.out.println("6");
    } catch(Exception e)  {
     e.printStackTrace();
    } 
       System.out.println("7");
       long id2 = (long)(Math.random()*RANGE);
       int userid2 = (int)(Math.random()*RANGE);
       info2.index.put("shareId", Long.toString(id2));
       info2.index.put("shareUserId", Integer.toString(userid2));

       ShareIndexLogicAdapter.getInstance().update(info2); 
       System.out.println("8");
    }
  }
    
}
