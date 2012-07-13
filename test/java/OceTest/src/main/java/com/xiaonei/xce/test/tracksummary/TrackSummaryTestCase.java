package com.xiaonei.xce.test.tracksummary;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.activetrack.TrackSummaryPrx;
import xce.activetrack.TrackSummaryPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.aiguidecache.AIGuideCacheAdapter;


@RunWith(Parameterized.class)
public class TrackSummaryTestCase {	
	
	private int _userId;
	TrackSummaryAdapter _adapter = TrackSummaryAdapter.getInstance();	
	AIGuideCacheAdapter _adapterCache = AIGuideCacheAdapter.getInstance();
	
	public TrackSummaryTestCase(int userId) {
		this._userId = userId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000070},		//测试第一组
		});
	}
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {  
	}
	@Before
	public void setUpBefore() throws Exception {
	}
	@After
	public void tearDownAfter() throws Exception {  
	}
	@Test
	public void Test()  {
		
		String ip = "10.2.8.45";
		String url1 = "http://apps.kaixin.com/farmgame/index.php";
		String url2 = "http://dog.kaixin.com/";
		AIGuideCacheAdapter.AppType appId1 = AIGuideCacheAdapter.AppType.APP_FARMGAME;			
		AIGuideCacheAdapter.AppType appId2 = AIGuideCacheAdapter.AppType.APP_DOG;
		
		// 清空相应的位
		_adapterCache.set(_userId, appId2, false);
		boolean status = _adapterCache.get(_userId, appId2);
		Assert.assertFalse(status);	
		
		// 进入初始状态"P"
		_adapter.set(_userId, "AIGuide", "P");		
		
		// 进入农场页
		_adapter.access(_userId, ip, url1);		
		try {
			Thread.sleep(1000);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		// 退出农场页
		_adapter.access(_userId, ip, url2);
		try {
			Thread.sleep(3000);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		// 检查置位
		status = _adapterCache.get(_userId, appId2);
		Assert.assertTrue(status);
	}
}


class TrackSummaryAdapter extends ServiceAdapter {
	private static final String endpoints = "@TrackSummary";
	private static TrackSummaryAdapter _instance = new TrackSummaryAdapter();

	private TrackSummaryAdapter() {
		super(endpoints, 10, Channel.newActiveTrackChannel(endpoints));
	}
	
	public static TrackSummaryAdapter getInstance(){
		return _instance;
	}
	
	public Map<String, String> get(int userId){		
		return getTrackSummary(userId).get(userId);
	}
	
	public void set(int userId, String key, String value) {
		getTrackSummary(userId).set(userId, key, value);
	}
	
	public void access(int userId, String ip, String url) {
		getTrackSummary(userId).access(userId, ip, url);		
	}

	private Vector<TrackSummaryPrx> managers = new Vector<TrackSummaryPrx>();
	private Vector<TrackSummaryPrx> managersOneway = new Vector<TrackSummaryPrx>();

	protected TrackSummaryPrx getTrackSummary(int userId) {
		return locateProxy(managers, "TS", userId, Channel.Invoke.Twoway,
				TrackSummaryPrxHelper.class, 300);
	}

	protected TrackSummaryPrx getTrackSummaryOneway(int userId) {
		return locateProxy(managersOneway, "TS", userId, Channel.Invoke.Oneway,
				TrackSummaryPrxHelper.class, 300);
	}
}
