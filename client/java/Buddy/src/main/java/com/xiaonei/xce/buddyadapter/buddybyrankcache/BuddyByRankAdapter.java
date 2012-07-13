package com.xiaonei.xce.buddyadapter.buddybyrankcache;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.FriendRankCachePrx;
import xce.socialgraph.FriendRankCachePrxHelper;
import xce.socialgraph.RankData;

import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;
import com.xiaonei.xce.log.Oce;

public class BuddyByRankAdapter<T extends Rank> extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerFriendRankCache";

    private static int _interval = 120;

    private final RankFactory<T> _factory;

    public BuddyByRankAdapter(RankFactory<T> factory) {
        super(ctr_endpoints, _interval);
        _factory = factory;
    }

    protected FriendRankCachePrx getFriendRankManager(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 1000);
        if (prx0 != null) {
            FriendRankCachePrx prx = FriendRankCachePrxHelper.uncheckedCast(prx0.ice_timeout(1000)
                    .ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    protected FriendRankCachePrx getFriendRankCacheOneWay(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 1000);
        if (prx0 != null) {
            FriendRankCachePrx prx = FriendRankCachePrxHelper.uncheckedCast(prx0.ice_timeout(1000)
                    .ice_oneway());
            return prx;
        } else {
            return null;
        }
    }

    /**
     * 根据ID升序获取Rank列表
     * 
     * @param userId
     * @return
     */
    public List<T> getFriendRankSortById(int userId) {
        FriendRankCachePrx prx = null;
        try {
            prx = getFriendRankManager(userId);
            RankData[] ret = prx.GetRank(userId);

            List<T> listRank = new ArrayList<T>();
            for (RankData one : ret) {
                T obj = _factory.create();
                obj.setRank(one.userid, one.weight);
                listRank.add(obj);
            }
            return listRank;
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortById [ " + prx + " ] id="
                            + userId + " Ice.ConnectTimeoutException");
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortById [ " + prx + " ] id="
                            + userId + " Ice.TimeoutException");
        } catch (Exception e) {
            Oce.getLogger().error(this.getClass().getName() + " getFriendRankSortById " + e);
            //e.printStackTrace();
        }
        return null;
    }

    /**
     * 根据Rank值排序获取Rank列表
     * 
     * @param userId
     * @return
     */
    public List<T> getFriendRankSortByScore(int userId) {
        FriendRankCachePrx prx = null;
        try {
            prx = getFriendRankManager(userId);
            RankData[] ret = prx.GetRank(userId);

            List<T> listRank = new ArrayList<T>();
            for (RankData one : ret) {
                T obj = _factory.create();
                obj.setRank(one.userid, one.weight);
                listRank.add(obj);
            }

            Comparator<Rank> scoreComparator = new ComparatorRankByScore();
            Collections.sort(listRank, scoreComparator);

            return listRank;
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortByScore [ " + prx + " ] id="
                            + userId + " Ice.ConnectTimeoutException");
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortByScore [ " + prx + " ] id="
                            + userId + " Ice.TimeoutException");
        } catch (Exception e) {
            Oce.getLogger().error(this.getClass().getName() + " getFriendRankSortByScore " + e);
            //e.printStackTrace();
        }
        return null;
    }

    /**
     * 根据Rank值排序获取id列表
     * 
     * @param userId
     * @return
     */
    public List<Integer> getFriendIdSortByScore(int userId) {
        FriendRankCachePrx prx = null;
        try {
            prx = getFriendRankManager(userId);
            RankData[] ret = prx.GetRank(userId);

            List<T> listRank = new ArrayList<T>();
            for (RankData one : ret) {
                T obj = _factory.create();
                obj.setRank(one.userid, one.weight);
                listRank.add(obj);
            }

            Comparator<Rank> scoreComparator = new ComparatorRankByScore();
            Collections.sort(listRank, scoreComparator);

            List<Integer> listInt = new ArrayList<Integer>();
            for (int i = 0; i < listRank.size(); ++i) {
                listInt.add(listRank.get(i).getUserId());
            }
            return listInt;
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortByScore [ " + prx + " ] id="
                            + userId + " Ice.ConnectTimeoutException");
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRankSortByScore [ " + prx + " ] id="
                            + userId + " Ice.TimeoutException");
        } catch (Exception e) {
            Oce.getLogger().error(this.getClass().getName() + " " + e);
            //e.printStackTrace();
        }
        return null;
    }

    /**
     * 获取对应userId好友的id->weight映射
     * 
     * @param userId
     * @return
     */
    public Map<Integer, Integer> getFriendRevRank(int userId) {
        Map<Integer, Integer> result = new HashMap<Integer, Integer>();
        FriendRankCachePrx prx = null;
        try {
            Map<Integer, List<Integer>> tmpResult = new HashMap<Integer, List<Integer>>();
            List<Integer> userFriend = BuddyByIdCacheAdapter.getInstance().getFriendListAsc(userId,
                    2000);

            for (Integer o : userFriend) {
                int k = o % 10;
                if (!tmpResult.containsKey(k)) {
                    List<Integer> list = new ArrayList<Integer>();
                    list.add(o);
                    tmpResult.put(k, list);
                } else {
                    List<Integer> list = tmpResult.get(k);
                    list.add(o);
                }
            }
            for (Entry<Integer, List<Integer>> k : tmpResult.entrySet()) {
                int key = k.getKey();
                List<Integer> v = k.getValue();

                int size = v.size();
                int[] idList = new int[size];
                for (int i = 0; i < v.size(); ++i) {
                    idList[i] = v.get(i);
                }

                if (!v.isEmpty()) {
                    prx = getFriendRankManager(key);
                    Map<Integer, Integer> thisRank = prx.GetRevRank(userId, idList);
                    for (Entry<Integer, Integer> ee : thisRank.entrySet()) {
                        result.put(ee.getKey(), ee.getValue());
                    }
                }
            }
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRevRank [ " + prx + " ] id=" + userId
                            + " Ice.ConnectTimeoutException");
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getFriendRevRank [ " + prx + " ] id=" + userId
                            + " Ice.TimeoutException");
        } catch (Exception e) {
            Oce.getLogger().error(this.getClass().getName() + " getFriendRevRank " + e);
        }
        return result;
    }

    public static void main(String[] args) {
        int userId = 1331;
        BuddyByRankAdapter<DefaultRank> adapter = new BuddyByRankAdapter<DefaultRank>(
                new DefaultRankFactory());
        List<DefaultRank> listRank = adapter.getFriendRankSortByScore(userId);
        if (listRank.size() >= 0) {
            System.out.println("您关注的好友，按亲密度降序排列：");
            for (Rank r : listRank) {
                System.out.print("(" + r.getUserId() + ":" + r.getWeight() + ")");
            }
        }
        System.out.println();

        Map<Integer, Integer> list = adapter.getFriendRevRank(userId);
        for (Entry<Integer, Integer> u : list.entrySet()) {
            System.out.println(u.getKey() + " : " + u.getValue());
        }
        System.out.println(list.size());

        System.exit(0);
    }
}
