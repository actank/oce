package com.xiaonei.xce.socialgraph.ffucr;

import java.util.List;

public interface FriendClusterRecommendAdapter {

    /**
     * 取好友簇推荐数据
     * 
     * @param userId
     * @return
     */
    List<Recommend> getFFUCR(int userId);

}
