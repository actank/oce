/// 
/// @file TTManagerAdapter.java
/// @brief java版操纵TTManager的adatper， 提供遍历key(TraversalKey)和根据一批key查询相应序列化的用户信息的接口(Query), 使用方法见被注释掉的main函数
/// @author zhigang.wu@opi-corp.com
/// @date 2010-05-10
/// 
package com.xiaonei.xce.searchcache;
import Ice.*;
import java.lang.Object;
import java.lang.String;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;
import Ice.ObjectPrx;
import xce.searchcache.TTManagerPrx;
import xce.searchcache.TTManagerPrxHelper;
import xce.searchcache.Int2ByteSeqMapHolder;
import xce.searchcache.IntSeqHolder;
import xce.searchcache.IntSeqHelper;
import xce.util.channel.Channel;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.task.Task;
import xce.util.task.TaskManager;
////////用于序列化
///import com.xiaonei.search2.model.info.UserInfo;
///import com.xiaonei.search2.model.info.UserInfoUnserializer;

public class TTManagerAdapter  extends ReplicatedServiceAdapter{
  public final int kCluster = 1;
  private static final String endpoints = "@TTManager";
  private Vector<TTManagerPrx> managers = new Vector<TTManagerPrx>();
  private static TTManagerAdapter _instance = new TTManagerAdapter();
  private TTManagerAdapter()  {
    super(endpoints, 4, Channel.newSearchCacheChannel(endpoints),"XiaoNeiSearchCache/Query");
    managers_ = new Vector(cluster() > 0 ? cluster():1);
  }
  public static TTManagerAdapter getInstance() {
    return _instance;
  }
  public boolean verifyProxy(ObjectPrx proxy) {
    return true;
  }
  public TTManagerPrx GetTTManager() {
    //-1表示不限制超时时间，否则默认为260ms
    return TTManagerPrxHelper.uncheckedCast(locateProxy(managers,"T",0,Channel.Invoke.Twoway,TTManagerPrxHelper.class,-1));
  }
  /// @brief Traversal 顺序遍历内存id列表，然后从TT中取出相应一批用户的信息，每次最多取200个用户的信息
  /// 
  /// @param off_set (in)  本次遍历所开始的位置(即用户id)
  /// @param travel_length (out) 本次遍历返回时共遍历的用户id数(即共扫描的位图位数)
  /// @param id_2_search_cache_data_ptr_map (out) 本次遍历所真正得到的用户信息，为用户id到用户序列化信息的映射
  /// 
  /// @return 成功执行无异常发生返回true，否则返回false
  public boolean Traversal( int off_set, IntHolder travel_length, Int2ByteSeqMapHolder user_id_2_user_info_map)  {
    if (off_set < 0) {
      return false;
    }
    System.out.println("[TEMPLE] Step in TTManagerAdapter::Traversal");
    TTManagerPrx proxy = GetTTManager();
    if (proxy!=null) {
      boolean return_value = proxy.Traversal(off_set, travel_length, user_id_2_user_info_map);
      if (return_value) {
      }
      System.out.println("[TEMPLE] Step out TTManagerAdapter::Traversal");
      return return_value;
    }
    System.out.println("[TEMPLE] Step out TTManagerAdapter::Traversal");
    return false;
  }

  /// @brief Query 查询一批用户的信息
  /// 
  /// @param user_ids 待查询的一批用户id
  /// @param user_id_2_user_info_string_map 用户id到用户信息(序列化的数据) 的 映射
  /// 
  /// @return 成功执行无异常发生返回true，否则返回false
  public boolean Query(int [] user_ids, Int2ByteSeqMapHolder user_id_2_user_info_map) {
    TTManagerPrx proxy = GetTTManager();
    
    if (null != proxy) {
      return proxy.Gets(user_ids, user_id_2_user_info_map);
    } else {
      return false;
    }

  }

  /// @brief TraversalKey 按照id从小到大的顺序遍历, 得到一批在tt中存储的有效用户的id 
  /// 
  /// @param off_set(in) 本次遍历的开始位置(本次最小的用户id)
  /// @param travel_length(int) 指定本次遍历请求的有效用户id数
  /// 
  /// @return 返回一个数组，该数组中按照从小到大的顺序存放有效用户的id,当返回的有效用户id数为零或者小于request_num时，表示遍历结束, 即已经到了最后
  public int [] TraversalKey(int off_set, int request_num) {
    int[] temp_array = new int[0];

    if (off_set < 0 || request_num <= 0) {
      return temp_array;
    }

    TTManagerPrx proxy = GetTTManager();

    if (null != proxy) {
      return proxy.TraversalKey(off_set, request_num);
    } else {
      return temp_array;
    }
  }
  /// @brief GetMemIdsListSize 远程获取TTManager(TT管理器)所维护的内存id列表可装载的元素的最大数目
  /// 
  /// @return  TTManager内存id列表可装载的元素的最大数目
  public int GetMemIdsListSize()  {
    TTManagerPrx proxy = GetTTManager();
    if (proxy!=null) {
      return proxy.GetMemIdsListSize();
    }
    return -1;
  }

  private String name() {
    return "TTManager";
  }
  private String endpoints() {
    return "@TTManager";
  }
  private int cluster() {
    return kCluster;
  }
  private Vector<TTManagerPrx> managers_;

  public static void main(String[] args)  {
   int start_pos = 0;
   System.out.println("start_pos="+start_pos);//  << endl;
   int request_num = 1000;
   TTManagerAdapter test = new TTManagerAdapter();
   int idx = 0;
   while (true) {
     int [] user_ids_array = test.TraversalKey(start_pos, request_num);

     System.out.println("idx="+idx+ ", start_pos="+start_pos+", request_num="+request_num+",user_ids_array.length="+user_ids_array.length);
     ///for (int i=0; i<user_ids_array.length; ++i) {
     ///  System.out.println("user id " + i + " is " + user_ids_array[i]);
     ///}

     ++idx;
     Int2ByteSeqMapHolder id_2_user_info_map=new Int2ByteSeqMapHolder();
     test.Query(user_ids_array, id_2_user_info_map);
     System.out.println("Query result size for user_ids is " + id_2_user_info_map.value.size());



     Iterator iter = id_2_user_info_map.value.entrySet().iterator();

     ///while (iter.hasNext()) {
     ///  java.util.Map.Entry entry = (java.util.Map.Entry)iter.next();
     ///  System.out.print(" key=" + entry.getKey());
     ///  byte[] bytes = (byte [])entry.getValue();
     ///  UserInfo info = UserInfoUnserializer.getInstance().unserialize(bytes, true);
     ///  System.out.println(" Value:" + info.toString());
     ///}
     ///System.out.println();


     
     if (0 == user_ids_array.length|| user_ids_array.length < request_num) break;
     //返回的数组user_ids_array是升序排列好的， 下次的起始id为本次返回的最大id 再加一
     start_pos = user_ids_array[user_ids_array.length-1] + 1;
   } 

   System.exit(0);
  }
}
