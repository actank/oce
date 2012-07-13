package com.xiaonei.xce.searchcache;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import com.xiaonei.xce.log.Oce;
//import oce.searchcache.DistSearchCache;
import com.xiaonei.xce.searchcache.DCSearchCacheAdapter;

public final class DistSearchCacheAdapter {
	private SearchCacheInvoker _invoker;
	
	public DistSearchCacheAdapter() {
		String flag=System.getProperty("SearchCacheInvoker","DC");
		if(flag.equals("DC")) {
			_invoker = new DCSearchCacheAdapter();
		}else {
			_invoker = null;//new IceSearchCacheAdapter();
		}
	}

	public Map<Integer, byte[]> getSearchCacheMap2(List<Integer> userIds) {
		return _invoker.getSearchCacheMap2(userIds);
	}

  /*  description: test for DistSearchCacheAdapter
   *                               */
	public static void main(String[] args) {
	/*	System.setProperty("SearchCacheInvoker", "DC");
		try {
			DistSearchCacheAdapter adapter = new DistSearchCacheAdapter();
			List<Integer> ids=new ArrayList<Integer>();
      ids.add(6790023);
      ids.add(13930017);
      ids.add(390006);
      ids.add(25701953);
      ids.add(58742);
      ids.add(723180044);
      ids.add(201783098);
      Map<Integer, byte[]> map=adapter.getSearchCacheMap2(ids);
      if(map == null)  {
        //System.out.println("map is null!");
      } else  {
        //System.out.println("map size is: " + map.size());
      }
      for(Entry<Integer,byte[]> entry:map.entrySet()){
        DistSearchCache.DistSearchCacheData tmp = null;
        try  {
            tmp = DistSearchCache.DistSearchCacheData.parseFrom(entry.getValue());
        } catch (Exception e )  {
         // System.out.println("unserialize error!");
        }
			  //System.out.println(entry.getKey() + " " + "status: "+ tmp.getStatus());
			  //System.out.println(entry.getKey() + " " + "safe_status: "+ tmp.getSafeStatus());
        //System.out.println(entry.getKey()+" name: "+ tmp.getName());
				//System.out.println(entry.getKey()+" gender: " + tmp.getGender());
        if(tmp.getHighSchoolsList().size()>0)  {
				 
          //System.out.println(entry.getKey()+" highSchoolSize: " + tmp.getHighSchoolsList().size());
          //System.out.println(entry.getKey()+" first highSchoolId: " + tmp.getHighSchoolsList().get(0).getId());
        }
			}
		} catch (Exception e) {
			//System.out.println("error");
      //Oce.getLogger().error(e);
		}*/
		//System.exit(0);
	}
}
