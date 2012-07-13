package com.xiaonei.xce.stateindexlogic;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.lang.Math;
import Ice.ObjectPrx;
import java.util.Random;

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

public class StateIndexLogicAdapter extends  ReplicatedClusterAdapter {
  
  public static final int THREAD_SIZE = 8;

  private static final String sil_endpoints = "ControllerStateIndexLogic";

  private static int _interval = 120;

  private Vector<IndexUpdateLogicPrx> managers = new Vector<IndexUpdateLogicPrx>(); 
  
  private StateIndexLogicAdapter()  {

    super(sil_endpoints, _interval);

  }

  private static StateIndexLogicAdapter _instance = new StateIndexLogicAdapter();

  public static StateIndexLogicAdapter getInstance()  {
    return _instance;
  }


  protected IndexUpdateLogicPrx getStateIndexLogicManagerOneway(int index)  {
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
      IndexUpdateLogicPrx tmp = StateIndexLogicAdapter.getInstance().getStateIndexLogicManagerOneway(0);
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
    String content0 = "艳二雄和蛋蛋的故事";
    String dtime0 = "2010-12-20";
    int comment_count0 = 34;
    int repeat_count0 = 4;
    int type0 = 2;
    String link0 = "http://renren.com";
    int source0 = 1;
    int repeat_count_total0 = 89;
    int markbit0 = 2;
    short operation0 = 0;

    String content1 = "小萝卜头";
    String dtime1 = "2111-12-21";
    int comment_count1 = 34;
    int repeat_count1 = 4;
    int type1 = 2;
    String link1 = "http://renren.com";
    int source1 = 1;
    int repeat_count_total1 = 89;
    int markbit1 = 2;
    short operation1 = 1;

    String content2 = "人人网";
    String dtime2 = "2212-12-22";
    int comment_count2 = 34;
    int repeat_count2 = 4;
    int type2 = 2;
    String link2 = "http://renren.com";
    int source2 = 1;
    int repeat_count_total2 = 89;
    int markbit2 = 2;
    short operation2 = 2;

    info0.type = BusinessType.State;
    info0.index = new HashMap<String, String>();
    info0.index.put("content", content0);
    info0.index.put("dtime", dtime0);
    info0.index.put("commentCount", Integer.toString(comment_count0));
    info0.index.put("repeatCount", Integer.toString(repeat_count0));
    info0.index.put("type", Integer.toString(type0));
    info0.index.put("link", link0);
    info0.index.put("source", Integer.toString(source0));
    info0.index.put("repeatCountTotal", Integer.toString(repeat_count_total0));
    info0.index.put("markbit", Integer.toString(markbit0));
    info0.operationType = operation0;
    
    info1.type = BusinessType.State;
    info1.index = new HashMap<String, String>();
    info1.index.put("content", content1);
    info1.index.put("dtime", dtime1);
    info1.index.put("commentCount", Integer.toString(comment_count1));
    info1.index.put("repeatCount", Integer.toString(repeat_count1));
    info1.index.put("type", Integer.toString(type1));
    info1.index.put("link", link1);
    info1.index.put("source", Integer.toString(source1));
    info1.index.put("repeatCountTotal", Integer.toString(repeat_count_total1));
    info1.index.put("markbit", Integer.toString(markbit1));
    info1.operationType = operation1;
    
    info2.type = BusinessType.State;
    info2.index = new HashMap<String, String>();
    info2.index.put("content", content2);
    info2.index.put("dtime", dtime2);
    info2.index.put("commentCount", Integer.toString(comment_count2));
    info2.index.put("repeatCount", Integer.toString(repeat_count2));
    info2.index.put("type", Integer.toString(type2));
    info2.index.put("link", link2);
    info2.index.put("source", Integer.toString(source2));
    info2.index.put("repeatCountTotal", Integer.toString(repeat_count_total2));
    info2.index.put("markbit", Integer.toString(markbit2));
    info2.operationType = operation2;
    


    for(int i = 0; i < 1000; i++)  {
    System.out.println("info0 owner: "+ info0.operationType + " info1 owner: "+ info1.operationType + " info2 owner: "+ info2.operationType);

    long id0 = (long)(Math.random()*RANGE);
    int userid0 = (int)(Math.random()*RANGE);
    info0.index.put("id", Long.toString(id0));
    info0.index.put("userid", Integer.toString(userid0));


    StateIndexLogicAdapter.getInstance().update(info0);
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
    info1.index.put("userid", Integer.toString(userid1));
     
    StateIndexLogicAdapter.getInstance().update(info1);
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
    info2.index.put("userid", Integer.toString(userid2));



       StateIndexLogicAdapter.getInstance().update(info2); 
       System.out.println("8");
    }
  }
    
}
