package com.xiaonei.xce.searchrelationlogic;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.lang.Math;
import Ice.ObjectPrx;

import com.xiaonei.search5.searchrelation.Operation;
import com.xiaonei.search5.searchrelation.SearchRelationLogicPrx;
import com.xiaonei.search5.searchrelation.SearchRelationLogicPrxHelper;


import xce.util.channel.Channel;
import xce.clusterstate.ReplicatedClusterAdapter;

import xce.util.tools.MathUtil;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public class SearchRelationLogicAdapter extends  ReplicatedClusterAdapter {
  
  public static final int THREAD_SIZE = 8;

  private static final String sil_endpoints = "ControllerSearchRelationLogic";

  private static int _interval = 120;

  private Vector<SearchRelationLogicPrx> managers = new Vector<SearchRelationLogicPrx>(); 
  
  private SearchRelationLogicAdapter()  {

    super(sil_endpoints, _interval);

  }

  private static SearchRelationLogicAdapter _instance = new SearchRelationLogicAdapter();

  public static SearchRelationLogicAdapter getInstance()  {
    return _instance;
  }


  protected SearchRelationLogicPrx getSearchRelationLogicManagerOneway(int index)  {
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
      SearchRelationLogicPrx prx = SearchRelationLogicPrxHelper.uncheckedCast(prx1.get(0).ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }
  }



 public void pushFriendShip(int fromUserId, int toUserId, Operation op)  {
    try  {
      SearchRelationLogicPrx tmp = SearchRelationLogicAdapter.getInstance().getSearchRelationLogicManagerOneway(0);
      if(tmp != null)  {
           tmp.pushFriendShip(fromUserId, toUserId, op);
        }  else  {
           return;
        }
    } catch(Exception e)  {
      e.printStackTrace();
    }
  }

  /*protected boolean verifyProxy(ObjectPrx proxy) {
      return SearchRelationLogicPrxHelper.uncheckedCast(proxy).isValid();
  }*/ 
  
  public static void main(String[] args)  {
    
    int fromUserId0 = 25701953, toUserId0 = 58742, fromUserId1 = 34355462, toUserId1 = 34343523, fromUserId2 = 44444443, toUserId2 = 9999999;
    Operation op = Operation.Add; 
    for(int i = 0; i < 1000; i++)  {
      
      SearchRelationLogicAdapter.getInstance().pushFriendShip(fromUserId0, toUserId0, op);
    try  {    
      Thread.sleep(100);
    }  catch(Exception e)  {
     e.printStackTrace();
    }
      SearchRelationLogicAdapter.getInstance().pushFriendShip(fromUserId1, toUserId1, op);
    try  {    
      Thread.sleep(100);
    }  catch(Exception e)  {
     e.printStackTrace();
    }
      SearchRelationLogicAdapter.getInstance().pushFriendShip(fromUserId2, toUserId2, op);
    try  {    
      Thread.sleep(100);
    }  catch(Exception e)  {
     e.printStackTrace();
    }
  }
 } 

}
