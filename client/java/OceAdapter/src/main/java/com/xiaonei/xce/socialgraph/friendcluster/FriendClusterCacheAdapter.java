package com.xiaonei.xce.socialgraph.friendcluster;

import java.util.List;

public interface FriendClusterCacheAdapter {

    /**
     * 取一帮用户的簇数据,数据顺序是:先按照簇出现次数,再按簇分数, ForGuide最后要过滤掉userIdList
     * 
     * @param userIdList
     * @param limit
     * @return
     */
    public List<Integer> getFriendClusterPeopleForGuide(List<Integer> userIdList, int limit);

    /**
     * 取一个用户所属的好友簇名字列表
     * 
     * @param uid
     * @return
     */
    public List<String> getClusterNames(int uid);

    /**
     * 取一个簇的全部用户
     * 
     * @param clusterName
     * @return
     */
    public List<Integer> getClusterUsers(String clusterName);

}
