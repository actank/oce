package com.xiaonei.xce.pagecommonfans;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.xiaonei.xce.log.Oce;

import xce.buddy.PageCommonFansCacheManagerPrx;
import xce.buddy.PageCommonFansCacheManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

/**
 * PageCommonFans
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class PageCommonFansCacheAdapter extends ReplicatedClusterAdapter {

    private static final String endpoints = "ControllerPageCommonFansCache";

    private static int interval = 120;

    private static PageCommonFansCacheAdapter instance = null;
    
    private static final int TIME_OUT = 2000;

    public static PageCommonFansCacheAdapter getInstance() {
        if (instance == null) {
            synchronized (PageCommonFansCacheAdapter.class) {
                if (instance == null) {
                    try {
                        instance = new PageCommonFansCacheAdapter();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            }
        }
        return instance;
    }

    private PageCommonFansCacheAdapter() {
        super(endpoints, interval);
    }

    private PageCommonFansCacheManagerPrx getPageCommonFansCacheManagerPrxTwoWay(int id) {
        Ice.ObjectPrx prx0 = getValidProxy(id, TIME_OUT);
        if (prx0 != null) {
            PageCommonFansCacheManagerPrx prx = PageCommonFansCacheManagerPrxHelper
                    .uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    private PageCommonFansCacheManagerPrx getPageCommonFansCacheManagerPrxOneWay(int id) {
        Ice.ObjectPrx prx0 = getValidProxy(id, TIME_OUT);
        if (prx0 != null) {
            PageCommonFansCacheManagerPrx prx = PageCommonFansCacheManagerPrxHelper
                    .uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_oneway());
            return prx;
        } else {
            return null;
        }
    }

    /**
     * 验证参数
     * 
     * @param uid
     * @param pid
     * @param begin
     * @param limit
     * @return
     */
    private boolean checkParam(int uid, int pid, int begin, int limit) {
        if (uid < 0 || pid < 0 || begin < 0 || limit < -1) {
            return false;
        }
        return true;
    }

    /**
     * 取userId好友中是pageId粉丝的人 请注意begin,limit的使用
     * 
     * @param userId
     * @param pageId
     * @param begin >=0
     * @param limit <br/>
     *        如果limit == -1,取区间为[begin, end) <br/>
     *        如果 limit >=0 取[begin, min{end,begin+limit})
     * @return
     */
    public List<Integer> getCommonFans(int userId, int pageId, int begin, int limit) {
        if (!checkParam(userId, pageId, begin, limit)) {
            return new ArrayList<Integer>();
        }

        List<Integer> commonFansList = new ArrayList<Integer>();
        PageCommonFansCacheManagerPrx proxy = null;
        try {
            proxy = getPageCommonFansCacheManagerPrxTwoWay(userId);
            //            System.out.println("proxy:" + proxy);
            int[] array = proxy.GetCommon(userId, pageId, begin, limit);
            if (array != null && array.length != 0) {
                for (int i : array)
                    commonFansList.add(i);
            }
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCommonFans " + userId
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCommonFans " + userId + " Ice.TimeoutException "
                            + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return commonFansList;

    }

    /**
     * 返回一个uid在快速缓存中的所以信息<br/>
     * key:pageid value:fans id list
     * 
     * @param userId
     * @return
     */
    public Map<Integer, List<Integer>> getFull(int userId) {
        Map<Integer, List<Integer>> retMap = new HashMap<Integer, List<Integer>>();
        PageCommonFansCacheManagerPrx proxy = null;
        try {
            proxy = getPageCommonFansCacheManagerPrxTwoWay(userId);
            Map<Integer, int[]> map = proxy.GetFull(userId);
            for (Entry<Integer, int[]> entry : map.entrySet()) {
                Integer key = entry.getKey();
                int[] value = entry.getValue();
                if (value != null && value.length != 0) {
                    List<Integer> list = new ArrayList<Integer>();
                    for (int v : value)
                        list.add(v);
                    retMap.put(key, list);
                }
            }
        } catch (Ice.ConnectTimeoutException e) {
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return retMap;
    }

    /**
     * 添加信任的page
     * 
     * @param pageIdList
     */
    public void addPages(List<Integer> pageIdList) {
        if (pageIdList == null || pageIdList.size() == 0) {
            return;
        }
        PageCommonFansCacheManagerPrx proxy = null;
        try {
            proxy = getPageCommonFansCacheManagerPrxTwoWay(0);
            int[] array = new int[pageIdList.size()];
            for (int i = 0; i < pageIdList.size(); ++i) {
                array[i] = pageIdList.get(i);
            }
            proxy.AddPages(array);
        } catch (Ice.ConnectTimeoutException e) {
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}
