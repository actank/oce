package com.xiaonei.xce.test.usercache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.usercache.UserCacheLoaderPrx;
import xce.usercache.UserCacheLoaderPrxHelper;
import xce.usercache.UserCacheManagerPrx;
import xce.usercache.UserCacheManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheAdapter;
import com.xiaonei.xce.usercache.UserCacheFactory;

@RunWith (Parameterized.class)
public class UserCacheTestCase {
	private int _id;	
	private int _statusO;
	private int _starO;
	private int _levelO;
	private int _stateO;
	private int _stageO;
	private int _univO;
	private String _tinyurlO;
	private String _nameO;	
	private String _genderO;
	
	private int _statusN;
	private int _starN;
	private int _levelN;
	private int _stateN;
	private int _stageN;
	private int _univN;
	private String _tinyurlN;
	private String _nameN;	
	private String _genderN;
	
	public UserCacheTestCase(int id, int statusO, int starO, int levelO, int stateO, int stageO, int univO,  String tinyurlO, String nameO, String genderO,
			int statusN, int starN, int levelN, int stateN, int stageN, int univN, String tinyurlN, String nameN, String genderN){
		_id=id;
		_statusO=statusO;
		_starO=starO;
		_levelO=levelO;
		_stateO=stateO;
		_stageO=stageO;
		_univO=univO;
		_tinyurlO=tinyurlO;
		_nameO=nameO;
		_genderO=genderO;

		_statusN=statusN;
		_starN=starN;
		_levelN=levelN;
		_stateN=stateN;
		_stageN=stageN;
		_univN=univN;
		_tinyurlN=tinyurlN;
		_nameN=nameN;
		_genderN=genderN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,1,1,1,1,1,1,"tinyurl1","name1","男生",2,0,0,2,2,2,"tinyurl2","name3","女生"},
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
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_passport(id,account,safe_status,status, domain) values ("+_id+", 'account1', 1,"+_statusO+", 'domain')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_state(id,state,star,level) values ("+_id+","+_stateO+" , "+_starO+","+_levelO+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_url(id,tinyurl) values ("+_id+",'"+_tinyurlO+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_names(id,name) values ("+_id+",'"+_nameO+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_born(id,gender) values ("+_id+", '"+_genderO+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_stage(id, univ, stage) values ("+_id+","+_univO+" , "+_stageO+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		
	}
	
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	@Test
	public void test() {
		reload();
		checkO();
		update();
		reload();
		checkN();
	}
	
	private void update(){
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_passport set status="+_statusN+" where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_state set state="+_stateN+", star="+_starN+", level="+_levelN+" where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_url set tinyurl='"+_tinyurlN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_names set name='"+_nameN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_born set gender='"+_genderN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_stage set univ="+_univN+", stage="+_stageN+" where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void checkN(){
		List<UserCacheManagerPrx> managers=getProxies(_id);
		for(UserCacheManagerPrx ucmp:managers){
			int[] ids=new int[]{_id};
			Map<String,String>[] maps=ucmp.getUserCacheList(ids);
			Assert.assertEquals(ids.length, maps.length);
			for(Map<String,String> map:maps){
				checkN(map);
			}
			Map<String,String> m=ucmp.getUserCache(_id);
			checkN(m);
		}
		UserCacheAdapter<MyUserCache> adapter=new UserCacheAdapter<MyUserCache>(new MyUserCacheFactory());
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer,MyUserCache> map=adapter.getUserCacheMap(ids);
		Assert.assertEquals(ids.size(), map.size());
		Assert.assertTrue(map.containsKey(_id));
		checkN(map.get(_id));
	}
	
	private void checkO(){
		List<UserCacheManagerPrx> managers=getProxies(_id);
		for(UserCacheManagerPrx ucmp:managers){
			int[] ids=new int[]{_id};
			Map<String,String>[] maps=ucmp.getUserCacheList(ids);
			Assert.assertEquals(ids.length, maps.length);
			for(Map<String,String> map:maps){
				checkO(map);
			}
			Map<String,String> m=ucmp.getUserCache(_id);
			checkO(m);
		}
		UserCacheAdapter<MyUserCache> adapter=new UserCacheAdapter<MyUserCache>(new MyUserCacheFactory());
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer,MyUserCache> map=adapter.getUserCacheMap(ids);
		Assert.assertEquals(ids.size(), map.size());
		Assert.assertTrue(map.containsKey(_id));
		checkO(map.get(_id));

	}
	
	private void reload(){
		ObjectPrx op=Channel.newCoreChannel("core").getCommunicator().stringToProxy("M@UserCacheLoader");
		UserCacheLoaderPrx uclp=UserCacheLoaderPrxHelper.uncheckedCast(op);
		int[] ids=new int[]{_id};
		uclp.load(ids);
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	private void checkN(MyUserCache cache){
		Assert.assertEquals(cache.getId(), _id);
		Assert.assertEquals(cache.getStatus(), _statusN);
		Assert.assertEquals(cache.getState(), _stateN);
		Assert.assertEquals(cache.getStar(), _starN);
		Assert.assertEquals(cache.getLevel(), _levelN);
		Assert.assertEquals(cache.getTinyUrl(), _tinyurlN);
		Assert.assertEquals(cache.getUniv(), _univN);
		Assert.assertEquals(cache.getName(), _nameN);
		Assert.assertEquals(cache.getGender(), _genderN);
	}
	
	private void checkN(Map<String,String> map){
		Assert.assertEquals(Integer.parseInt(map.get("id")),_id);
		Assert.assertEquals(Integer.parseInt(map.get("status")),_statusN);
		Assert.assertEquals(Integer.parseInt(map.get("state")),_stateN);
		Assert.assertEquals(Integer.parseInt(map.get("star")),_starN);
		Assert.assertEquals(Integer.parseInt(map.get("level")),_levelN);
		Assert.assertEquals(map.get("tinyurl"),_tinyurlN);
		Assert.assertEquals(map.get("name"),_nameN);
		Assert.assertEquals(Integer.parseInt(map.get("univ")),_univN);
		Assert.assertEquals(Integer.parseInt(map.get("stage")),_stageN);
		Assert.assertEquals(map.get("gender"), _genderN);
	}
	
	private void checkO(MyUserCache cache){
		Assert.assertEquals(cache.getId(), _id);
		Assert.assertEquals(cache.getStatus(), _statusO);
		Assert.assertEquals(cache.getState(), _stateO);
		Assert.assertEquals(cache.getStar(), _starO);
		Assert.assertEquals(cache.getLevel(), _levelO);
		Assert.assertEquals(cache.getTinyUrl(), _tinyurlO);
		Assert.assertEquals(cache.getUniv(), _univO);
		Assert.assertEquals(cache.getName(), _nameO);
		Assert.assertEquals(cache.getGender(), _genderO);
	}
	
	private void checkO(Map<String,String> map){
		Assert.assertEquals(Integer.parseInt(map.get("id")),_id);
		System.out.println(Integer.parseInt(map.get("status"))+" "+_statusO);
		Assert.assertEquals(Integer.parseInt(map.get("status")),_statusO);
		Assert.assertEquals(Integer.parseInt(map.get("state")),_stateO);
		Assert.assertEquals(Integer.parseInt(map.get("star")),_starO);
		Assert.assertEquals(Integer.parseInt(map.get("level")),_levelO);
		Assert.assertEquals(map.get("tinyurl"),_tinyurlO);
		
		Assert.assertEquals(Integer.parseInt(map.get("univ")),_univO);
		Assert.assertEquals(Integer.parseInt(map.get("stage")),_stageO);
		System.out.println(map.get("name")+" "+_nameO);
		Assert.assertEquals(map.get("name"),_nameO);
		System.out.println("fuck: "+map.containsKey("gender")+" "+map.get("gender"));
		Assert.assertEquals(map.get("gender"), _genderO);
	}
	
	private List<UserCacheManagerPrx> getProxies(int userid){
		List<UserCacheManagerPrx> managers=new ArrayList<UserCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("new");
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("old");
			}
		}
		return managers;
	}
	
	private static class MyUserCache extends UserCache{
		
	}
	
	private static class MyUserCacheFactory implements UserCacheFactory<MyUserCache>{

		public MyUserCache createUserCache() {
			return new MyUserCache();
		}
		
	}
}
