package com.xiaonei.xce.blogindexlogic;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.lang.Math;
import Ice.ObjectPrx;

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

public class BlogIndexLogicAdapter extends  ReplicatedClusterAdapter {
  
  public static final int THREAD_SIZE = 8;

  private static final String sil_endpoints = "ControllerBlogIndexLogic";

  private static int _interval = 120;

  private Vector<IndexUpdateLogicPrx> managers = new Vector<IndexUpdateLogicPrx>(); 
  
  private BlogIndexLogicAdapter()  {

    super(sil_endpoints, _interval);

  }

  private static BlogIndexLogicAdapter _instance = new BlogIndexLogicAdapter();

  public static BlogIndexLogicAdapter getInstance()  {
    return _instance;
  }


  protected IndexUpdateLogicPrx getBlogIndexLogicManagerOneway(int index)  {
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
      IndexUpdateLogicPrx tmp = BlogIndexLogicAdapter.getInstance().getBlogIndexLogicManagerOneway(0);
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
    int type0 = 2;
    int category_id0 = 5;
    String title0 = "火龙果的故事";
    String body0 = "小萝卜头";
    int view_count0 = 19;
    int comment_count0 = 34;
    int blogControl0 = 8;
    String time0 = "2010-12-20";
    String upTime0 = "2010-12-19";
    short operation0 = 0;
    
    int type1 = 2;
    int category_id1 = 5;
    String title1 = "火龙果的故事";
    String body1 = "小萝卜头";
    int view_count1 = 1;
    int comment_count1 = 3;
    int blogControl1 = 8;
    String time1 = "2010-12-02";
    String upTime1 = "2010-12-19";
    short operation1 = 2;
    
    int type2 = 2;
    int category_id2 = 5;
    String title2 = "火龙果的故事";
    String body2 = "小萝卜头";
    int view_count2 = 2;
    int comment_count2 = 3;
    int blogControl2 = 8;
    String time2 = "2010-22-02";
    String upTime2 = "2010-22-29";
    short operation2 = 1;
    
    info0.type = BusinessType.Blog;
    info0.index = new HashMap<String, String>();
    info0.index.put("type", Integer.toString(type0));
    info0.index.put("categoryId", Integer.toString(category_id0));
    info0.index.put("title", title0);
    info0.index.put("body", body0);
    info0.index.put("viewCount", Integer.toString(view_count0));
    info0.index.put("commentCount", Integer.toString(comment_count0));
    info0.index.put("commentCount", Integer.toString(comment_count0));
    info0.index.put("blogControl", Integer.toString(blogControl0));
    info0.index.put("time", time0);
    info0.index.put("upTime", upTime0);
    info0.operationType = operation0;
    
    info1.type  = BusinessType.Blog;
    info1.index = new HashMap<String, String>();
    info2.index = new HashMap<String, String>();
    info1.index.put("type", Integer.toString(type1));
    info1.index.put("categoryId", Integer.toString(category_id1));
    info1.index.put("title", title1);
    info1.index.put("body", body1);
    info1.index.put("viewCount", Integer.toString(view_count1));
    info1.index.put("commentCount", Integer.toString(comment_count1));
    info1.index.put("commentCount", Integer.toString(comment_count1));
    info1.index.put("blogControl", Integer.toString(blogControl1));
    info1.index.put("time", time1);
    info1.index.put("upTime", upTime1);
    info1.operationType = operation1;
    
    info2.type = BusinessType.Blog;
    info2.index.put("type", Integer.toString(type2));
    info2.index.put("categoryId", Integer.toString(category_id2));
    info2.index.put("title", title2);
    info2.index.put("body", body2);
    info2.index.put("viewCount", Integer.toString(view_count2));
    info2.index.put("commentCount", Integer.toString(comment_count2));
    info2.index.put("commentCount", Integer.toString(comment_count2));
    info2.index.put("blogControl", Integer.toString(blogControl2));
    info2.index.put("time", time2);
    info2.index.put("upTime", upTime2);
    info2.operationType = operation2;
    for(int i = 0; i < 1000; i++)  {
    System.out.println("info0 owner: "+ info0.operationType + " info1 owner: "+ info1.operationType + " info2 owner: "+ info2.operationType);
    long id0 = (long)(Math.random()*RANGE);
    int userid0 = (int)(Math.random()*RANGE);
    info0.index.put("id", Long.toString(id0));
    info0.index.put("owner", Integer.toString(userid0));

    
    BlogIndexLogicAdapter.getInstance().update(info0);
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
    info1.index.put("id", Long.toString(id1));
    info1.index.put("owner", Integer.toString(userid1));

     
    BlogIndexLogicAdapter.getInstance().update(info1);
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
       info2.index.put("id", Long.toString(id2));
       info2.index.put("owner", Integer.toString(userid2));
       
       BlogIndexLogicAdapter.getInstance().update(info2); 
       System.out.println("8");
    }
  }
    
}
