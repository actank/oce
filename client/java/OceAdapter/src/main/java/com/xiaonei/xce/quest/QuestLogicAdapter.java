package com.xiaonei.xce.quest;

import com.xiaonei.xce.log.Oce;

import xce.clusterstate.ClientInterface;
import xce.quest.QuestLogicPrx;
import xce.quest.QuestLogicPrxHelper;
import xce.quest.TipFlow;
import xce.quest.TipOrder;

public class QuestLogicAdapter {
	private QuestLogicAdapter() {

	}

	private static final String endpoints = "ControllerQuestLogic";
	private static int interval = 120;

	private static QuestLogicAdapter _instance = new QuestLogicAdapter();

	private static ClientInterface _csClient = new ClientInterface(endpoints,
			interval);

	public static QuestLogicAdapter getInstance() {
		return _instance;
	}

	public void addTip(String title, String content, String url, int showcount) {
		QuestLogicPrx prx = null;
		try {
			prx = getQuestLogicManagerOneway(0);
			prx.addTip(title, content, url, showcount);
			
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName(), e);
		}
	}

	public void removeTip(int tipid) {
		QuestLogicPrx prx = null;
		try {
			prx = getQuestLogicManagerOneway(0);
			prx.removeTip(tipid);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeTip prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeTip", e);
		}
	}

	public void modifyTip(int tipid, String title, String content, String url) {
		QuestLogicPrx prx = null;
		try {
			prx = getQuestLogicManagerOneway(0);
			prx.modifyTip(tipid, title, content, url);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".modifyTip prx " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".modifyTip", e);
		}
	}

	public void setTipFlow(TipFlow flow) {
		QuestLogicPrx prx = null;
		try {
			prx = getQuestLogicManagerOneway(0);
			prx.setTipFlow(flow);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setTipFlow prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".setTipFlow", e);
		}
	}

	public void renew() {
		QuestLogicPrx prx = null;
		try {
			prx = getQuestLogicManagerOneway(0);
			prx.renew();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".renew prx = " + prx + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".renew", e);
		}
	}

	// 获取一个适应userid的服务的prx(twoway)
	protected QuestLogicPrx getQuestLogicManager(int userId) {

		Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
		if (prx0 != null) {
			QuestLogicPrx prx = QuestLogicPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	// 获取一个适应userid的服务的prx(oneway)
	protected QuestLogicPrx getQuestLogicManagerOneway(int userId) {

		Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
		if (prx0 != null) {
			QuestLogicPrx prx = QuestLogicPrxHelper.uncheckedCast(prx0
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
		
		QuestLogicAdapter adapter = QuestLogicAdapter.getInstance();
		// adapter.addTip("Tip5", "Tip5 第5个Tip", "http://share.renren.com", 5);
		// adapter.addTip("黑田官兵卫", "丰臣家的军师2", "http://blog.renren.com", 5);		
		// adapter.modifyTip(5, "", "", "http://gift.renren.com");
		
		TipFlow flow = new TipFlow();
		flow.seq = new TipOrder[5];
		flow.seq[0] = new TipOrder(2, 4);
		flow.seq[1] = new TipOrder(4, 3);
		flow.seq[2] = new TipOrder(1, 3);
		flow.seq[3] = new TipOrder(5, 5);
		flow.seq[4] = new TipOrder(3, 2);
		adapter.setTipFlow(flow);
		
		adapter.renew();		
		System.exit(0);

	}

}
