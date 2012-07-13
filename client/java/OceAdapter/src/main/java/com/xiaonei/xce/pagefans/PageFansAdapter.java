package com.xiaonei.xce.pagefans;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import xce.buddy.BuddySixDegreesManagerPrx;
import xce.buddy.BuddySixDegreesManagerPrxHelper;
import xce.buddy.PageEntry;
import xce.buddy.PageFansManagerPrx;
import xce.buddy.PageFansManagerPrxHelper;
import xce.buddy.PageFansReloaderPrx;
import xce.buddy.PageFansReloaderPrxHelper;
import xce.buddyapplycache.BuddyApplyCacheManagerNPrx;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.ObjectPrx;
import Ice.TimeoutException;
import xce.util.tools.IPAddress;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public final class PageFansAdapter {

	private static PageFansCacheAdapter PageCache = new PageFansCacheAdapter();
	private static PageFansReloaderAdapter PageReloader = new PageFansReloaderAdapter();

	public PageFansAdapter() {
	}

	public PageFans getFan(int pageId, int userId) {

		PageFans results = new PageFans();
		PageFansManagerPrx prx = null;
		try {
			prx = PageCache.getPageFansManager(pageId);
			PageEntry res = prx.getFan(pageId, userId);
			if (res == null)
				return results;
			results.Parse(res);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFan [ " + prx + " ] id="
							+ userId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFan [ " + prx + " ] id="
							+ userId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFan " + userId + " ", e);
		}

		return results;
	}

	// PageEntry getFanById(int id, int pageId, int userId);
	// MyUtil::IntSeq getFansIds(int pageid, int begin , int limit);

	public List<PageFans> getFans(int pageId, int begin, int limit) {
		List<PageFans> results = new ArrayList<PageFans>();

		PageFansManagerPrx prx = null;
		try {
			prx = PageCache.getPageFansManager(pageId);

			PageEntry[] res = prx.getFans(pageId, begin, limit);
			for (int i = 0; i < res.length; i++) {
				PageFans obj = new PageFans();
				obj.Parse(res[i]);
				results.add(obj);
			}
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFans [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFans [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFans " + pageId + " ", e);
		}

		return results;

	}

	public List<PageFans> getFansByUserIds(int pageId, int[] userIds) {
		List<PageFans> results = new ArrayList<PageFans>();

		PageFansManagerPrx prx = null;
		try {
			prx = PageCache.getPageFansManager(pageId);
			PageEntry[] res = prx.getFansByUserIds(pageId, userIds);
			for (int i = 0; i < res.length; i++) {
				PageFans obj = new PageFans();
				obj.Parse(res[i]);
				results.add(obj);
			}
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByUserIds [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByUserIds [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByUserIds " + pageId + " ", e);
		}

		return results;
	}

	public List<PageFans> getFansByState(int pageId, int state, int begin, int limit) {
		List<PageFans> results = new ArrayList<PageFans>();
		PageFansManagerPrx prx = null;
		try {
			prx = PageCache.getPageFansManager(pageId);
			PageEntry[] res = prx.getFansByState( pageId,  state,  begin,  limit);
			for (int i = 0; i < res.length; i++) {
				PageFans obj = new PageFans();
				obj.Parse(res[i]);
				results.add(obj);
			}
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansByState " + pageId + " ", e);
		}

		return results;
	}

	public int getFansCountByState(int pageId, int state) {
		
		PageFansManagerPrx prx = null;
		int results = -1;
		try {
			prx = PageCache.getPageFansManager(pageId);
		   results = prx.getFansCountByState( pageId,  state);			
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansCountByState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansCountByState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFansCountByState " + pageId + " ", e);
		}

		return results;
	}

	public boolean isFan(int pageId, int userId) {
		PageFansManagerPrx prx = null;
		boolean results = false;
		try {
			prx = PageCache.getPageFansManager(pageId);
		    results = prx.isFan( pageId,  userId);			
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFan " + pageId + " ", e);
		}

		return results;
	}

	public int findBlock(int pageId, int userId) {
		PageFansManagerPrx prx = null;
		int results = -1;
		try {
			prx = PageCache.getPageFansManager(pageId);
		    results = prx.findBlock( pageId,  userId);			
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".findBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".findBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".findBlock " + pageId + " ", e);
		}

		return results;
	}

	public List<BlacklistItem>  getBlockList(int pageId) {
		List<BlacklistItem> results = new ArrayList<BlacklistItem>();
		PageFansManagerPrx prx = null;
		try {
			prx = PageCache.getPageFansManager(pageId);
			PageEntry[] res = prx.getBlockList( pageId);
			for (int i = 0; i < res.length; i++) {
				BlacklistItem obj = new BlacklistItem();
				obj.Parse(res[i]);
				results.add(obj);
			}
			return results;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBlockList [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBlockList [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBlockList " + pageId + " ", e);
		}

		return results;
	}

	public int addFan(PageFans pe) {
		PageFansReloaderPrx prx = null;	
		int pageId= pe.getPageId();
		try {
			prx =  PageReloader.getReloaderPrx(pageId);
			PageEntry obj = new PageEntry();
			
			if(pe.getAddTime()!= null )
			{
				obj.addTime = (int)(pe.getAddTime().getTime()/1000L);
			}else
			{
				obj.addTime = (int)((new Date()).getTime()/1000L);
			}
			obj.id = pe.getId();
			obj.operatorId = pe.getOperatorId();
			obj.pageId = pe.getPageId();
			obj.state = pe.getState();
			obj.userId = pe.getUserId();
							
		    prx.addFan( obj);			
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addFan " + pageId + " ", e);
		}

		return 0;
	}

	public int removeFan(int pageId, int userId) {
		PageFansReloaderPrx prx = null;			
		try {
			prx = PageReloader.getReloaderPrx(pageId);
			prx.removeFan(pageId,userId);		
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeFan [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeFan " + pageId + " ", e);
		}

		return 0;
	}

	public int updateFanWithState(PageFans pe) {
		PageFansReloaderPrx prx = null;	
		int pageId= pe.getPageId();
		try {
			prx =  PageReloader.getReloaderPrx(pageId);
			PageEntry obj = new PageEntry();
			if(pe.getAddTime()!= null )
			{
				obj.addTime = (int)(pe.getAddTime().getTime()/1000L);
			}
			obj.id = pe.getId();
			obj.operatorId = pe.getOperatorId();
			obj.pageId = pe.getPageId();
			obj.state = pe.getState();
			obj.userId = pe.getUserId();
							
		    prx.updateFanWithState( obj);			
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFanWithState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFanWithState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFanWithState " + pageId + " ", e);
		}

		return 0;
	}

	public int updateFansWithState(int pageId, int oriState, int tarState) {
		PageFansReloaderPrx prx = null;			
		try {
			if(oriState ==tarState) 
				return 1;
			prx = PageReloader.getReloaderPrx(pageId);
			prx.updateFansWithState( pageId,  oriState,  tarState);		
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFansWithState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFansWithState [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateFansWithState " + pageId + " ", e);
		}

		return 0;
	}

	public int addBlock(int pageId, int userId, int operatorId, Date timestamp) {		
		
		PageFansReloaderPrx prx = null;			
		try {
			prx = PageReloader.getReloaderPrx(pageId);
			int addTime = 0;
			if( timestamp!= null )
			{
				 addTime= (int)(timestamp.getTime()/1000L);	
			}else
			{
				 addTime= (int)((new Date()).getTime()/1000L);	
			}
			prx.addBlock(pageId, userId,operatorId, addTime);
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock " + pageId + " ", e);
		}

		return 0;
	}

	public int removeBlock(int pageId, int userId) {
		PageFansReloaderPrx prx = null;			
		try {
			prx = PageReloader.getReloaderPrx(pageId);
			prx.removeBlock( pageId, userId);
			return 0;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock [ " + pageId
							+ " ] pageId=" + pageId + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addBlock " + pageId + " ", e);
		}

		return 0;
	}

	

	public void Out(PageFans pe) {

		System.out.println("pageid " + pe.getPageId());
		System.out.println("id " + pe.getId());
		System.out.println("state " + pe.getState());
		System.out.println("userid " + pe.getUserId());
		System.out.println("addtime  " + pe.getAddTime());
		System.out.println("operator  " + pe.getOperatorId());
		System.out.println("\n");
	}
	
	public void Out(BlacklistItem pe) {

		System.out.println("pageid " + pe.getPageId());
		System.out.println("id " + pe.getId());
	
		System.out.println("userid " + pe.getUserId());
		System.out.println("timestamp  " + pe.getTimestamp());

		System.out.println("\n");
	}

	public static void main(String[] args) {
		try {
			PageFansAdapter adapter = new PageFansAdapter();

			int[] userIds = new int[10];

//			userIds[0] = 1;
//			userIds[1] = 6;
//
//			List<PageFans> pes = adapter.getFansByUserIds(198000002, userIds);

			List<PageFans> pes = adapter.getFansByState(198000002,5, 0, 100);

			for (int i = 0; i < pes.size(); i++) {
				PageFans pe = pes.get(i);
				adapter.Out(pe);
			}

//			System.out.println(adapter.isFan(198000002, 264631588));
			PageFans pe = new PageFans();

			pe.setOperatorId( 103002);
			pe.setPageId( 198000002);
			pe.setState(5);
			pe.setUserId(10002);
//		//	pe.setAddTime((new java.util.Date()));
//			final java.text.SimpleDateFormat df = new java.text.SimpleDateFormat(
//					"ms");
//     
			
//			pe.setAddTime((new java.util.Date()));
			

			//adapter.addFan(pe);
			// adapter.removeFan(198000002, 1022);

			 adapter.updateFanWithState(pe);
//
//			 adapter.addBlock(198000002, 10, 10, new java.util.Date());
//
//			List<BlacklistItem> pes2  = adapter.getBlockList(198000002);
//			for (int i = 0; i < pes2.size(); i++) {
//				BlacklistItem pe2 = pes2.get(i);
//				adapter.Out(pe2);
//			}

			// System.out.println(adapter.findBlock(198000002, 4));
//			 adapter.removeBlock(198000002, 10);
			//			
//			 int count = adapter.getFansCountByState(198000002, 4);
//			 System.out.println(" cout = " + count);
//			 adapter.updateFansWithState(198000002, 4,3);
////						
//		 count = adapter.getFansCountByState(198000002, 3);
//			 System.out.println(" cout = " + count);
//
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.exit(0);
	}
}
