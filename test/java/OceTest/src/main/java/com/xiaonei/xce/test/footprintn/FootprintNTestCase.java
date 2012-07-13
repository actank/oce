package com.xiaonei.xce.test.footprintn;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;
import java.util.List;
import java.util.Set;
import java.util.HashSet;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.footprint.FootprintNewAdapter;
import com.xiaonei.xce.footprint.FootprintAdapterReplicaImpl;
import mop.hi.oce.domain.model.FootprintNewView;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

@RunWith(Parameterized.class)
public class FootprintNTestCase {	
	
	private int hostId, guestId;
	
	FootprintNewAdapter _adapter = new FootprintAdapterReplicaImpl("Footprint", 10, true);
	
	
	public FootprintNTestCase(int hostId, int guestId) {
		this.hostId = hostId;		
		this.guestId = guestId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
				{222525090, 261947419},			// 校内
				{222525090, 261947418},			// 校内
				{701306920, 261947404}			// 开心	
		      	
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
		
		try {
			_adapter.toString();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		// 取初始来访列表、脚印数、访问数
		Set<Integer> guests1 = new HashSet<Integer>();
		int f1 = _adapter.getSize(hostId);
		int visitcount1 = _adapter.getUserVisitCount(hostId); 
		List<FootprintNewView> fpnvs1 = _adapter.getAll(hostId, 0, 6).getFootprintViews();		
		System.out.println("number of footprint of " + hostId + " : " + f1);
		System.out.println("visit count of " + hostId + " : " + visitcount1);
		for (int i = 0; i < fpnvs1.size(); ++i ) {
			guests1.add(fpnvs1.get(i).getGuest());
			// System.out.println(fpnvs1.get(i).getHost() + ":" + fpnvs1.get(i).getGuest());			
		}
		
		// 踩脚印
		FootprintNewView fpnv_new = new FootprintNewView();
		fpnv_new.setHost(hostId);
		fpnv_new.setGuest(guestId);		
		_adapter.visitHomepage(fpnv_new, true, "");
		System.out.println(hostId + " visited " + guestId);
		
		try {
			Thread.sleep(3000);
		} catch (Exception e) {
			e.printStackTrace();
		}		
		
		Set<Integer> guests2 = new HashSet<Integer>();
		int f2 = _adapter.getSize(hostId);
		int visitcount2 = _adapter.getUserVisitCount(hostId); 
		List<FootprintNewView> fpnvs2 = _adapter.getAll(hostId, 0, 6).getFootprintViews();
		for (int i = 0; i < fpnvs2.size(); ++i ) {
			guests2.add(fpnvs2.get(i).getGuest());
			// System.out.println(fpnvs1.get(i).getHost() + ":" + fpnvs1.get(i).getGuest());			
		}
		System.out.println("number of footprint of " + hostId + " : " + f2);
		System.out.println("visit count of " + hostId + " : " + visitcount2);
		
		// 新的列表中应该有这个人
		Assert.assertTrue(guests2.contains(guestId));
		
		// 如果过去的前6个人中有这人则不涨人气，否则则涨
		if ( guests1.contains(guestId)) {
			Assert.assertEquals(visitcount1, visitcount2);
		} else {
			Assert.assertEquals(visitcount1+1, visitcount2);
		}
		System.out.println("------------------------\n"); 
	}	
}