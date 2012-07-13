package com.xiaonei.xce.test.networkcache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.networkcache.NetworkCacheReloaderPrx;
import xce.networkcache.NetworkCacheReloaderPrxHelper;
import xce.networkcache.NetworkCacheUnit;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.networkcache.NetworkCacheAdapter;

@RunWith (Parameterized.class)
public class NetworkCacheTestCase {
	private int host_;
	private int guest_;
	
	public NetworkCacheTestCase(int host, int guest){
		host_=host;
		guest_=guest;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,6814},
			});
	}
	
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		
	}
	
	private void Delete(int host, int guest) {
		try {
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s = c.createStatement();
				try{
					String sql = "delete from network_history where userid="+host+" or userid="+guest;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertTrue(false);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertTrue(false);
			}finally{
				c.close();
			}
		}catch (Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
	}
	
	private void Insert(int userid, int networkid, int status) {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s = c.createStatement();
				try{
					String sql = "insert into network_history (userid,network_id,status, stage, info_id) values("+userid+","+networkid+","+status+",0,0)";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertTrue(false);
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertTrue(false);
			}
		}catch (Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
	}
	
	@Before
	public void setUpBefore() throws Exception {
	}
	
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	private void TestReload(){
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(guest_,0,0);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(guest_,0,-1);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(guest_,0,-1);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,1);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(host_,1,1);
		Insert(guest_,0,0);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(host_,1,1);
		Insert(guest_,0,-1);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(host_,1,1);
		Insert(guest_,0,-1);
		Reload();
		Check(host_,guest_,true);
		
		Delete(host_,guest_);
		Insert(host_,0,0);
		Insert(host_,1,1);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(host_,1,1);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,1);
		Insert(host_,1,1);
		Insert(guest_,0,1);
		Reload();
		Check(host_,guest_,false);
	}
	
	private void TestAdd(){
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(guest_,0,1);
		Reload();
		Add(guest_,0);
		Check(host_,guest_,true);
	}
	
	private void TestRemove(){
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(guest_,0,-1);
		Reload();
		Remove(guest_,0);
		Check(host_,guest_,false);
		
		Delete(host_,guest_);
		Insert(host_,0,-1);
		Insert(host_,1,-1);
		Insert(guest_,0,-1);
		Reload();
		Remove(host_,1);
		Check(host_,guest_,true);
	}
	
	@Test
	public void test() {
		TestReload();
		TestAdd();
		TestRemove();
	}
	
	private void Check(int host, int guest,boolean flag){
		try{
			Assert.assertEquals(NetworkCacheAdapter.GetInstance().IsSameNetwork(6813, 6814), flag);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
	}
	
	private void Remove(int user, int network){
		ObjectPrx op = Channel.newUserChannel("").getCommunicator().stringToProxy("M@NetworkCacheReloader0");
		NetworkCacheReloaderPrx ncrp=NetworkCacheReloaderPrxHelper.uncheckedCast(op);
		NetworkCacheUnit[] datas=new NetworkCacheUnit[]{new NetworkCacheUnit(user,network)};
		try{
			ncrp.RemoveNetwork(datas);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
		try{
			Thread.sleep(1000);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void Add(int user, int network){
		ObjectPrx op = Channel.newUserChannel("").getCommunicator().stringToProxy("M@NetworkCacheReloader0");
		NetworkCacheReloaderPrx ncrp=NetworkCacheReloaderPrxHelper.uncheckedCast(op);
		NetworkCacheUnit[] datas=new NetworkCacheUnit[]{new NetworkCacheUnit(user,network)};
		try{
			ncrp.AddNetwork(datas);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
		try{
			Thread.sleep(1000);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void Reload(){
		ObjectPrx op = Channel.newUserChannel("").getCommunicator().stringToProxy("M@NetworkCacheReloader0");
		NetworkCacheReloaderPrx ncrp=NetworkCacheReloaderPrxHelper.uncheckedCast(op);
		int[] ids=new int[]{host_,guest_};
		try{
			ncrp.Reload(ids);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertTrue(false);
		}
		try{
			Thread.sleep(2000);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
}
