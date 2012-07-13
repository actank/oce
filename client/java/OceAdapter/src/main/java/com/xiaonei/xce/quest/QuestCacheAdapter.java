package com.xiaonei.xce.quest;

import java.util.Date;

import com.xiaonei.xce.log.Oce;

import xce.clusterstate.ClientInterface;
import xce.quest.QuestCachePrx;
import xce.quest.QuestCachePrxHelper;
import xce.quest.Tip;
import xce.quest.TipFlow;
import xce.quest.TipOrder;

public class QuestCacheAdapter {
	private QuestCacheAdapter() {

	}

	private static final String endpoints = "ControllerQuestCache";
	private static int interval = 120;

	private static QuestCacheAdapter _instance = new QuestCacheAdapter();

	private static ClientInterface _csClient = new ClientInterface(endpoints,
			interval);

	public static QuestCacheAdapter getInstance() {
		return _instance;
	}

	public boolean newFlow(int userid) {
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManager(userid);
			prx.newFlow(userid);
		} catch (Ice.TimeoutException e) { 
			Oce.getLogger().error(this.getClass().getName() + ".newFlow prx = " + prx + " userId = " + userid + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".newFlow prx = " + prx + " userId = " + userid, e);
		}
		return true;
	}

	public Tip showTip(int userid) {
		Tip tip = new Tip();
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManager(userid);
			tip = prx.show(userid);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".showTip prx = " + prx + " userId = " + userid + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".showTip prx = " + prx + " userId = " + userid, e);
		}
		return tip;
	}

	public void click(int tipid, int userid) {
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManagerOneway(userid);
			prx.click(tipid, userid);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".click prx = " + prx + " userId = " +  userid + "->" + tipid + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".click prx = " + prx + " userId = " +  userid + "->" + tipid, e);
		}
	}

	public void renewTips() {
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManagerOneway(0);
			prx.renewTips();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".renewTips prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".renewTips prx = " + prx, e);
		}
	}

	public void renewFlow() {
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManagerOneway(0);
			prx.renewFlow();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".renewFlow prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".renewFlow prx = " + prx, e);
		}
	}

	public void responseUrl(int userid, String url) {
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManagerOneway(userid);
			prx.responseUrl(userid, url);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".responseUrl prx = " + prx + " userId = "+  userid + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".responseUrl prx = " + prx + " userId = " +  userid, e);
		}
	}

	public void printTipIndex(){
		QuestCachePrx prx = null;
		try {
			prx = getQuestCacheManager(0);
			System.out.println(prx.ice_toString());			
			Tip[] tipSeq = getQuestCacheManager(0).listTips();			
			for(int i=0;i<tipSeq.length;++i){
				System.out.println("id->"+tipSeq[i].id+"  title"+tipSeq[i].title+"  content"+tipSeq[i].content+"  url"+tipSeq[i].url);
			}			
			System.out.println(tipSeq);
		} catch (Exception e) {			
			e.printStackTrace();
		}
	}
	
	public void printFlowIndex(){
		try {
			TipFlow[] flows = getQuestCacheManager(0).listFlows();
			for(int i=0;i<flows.length;++i){
				TipOrder[] flow = flows[i].seq;
				for(int j=0;j<flow.length;++j){
					System.out.println(flow[j].tipId+"	"+flow[j].showCount);
				}
				//System.out.println();
			}
		} catch (Exception e) {
			System.err.println(this.getClass().getName() + " printFlowIndex " + e);
			//e.printStackTrace();
		}
	}
	
	public void renew(int hostId) {
		try {
			getQuestCacheManagerOneway(hostId).renewFlow();
			getQuestCacheManagerOneway(hostId).renewTips();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	// 获取一个适应userid的服务的prx(twoway)
	protected QuestCachePrx getQuestCacheManager(int userId) {

		Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
		
		if (prx0 != null) {
			QuestCachePrx prx = QuestCachePrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());			
			return prx;
		} else {
			return null;
		}
	}

	// 获取一个适应userid的服务的prx(oneway)
	protected QuestCachePrx getQuestCacheManagerOneway(int userId) {

		Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
		if (prx0 != null) {
			QuestCachePrx prx = QuestCachePrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		QuestCacheAdapter adapter = QuestCacheAdapter.getInstance();
		
		adapter.printTipIndex();	
		adapter.printFlowIndex();		
		
		int userid = 222677622;
		userid = 222525090;
		
		// adapter.newFlow(userid);		
		
		// String url = "http://www.renren.com/1234";
		// adapter.responseUrl(userid, url);
		
		// Tip tip = adapter.showTip(userid);		
		
		// System.out.println(tip.id + "	" + tip.title + "	" + tip.content + "	" + tip.url);
		System.exit(0);
	}

}
