package com.xiaonei.xce.lbsindexlogic;

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

public class LBSIndexLogicAdapter extends  ReplicatedClusterAdapter {
  
  public static final int THREAD_SIZE = 8;

  private static final String sil_endpoints = "ControllerLBSIndexLogic";

  private static int _interval = 120;

  private Vector<IndexUpdateLogicPrx> managers = new Vector<IndexUpdateLogicPrx>(); 
  
  private LBSIndexLogicAdapter()  {

    super(sil_endpoints, _interval);

  }

  private static LBSIndexLogicAdapter _instance = new LBSIndexLogicAdapter();

  public static LBSIndexLogicAdapter getInstance()  {
    return _instance;
  }


  protected IndexUpdateLogicPrx getBlogIndexLogicManagerOneway(int index)  {
//    Ice.ObjectPrx prx0 = getValidProxy(index, 300);
    @SuppressWarnings("unchecked")
    List<Ice.ObjectPrx> prx1 = getAllProxy(index);
    System.out.println("proxy size: " + prx1.size());
    int i = 0;
    for(Ice.ObjectPrx tmp : prx1)  {
      System.out.println("No " + i +" proxy: " + tmp);
      i++;
    }
    if (prx1 != null && prx1.size() > 0) {
      IndexUpdateLogicPrx prx = IndexUpdateLogicPrxHelper.uncheckedCast(prx1.get(0).ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }
  }



 public void update(IndexUpdateData info)  {
    try  {
      IndexUpdateLogicPrx tmp = LBSIndexLogicAdapter.getInstance().getBlogIndexLogicManagerOneway(0);
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
    String id_common = "lbs_";
    
    String type0 = "2";
    String lon0 = "32.08";
    String lat0 = "45.08";
    String name0 = "艳二雄和蛋蛋的故事";
    String city0 = "北京";
    String addrs0 = "静安庄";
    String grid20 = "343|4546";
    String grid30 = "567|67";
    String grid40 = "345|343";
    
    String type1 = "2";
    String lon1 = "34.38";
    String lat1 = "42.48";
    String name1 = "小萝卜头";
    String city1 = "厦门";
    String addrs1 = "左家庄西街";
    String grid21 = "243|4346";
    String grid31 = "367|37";
    String grid41 = "245|43";
    
    String type2 = "2";
    String lon2 = "24.38";
    String lat2 = "27.48";
    String name2 = "人人网";
    String city2 = "上海";
    String addrs2 = "天安门";
    String grid22 = "343|4546";
    String grid32 = "27|67";
    String grid42 = "345|324";
    
 
    
    
    info0.type = BusinessType.Letter;
    info0.index = new HashMap<String, String>();
     
   
    info0.index.put("SourceType", type0);
    info0.index.put("Lon", lon0);
    info0.index.put("Lat", lat0);
    info0.index.put("Name", name0);
    info0.index.put("City", city0);
    info0.index.put("Addrs", addrs0);
    info0.index.put("GRID2", grid20);
    info0.index.put("GRID3", grid30);
    info0.index.put("GRID4", grid40);
    info0.operationType = 0;
    
    info1.type = BusinessType.Letter;
    info1.index = new HashMap<String, String>();
    info1.index.put("SourceType", type1);
    info1.index.put("Lon", lon1);
    info1.index.put("Lat", lat1);
    info1.index.put("Name", name1);
    info1.index.put("City", city1);
    info1.index.put("Addrs", addrs1);
    info1.index.put("GRID2", grid21);
    info1.index.put("GRID3", grid31);
    info1.index.put("GRID4", grid41);
    info1.operationType = 2;
    
    info2.type = BusinessType.Letter;
    info2.index = new HashMap<String, String>();
    info2.index.put("SourceType", type2);
    info2.index.put("Lon", lon2);
    info2.index.put("Lat", lat2);
    info2.index.put("Name", name2);
    info2.index.put("City", city2);
    info2.index.put("Addrs", addrs2);
    info2.index.put("GRID2", grid22);
    info2.index.put("GRID3", grid32);
    info2.index.put("GRID4", grid42);
    info2.operationType = 2;
    for(int i = 0; i < 1000; i++)  {
    System.out.println("info0 owner: "+ info0.operationType + " info1 owner: "+ info1.operationType + " info2 owner: "+ info2.operationType);
     
    int id0 = (int)(Math.random()*RANGE); 
    String lbsId0 = id_common + Integer.toString(id0);
    info0.index.put("SourceID", lbsId0);  
    LBSIndexLogicAdapter.getInstance().update(info0);
    System.out.println("1");
    try  {    
      System.out.println("2");
      Thread.sleep(500);
      System.out.println("3");
    }  catch(Exception e)  {
     e.printStackTrace();
    } 
      System.out.println("4");
    int id1 = (int)(Math.random()*RANGE);
    String lbsId1 = id_common + Integer.toString(id1);
    info1.index.put("SourceID", lbsId1);
    LBSIndexLogicAdapter.getInstance().update(info1);
    try  { 
      System.out.println("5");
      Thread.sleep(200); 
      System.out.println("6");
    } catch(Exception e)  {
     e.printStackTrace();
    } 
       System.out.println("7");
       int id2 = (int)(Math.random()*RANGE);
       String lbsId2 = id_common + Integer.toString(id2);
       info2.index.put("SourceID", lbsId2);
       LBSIndexLogicAdapter.getInstance().update(info2); 
       System.out.println("8");
    }
  }
    
}
