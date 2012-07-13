package com.xiaonei.xce.test.userapicache;

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
import xce.userapicache.UserApiCacheData;
import xce.userapicache.UserApiCacheData1;
import xce.userapicache.UserApiCacheLoaderPrx;
import xce.userapicache.UserApiCacheLoaderPrxHelper;
import xce.userapicache.UserApiCacheManagerPrx;
import xce.userapicache.UserApiCacheManagerPrxHelper;
import xce.util.channel.Channel;

import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userapicache.UserApiCache;
import com.xiaonei.xce.userapicache.UserApiCacheAdapter;

@RunWith (Parameterized.class)
public class UserApiCacheTestCase {
	private int _id;
	
	private String _nameO;
	private int _stageO;
	private int _statusO;
	private String _genderO;
	private int _authO;
	private int _status_configO;
	private int _basic_configO;
	private String _profile_privacyO;
	private String _home_provinceO;
	private String _home_cityO;
	private int _birth_yearO;
	private int _birth_monthO;
	private int _birth_dayO;
	private int _univO;
	private String _main_urlO;
	private String _head_urlO;
	private String _tiny_urlO;
	private int _stateO;
	private int _levelO;
	
	private String _nameN;
	private int _stageN;
	private int _statusN;
	private String _genderN;
	private int _authN;
	private int _status_configN;
	private int _basic_configN;
	private String _profile_privacyN;
	private String _home_provinceN;
	private String _home_cityN;
	private int _birth_yearN;
	private int _birth_monthN;
	private int _birth_dayN;
	private int _univN;
	private String _main_urlN;
	private String _head_urlN;
	private String _tiny_urlN;
	private int _stateN;
	private int _levelN;
	
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,"nameO",1,1,"genderO",1,1,1,"profile_privacyO","home_provinceO","home_cityO",1,1,1,1,1,"main_urlO","head_urlO","tiny_urlO",0,0,
					"nameN",2,2,"genderN",2,2,2,"profile_privacyN","home_provinceN","home_cityN",2,2,2,2,2,"main_urlN","head_urlN","tiny_urlN",1,2},
			});
	}
	
	public UserApiCacheTestCase(int id, String nameO, int stageO, int statusO, String genderO, int authO, 
			int status_configO, int basic_configO, String profile_privacyO, String home_provinceO,String home_cityO,
			int birth_yearO, int birth_monthO, int birth_dayO, int selectO, int univO, String main_urlO,
			String head_urlO, String tiny_urlO,int stateO, int levelO, String nameN, int stageN, int statusN, String genderN, int authN, 
			int status_configN, int basic_configN, String profile_privacyN, String home_provinceN,String home_cityN,
			int birth_yearN, int birth_monthN, int birth_dayN, int selectN, int univN, String main_urlN,
			String head_urlN, String tiny_urlN, int stateN, int levelN){
		_id=id;
		_nameO=nameO;
		_stageO=stageO;
		_statusO=statusO;
		_genderO=genderO;
		_authO=authO;
		_status_configO=status_configO;
		_basic_configO=basic_configO;
		_profile_privacyO=profile_privacyO;
		_home_provinceO=home_provinceO;
		_home_cityO=home_cityO;
		_birth_yearO=birth_yearO;
		_birth_monthO=birth_monthO;
		_birth_dayO=birth_dayO;
		_univO=univO;
		_main_urlO=main_urlO;
		_head_urlO=head_urlO;
		_tiny_urlO=tiny_urlO;
		_stateO=stateO;
		_levelO=levelO;
		_nameN=nameN;
		_stageN=stageN;
		_statusN=statusN;
		_genderN=genderN;
		_authN=authN;
		_status_configN=status_configN;
		_basic_configN=basic_configN;
		_profile_privacyN=profile_privacyN;
		_home_provinceN=home_provinceN;
		_home_cityN=home_cityN;
		_birth_yearN=birth_yearN;
		_birth_monthN=birth_monthN;
		_birth_dayN=birth_dayN;
		_univN=univN;
		_main_urlN=main_urlN;
		_head_urlN=head_urlN;
		_tiny_urlN=tiny_urlN;
		_stateN=stateN;
		_levelN=levelN;
	}

	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_passport(id,account,safe_status,status, domain) values ("+_id+", 'account"+_id+"', 1,"+_statusO+", 'domain"+_id+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_stage(id,stage,univ) values ("
						+_id+", "+_stageO+","+_univO+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_born(id,gender,birth_year,birth_month,birth_day,home_province, home_city) values ("
						+_id+",  '"+_genderO+"' ,"+_birth_yearO+","+_birth_monthO+","+_birth_dayO+", '"+_home_provinceO+"','"+_home_cityO+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_config(id,basic_config,status_config,profile_privacy,email_config) values ("+_id+","+_basic_configO+","+_status_configO+",'"+_profile_privacyO+"',0)";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_url(id,mainurl,headurl,tinyurl) values ("+_id+",'"+_main_urlO+"','"+_head_urlO+"','"+_tiny_urlO+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_state(id,state,level) values ("+_id+","+_stateO+","+_levelO+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
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
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_right(id,auth) values ("+_id+","+_authO+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private void reload(){
		ObjectPrx op=Channel.newCoreChannel("core").getCommunicator().stringToProxy("M@UserApiCacheLoader");
		UserApiCacheLoaderPrx uaclp=UserApiCacheLoaderPrxHelper.uncheckedCast(op);
		int[] ids=new int[]{_id};
		uaclp.load(ids);
		try{
			Thread.sleep(1000);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
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
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_stage set stage="+_stageN+",univ="+_univN+" where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_born set gender='"+_genderN+"', birth_year="+_birth_yearN+", birth_month="+_birth_monthN+"" +
							",birth_day="+_birth_dayN+",home_province='"+_home_provinceN+"', home_city='"+_home_cityN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_config set basic_config="+_basic_configN+",status_config="+_status_configN+",profile_privacy='"+_profile_privacyN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_url set mainurl='"+_main_urlN+"',headurl='"+_head_urlN+"',tinyurl='"+_tiny_urlN+"' where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_state(id,state,level) values ("+_id+","+_stateN+","+_levelN+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
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
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="update user_right set auth="+_authN+" where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
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
	
	private List<UserApiCacheManagerPrx> getProxies(int userid){
		List<UserApiCacheManagerPrx> managers=new ArrayList<UserApiCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserApiCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("new");
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("old");
			}
		}
		return managers;
	}
	
	private void checkO(UserApiCacheData1 data){
		Assert.assertEquals(data.auth, _authO);
		Assert.assertEquals(data.basicconfig, _basic_configO);
		Assert.assertEquals(data.birthday, _birth_dayO);
		Assert.assertEquals(data.birthmonth, _birth_monthO);
		Assert.assertEquals(data.univ, _univO);
		Assert.assertEquals(data.birthyear, _birth_yearO);
		Assert.assertEquals(data.headurl, _head_urlO);
		Assert.assertEquals(data.homecity, _home_cityO);
		Assert.assertEquals(data.homeprovince, _home_provinceO);
		Assert.assertEquals(data.mainurl, _main_urlO);
		Assert.assertEquals(data.name, _nameO);
		Assert.assertEquals(data.profileprivacy, _profile_privacyO);
		Assert.assertEquals(data.gender, _genderO);
		Assert.assertEquals(data.stage, _stageO);
		Assert.assertEquals(data.status, _statusO);
		Assert.assertEquals(data.statusconfig, _status_configO);
		Assert.assertEquals(data.tinyurl, _tiny_urlO);
		Assert.assertEquals(data.state,_stateO);
		Assert.assertEquals(data.level, _levelO);
	}
	
	private void checkO(UserApiCache data){
		Assert.assertEquals(data.getAuth(), _authO);
		Assert.assertEquals(data.getBasicconfig(), _basic_configO);
		Assert.assertEquals(data.getBirthday(), _birth_dayO);
		Assert.assertEquals(data.getBirthmonth(), _birth_monthO);
		Assert.assertEquals(data.getUniv(), _univO);
		Assert.assertEquals(data.getBirthyear(), _birth_yearO);
		Assert.assertEquals(data.getHeadurl(), _head_urlO);
		Assert.assertEquals(data.getHomecity(), _home_cityO);
		Assert.assertEquals(data.getHomeprovince(), _home_provinceO);
		Assert.assertEquals(data.getMainurl(), _main_urlO);
		Assert.assertEquals(data.getName(), _nameO);
		Assert.assertEquals(data.getProfileprivacy(), _profile_privacyO);
		Assert.assertEquals(data.getSex(), _genderO);
		Assert.assertEquals(data.getStage(), _stageO);
		Assert.assertEquals(data.getStatus(), _statusO);
		Assert.assertEquals(data.getStatusconfig(), _status_configO);
		Assert.assertEquals(data.getTinyurl(), _tiny_urlO);
		Assert.assertEquals(data.getState(), _stateO);
	}
	
	private void checkO(){
		List<UserApiCacheManagerPrx> managers=getProxies(_id);
		for(UserApiCacheManagerPrx uacmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer, UserApiCacheData1> maps=uacmp.getUserApiCache1List(ids);
			Assert.assertEquals(ids.length, maps.size());
			Assert.assertTrue(maps.containsKey(_id));
			checkO(maps.get(_id));
		}
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer,UserApiCache> map=UserApiCacheAdapter.getInstance().getUserApiCacheList(ids);
		Assert.assertEquals(ids.size(),map.size());
		Assert.assertTrue(map.containsKey(_id));
		checkO(map.get(_id));
	}
	
	private void checkN(UserApiCacheData1 data){
		Assert.assertEquals(data.auth, _authN);
		Assert.assertEquals(data.basicconfig, _basic_configN);
		Assert.assertEquals(data.birthday, _birth_dayN);
		Assert.assertEquals(data.birthmonth, _birth_monthN);
		Assert.assertEquals(data.univ, _univN);
		Assert.assertEquals(data.birthyear, _birth_yearN);
		Assert.assertEquals(data.headurl, _head_urlN);
		Assert.assertEquals(data.homecity, _home_cityN);
		Assert.assertEquals(data.homeprovince, _home_provinceN);
		Assert.assertEquals(data.mainurl, _main_urlN);
		Assert.assertEquals(data.name, _nameN);
		Assert.assertEquals(data.profileprivacy, _profile_privacyN);
		Assert.assertEquals(data.gender, _genderN);
		Assert.assertEquals(data.stage, _stageN);
		Assert.assertEquals(data.status, _statusN);
		Assert.assertEquals(data.statusconfig, _status_configN);
		Assert.assertEquals(data.tinyurl, _tiny_urlN);
		Assert.assertEquals(data.state,_stateN);
		Assert.assertEquals(data.level, _levelN);
	}
	
	private void checkN(UserApiCache data){
		Assert.assertEquals(data.getAuth(), _authN);
		Assert.assertEquals(data.getBasicconfig(), _basic_configN);
		Assert.assertEquals(data.getBirthday(), _birth_dayN);
		Assert.assertEquals(data.getBirthmonth(), _birth_monthN);
		Assert.assertEquals(data.getUniv(), _univN);
		Assert.assertEquals(data.getBirthyear(), _birth_yearN);
		Assert.assertEquals(data.getHeadurl(), _head_urlN);
		Assert.assertEquals(data.getHomecity(), _home_cityN);
		Assert.assertEquals(data.getHomeprovince(), _home_provinceN);
		Assert.assertEquals(data.getMainurl(), _main_urlN);
		Assert.assertEquals(data.getName(), _nameN);
		Assert.assertEquals(data.getProfileprivacy(), _profile_privacyN);
		Assert.assertEquals(data.getSex(), _genderN);
		Assert.assertEquals(data.getStage(), _stageN);
		Assert.assertEquals(data.getStatus(), _statusN);
		Assert.assertEquals(data.getStatusconfig(), _status_configN);
		Assert.assertEquals(data.getTinyurl(), _tiny_urlN);
		Assert.assertEquals(data.getState(), _stateN);
	}
	
	private void checkN(){
		List<UserApiCacheManagerPrx> managers=getProxies(_id);
		for(UserApiCacheManagerPrx uacmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer, UserApiCacheData1> maps=uacmp.getUserApiCache1List(ids);
			Assert.assertEquals(ids.length, maps.size());
			Assert.assertTrue(maps.containsKey(_id));
			checkN(maps.get(_id));
		}
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer,UserApiCache> map=UserApiCacheAdapter.getInstance().getUserApiCacheList(ids);
		Assert.assertEquals(ids.size(),map.size());
		Assert.assertTrue(map.containsKey(_id));
		checkN(map.get(_id));
	}
}
