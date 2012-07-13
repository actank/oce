package com.xiaonei.xce.socialgraph;

import com.xiaonei.xce.socialgraph.autogroupwoker.AutoGroupCommonAdapter;
import com.xiaonei.xce.socialgraph.autogroupwoker.AutoGroupWorkerAdapter;
import com.xiaonei.xce.socialgraph.ffucr.FriendClusterRecommendAdapter;
import com.xiaonei.xce.socialgraph.ffucr.impl.FriendClusterRecommendAdapterImpl;
import com.xiaonei.xce.socialgraph.friendcluster.FriendClusterCacheAdapter;
import com.xiaonei.xce.socialgraph.friendcluster.impl.FriendClusterCacheAdapterImpl;
import com.xiaonei.xce.socialgraph.friendfinderworker.FriendFinderWorkerAdapter;
import com.xiaonei.xce.socialgraph.page.RcdPageAdapter;
import com.xiaonei.xce.socialgraph.puk.PukRecommendAdapter;
import com.xiaonei.xce.socialgraph.relation.RelationService;
import com.xiaonei.xce.socialgraph.relation.impl.RelationServiceClientImpl;

import com.xiaonei.xce.socialgraph.rcdhotpage.RcdHotPageAdapter;
import com.xiaonei.xce.socialgraph.rcdblog.RcdBlogAdapter;
import com.xiaonei.xce.socialgraph.rcdvideo.RcdVideoAdapter;
import com.xiaonei.xce.socialgraph.userapply.UserApplyStatsAdapter;


public class SocialGraphAdapter {

    /**
     * 好友分组数据实时计算接口
     * 
     * @author zhangnan(zhangnan@opi-corp.com)
     */
    public static AutoGroupCommonAdapter getAutoGroupCommonAdapter() {
    	return AutoGroupCommonAdapter.getInstance();
    }

    /**
     * 好友自动分组，异步计算接口，提供全部好友自动分组结果，包括打tag
     * 
     * @return
     */
    public static AutoGroupWorkerAdapter getAutoGroupWorkerAdapter() {
    	return AutoGroupWorkerAdapter.getInstance();
    }

    /**
     * 二度好友数据接口
     * 
     * @author zhangnan(zhangnan@opi-corp.com)
     */
    public static FriendFinderWorkerAdapter getFriendFinderWorkerAdapter() {
    	return FriendFinderWorkerAdapter.getInstance();
    }

    /**
     * 好友簇数据接口
     * 
     * @author michael(liyong@opi-corp.com)
     * @return
     */
    public static FriendClusterCacheAdapter getFriendClusterCacheAdapter() {
    	return FriendClusterCacheAdapterImpl.getInstance();
    }

    /**
     * 好友簇推荐数据接口
     * 
     * @author michael(liyong@opi-corp.com)
     * @return
     */
    public static FriendClusterRecommendAdapter getFriendClusterRecommendAdapter() {
    	return FriendClusterRecommendAdapterImpl.getInstance();
    }

    /**
     * SocialGraph关系服务接口
     * 
     * @return
     */
    public static RelationService getRelationService() {
    	return RelationServiceClientImpl.getInstance();
    }
    
    /**
     * 首页及推荐中心聚合页page推荐数据处理接口，目前首页推荐仅提供remove功能
     * @return
     */
    public static RcdPageAdapter getRcdPageAdapter() {
    	return RcdPageAdapter.getInstance();
    }
    
    /**
     * 推荐中心页提供“可能认识的人模块数据”
     * @return
     */
    public static PukRecommendAdapter getPukRecommendAdapter() {
    	return PukRecommendAdapter.getInstance();
    }

    /**
    * 推荐中心聚合页hotpage推荐数据处理接口
    */
    public static RcdHotPageAdapter getRcdHotPageAdapter() {
        return RcdHotPageAdapter.getInstance();
    }

    /**
    * 推荐中心聚合页blog推荐数据处理接口
    */
    public static RcdBlogAdapter getRcdBlogAdapter() {
        return RcdBlogAdapter.getInstance();
    }

    /**
    * 推荐中心聚合页video推荐数据处理接口
    */
    public static RcdVideoAdapter getRcdVideoAdapter() {
        return RcdVideoAdapter.getInstance();
    }
    
    /**
     * 用户最近一个月中的加好友行为接口
     * @return
     */
    public static UserApplyStatsAdapter getUserApplyStatsAdapter() {
    	return UserApplyStatsAdapter.getInstance();
    }
}
