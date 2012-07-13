package com.xiaonei.xce.test.userstatecache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.userstatecache.UserStateCacheData;
import xce.userstatecache.UserStateCacheLoaderPrx;
import xce.userstatecache.UserStateCacheLoaderPrxHelper;
import xce.userstatecache.UserStateCacheManagerPrx;
import xce.userstatecache.UserStateCacheManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userstatecache.UserStateCache;
import com.xiaonei.xce.userstatecache.UserStateCacheAdapter;

@RunWith (Parameterized.class)
public class UserStateCacheTestCase {
	private int _id;
	private int _stateO;
	private String _nameO;
	private String _nicknameO;
	private int _stateN;
	private String _nameN;
	private String _nicknameN;
	public UserStateCacheTestCase(int id, int stateO, String nameO, String nicknameO, int stateN, String nameN,String nicknameN){
		_id=id;
		_stateO=stateO;
		_nameO=nameO;
		_nicknameO=nicknameO;
		_stateN=stateN;
		_nameN=nameN;
		_nicknameN=nicknameN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,1,"nameO","nicknameO",2,"nameN","nicknameN"},
			});
	}
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_passport(id,account,safe_status,status, domain) values ("+_id+", 'account'"+_id+", 1,0, 'domain'"+_id+")";
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
					String sql="replace into user_state(id,state) values ("+_id+","+_stateO+")";
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
					String sql="replace into user_names(id,name,nickname) values ("+_id+",'"+_nameO+"','"+_nicknameO+"')";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_state set state="+_stateN+" where id="+_id;
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
					String sql="update user_names set name='"+_nameN+"', nickname='"+_nicknameN+"' where id="+_id;
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
	
	private void reload(){
		ObjectPrx op=Channel.newCoreChannel("core").getCommunicator().stringToProxy("L@UserStateCacheLoader");
		UserStateCacheLoaderPrx usclp=UserStateCacheLoaderPrxHelper.uncheckedCast(op);
		//int[] ids=new int[]{_id};
		usclp.load(_id);
	}
	
	private void checkN(){
		List<UserStateCacheManagerPrx> managers=getProxies(_id);
		for(UserStateCacheManagerPrx uscmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer,UserStateCacheData> maps=uscmp.getUserStateCacheList(ids);
			Assert.assertTrue(maps.containsKey(_id));
			Assert.assertEquals(ids.length, maps.size());
			checkN(maps.get(_id));
		}
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<java.lang.Integer, UserStateCache> map=UserStateCacheAdapter.getInstance().getUserStateCacheList(ids);
		Assert.assertTrue(map.containsKey(_id));
		Assert.assertEquals(ids.size(), map.size());
		checkN(map.get(_id));
	}
	
	private void checkO(){
		List<UserStateCacheManagerPrx> managers=getProxies(_id);
		for(UserStateCacheManagerPrx uscmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer,UserStateCacheData> maps=uscmp.getUserStateCacheList(ids);
			Assert.assertTrue(maps.containsKey(_id));
			Assert.assertEquals(ids.length, maps.size());
			checkO(maps.get(_id));
		}
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<java.lang.Integer, UserStateCache> map=UserStateCacheAdapter.getInstance().getUserStateCacheList(ids);
		Assert.assertTrue(map.containsKey(_id));
		Assert.assertEquals(ids.size(), map.size());
		checkO(map.get(_id));
	}
	
	private void checkN(UserStateCache data){
		Assert.assertEquals(_id,data.getId());
		System.out.println(_nameN+" "+data.getName());;
		Assert.assertEquals(_nameN, data.getName());
		Assert.assertEquals(_nicknameN, data.getNickname());
		Assert.assertEquals(_stateN, data.getState());
	}
	
	private void checkO(UserStateCache data){
		Assert.assertEquals(_id,data.getId());
		Assert.assertEquals(_nameO, data.getName());
		Assert.assertEquals(_nicknameO, data.getNickname());
		Assert.assertEquals(_stateO, data.getState());
	}
	
	private void checkN(UserStateCacheData data){
		Assert.assertEquals(_id, data.id);
		Assert.assertEquals(_nameN, data.name);
		Assert.assertEquals(_stateN, data.state);
		Assert.assertEquals(_nicknameN, data.nickname);
	}
	
	private void checkO(UserStateCacheData data){
		Assert.assertEquals(_id, data.id);
		Assert.assertEquals(_nameO, data.name);
		Assert.assertEquals(_stateO, data.state);
		Assert.assertEquals(_nicknameO, data.nickname);
	}
	
	private List<UserStateCacheManagerPrx> getProxies(int userid){
		List<UserStateCacheManagerPrx> managers=new ArrayList<UserStateCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserStateCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserStateCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("new");
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserStateCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("old");
			}
		}
		return managers;
	}
}
