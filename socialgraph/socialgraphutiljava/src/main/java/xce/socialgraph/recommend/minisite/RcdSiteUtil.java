/*
 * 作者：熊君武
 * 邮件：junwu.xiong@renren-inc.com
 * 说明：本文件用于首页推荐之小站推荐
 * 时间：2011-12-22
 */

package xce.socialgraph.recommend.minisite;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import com.renren.sns.minisite.xoa.model.SiteInfo;
import com.renren.xoa.lite.ServiceFactories;
import com.renren.xoa.lite.ServiceFactory;
import com.renren.xoa.lite.ServiceFuture;
import com.renren.xoa.lite.ServiceFutureHelper;

/*
 * 该类提供向用户推荐的小站数据，包括小站名，小站id, 小站头像，小站url,小站关注人数
 */

public class RcdSiteUtil {
	private static RcdSiteUtil instance = null;
	private static final String biz = "RCDSITE";
	
	private RcdSiteUtil() {
		
	}
    
	public static RcdSiteUtil getInstance() {
		if (null == instance) {
			synchronized (RcdSiteUtil.class) {
				if (null == instance) {
					try {
						instance = new RcdSiteUtil();
					} catch(Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
    //获取推荐给每个用户的小站数据，包括小站名，小站id, 小站头像，小站url,小站关注人数
	public List<RcdSiteData> getRcdSite(int userId, int limit) {
		List<RcdSiteData> rcdSiteList = new ArrayList<RcdSiteData>();
		//获取XOA ServiceFactory和Service实例
		try {
			final ServiceFactory fact = ServiceFactories.getFactory();
			final com.renren.sns.minisite.xoa.client.SiteService service = fact
					.getService(com.renren.sns.minisite.xoa.client.SiteService.class);
			int count = 0;
			if (limit <= 0) {
				count = 200;
			} else {
				count = limit;
			}

			long timeoutMillis = 500;
			ServiceFuture<SiteInfo[]> futureInfo = service.getSuggestSites(
					userId, count);
			SiteInfo[] siteInfo = ServiceFutureHelper.execute(futureInfo,
					timeoutMillis);
			Assert.assertTrue(siteInfo.length > 0);
			for (SiteInfo item : siteInfo) {
				int siteId = item.getId();
				RcdSiteData rcdSiteItem = new RcdSiteData();
				// 设置用户名
				rcdSiteItem.setUserId(userId);
				// 获取小站名，小站id, 小站头像，小站url
				rcdSiteItem.setSiteId(siteId);
				rcdSiteItem.setHead(item.getHead());
				rcdSiteItem.setName(item.getName());
				rcdSiteItem.setUrl(item.getUrl());

				// 获取小站关注人数
				ServiceFuture<Integer> futureCount = service
						.getFollowerCount(siteId);
				int siteFolllowerCount = ServiceFutureHelper.execute(
						futureCount, timeoutMillis);
				Assert.assertTrue(siteFolllowerCount > 0);
				rcdSiteItem.setFollowerCount(siteFolllowerCount);

				rcdSiteList.add(rcdSiteItem);
			}
		} catch (Exception e) {
          e.printStackTrace();
		}
		
		return rcdSiteList;
	}
	
	public static void main(String[] args) {
		int userId = 309372060;
		int limit = 2;		
		List<RcdSiteData> result = RcdSiteUtil.getInstance().getRcdSite(userId, limit);
		for (RcdSiteData item : result) {
			System.out.print(item.getUserId() + " : " + item.getSiteId() + " : " + item.getName() + 
					" : " + item.getHead() + " : " + item.getUrl() + " : " + item.getFollowerCount() + "\n");
		}
	}	
}