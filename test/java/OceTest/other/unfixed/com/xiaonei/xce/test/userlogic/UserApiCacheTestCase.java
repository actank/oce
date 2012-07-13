package com.xiaonei.xce.test.userlogic;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import junit.framework.Assert;

import mop.hi.oce.adapter.impl.UserConfigInfoAdapterImpl;
import mop.hi.oce.domain.user.UserConfigInfo;

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
import xce.userapicache.UserApiCacheData;
import xce.userapicache.UserApiCacheManagerPrx;
import xce.userapicache.UserApiCacheManagerPrxHelper;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userapicache.UserApiCache;
import com.xiaonei.xce.userapicache.UserApiCacheAdapter;
import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userbasic.UserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicInfo;

import com.xiaonei.xce.username.UserNameAdapter;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userpassport.UserPassportAdapter;
import com.xiaonei.xce.userpassport.UserPassportFactory;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userright.UserRightAdapter;
import com.xiaonei.xce.userright.UserRightFactory;
import com.xiaonei.xce.userright.UserRightInfo;
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

@RunWith (Parameterized.class)
public class UserApiCacheTestCase {
	private int _id;
	private String _name;
	private int _stage;
	private int _star;
	private int _status;
	private String _sex;
	private int _auth;
	private int _statusconfig;
	private int _basicconfig;
	private String _profileprivacy;
	private String _homeprovince;
	private String _homecity;
	private int _birthyear;
	private int _birthmonth;
	private int _birthday;
	private int _univ;
	private String _mainurl;
	private String _headurl;
	private String _tinyurl;
	
	public UserApiCacheTestCase (int id, String name, int stage, int star, int status, String sex,
			int auth, int statusconfig, int basicconfig, String profileprivacy, String homeprovince, String homecity,
			int birthyear, int birthmonth, int birthday, int univ, String mainurl, String headurl,
			String tinyurl){
		_id = id;
		_name = name;
		_stage = stage;
		_star = star;
		_status = status;
		_sex = sex;
		_auth = auth;
		_statusconfig = statusconfig;
		_basicconfig = basicconfig;
		_profileprivacy = profileprivacy;
		_homeprovince = homeprovince;
		_homecity = homecity;
		_birthyear = birthyear;
		_birthmonth = birthmonth;
		_birthday = birthday;
		_univ = univ;
		_mainurl = mainurl;
		_headurl = headurl;
		_tinyurl = tinyurl;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"name222",1,1,0,"m",1,1,1,"profileprivacyl200","homeprovince1","homecity1",80,2,28,15,"manurl1","headurl1","tinyurl1"},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_passport(id,account,domain,status) values ("+_id+",'account"+_id+"','domain"+_id+"', 0)";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_basic(id,status) values ("+_id+",0)";
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
					String sql="replace into user_state(id) values ("+_id+")";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_url(id) values ("+_id+")";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_names(id,name) values ("+_id+",'name"+_id+"')";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_right(id) values ("+_id+")";
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
		UserPassportAdapter<MyUserPassportInfo> upa=new UserPassportAdapter<MyUserPassportInfo>(new MyUserPassportFactory());
		UserConfigInfoAdapterImpl uca = new UserConfigInfoAdapterImpl();
		UserUrlAdapter<MyUserUrlInfo> uua=new UserUrlAdapter<MyUserUrlInfo>(new MyUserUrlFactory());
		UserNameAdapter<MyUserNameInfo> una=new UserNameAdapter<MyUserNameInfo>(new MyUserNameFactory());
		UserBasicAdapter<MyUserBasicInfo> uba=new UserBasicAdapter<MyUserBasicInfo>(new MyUserBasicFactory());
		UserRightAdapter<MyUserRightInfo> ura=new UserRightAdapter<MyUserRightInfo>(new MyUserRightFactory());
		
		MyUserUrlInfo muuiws1=new MyUserUrlInfo();
		muuiws1.setMainUrl(_mainurl);
		muuiws1.setHeadUrl(_headurl);
		muuiws1.setTinyUrl(_tinyurl);
		uua.setUserUrl(_id, muuiws1);	
		
		MyUserBasicInfo mubis1=new MyUserBasicInfo();
		mubis1.setStage(_stage);
		mubis1.setUniv(_univ);
		mubis1.setHomeProvince(_homeprovince);
		mubis1.setHomeCity(_homecity);
		mubis1.setBirthYear(_birthyear);
		mubis1.setBirthMonth(_birthmonth);
		mubis1.setBirthDay(_birthday);
		mubis1.setUniv(_univ);
		mubis1.setSelected(_star);
		mubis1.setGender(_sex);
		uba.setUserBasic(_id, mubis1);
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserPassportInfo mupiws1=new MyUserPassportInfo();
		mupiws1.setStatus(_status);
		upa.setUserPassport(_id, mupiws1);
		
		
		
		MyUserNameInfo muniws1=new MyUserNameInfo();
		muniws1.setName(_name);
		una.setUserName(_id, muniws1);
		
		MyUserRightInfo muris1=new MyUserRightInfo();
		muris1.setAuth(_auth);
		ura.setUserRight(_id, muris1);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserConfigInfo muconf1=new MyUserConfigInfo();
		muconf1.setStatusConfig(_statusconfig);
		muconf1.setBasicConfig(_basicconfig);
		muconf1.setProfilePrivacy(_profileprivacy);
		uca.setUserConfig(_id, muconf1);
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
		checkO();
		
	}
	
	private List<UserApiCacheManagerPrx> getProxies(int userid){
		List<UserApiCacheManagerPrx> managers=new ArrayList<UserApiCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserApiCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
			}
		}
		return managers;
	}
	
	private void checkO(UserApiCacheData cache){
		Assert.assertEquals(cache.id, _id);
		System.out.println("name: " + cache.name);
		Assert.assertEquals(cache.name, _name);
		Assert.assertEquals(cache.stage, _stage);
		Assert.assertEquals(cache.star, _star);
		Assert.assertEquals(cache.status, _status);	
		Assert.assertEquals(cache.sex, _sex);
		Assert.assertEquals(cache.auth, _auth);
		
		Assert.assertEquals(cache.statusconfig, _statusconfig);
		Assert.assertEquals(cache.basicconfig, _basicconfig);
		System.out.println("profileprivacy: " +cache.profileprivacy);
		Assert.assertEquals(cache.profileprivacy, _profileprivacy);
		
		Assert.assertEquals(cache.homeprovince, _homeprovince);
		Assert.assertEquals(cache.homecity, _homecity);
		Assert.assertEquals(cache.birthyear, _birthyear);
		Assert.assertEquals(cache.birthmonth, _birthmonth);
		Assert.assertEquals(cache.birthday, _birthday);
		Assert.assertEquals(cache.univ, _univ);
			
		Assert.assertEquals(cache.mainurl, _mainurl);
		Assert.assertEquals(cache.headurl, _headurl);
		Assert.assertEquals(cache.tinyurl, _tinyurl);
	}
	
	private void checkO(UserApiCache cache){
		Assert.assertEquals(cache.id, _id);
		Assert.assertEquals(cache.name, _name);
		Assert.assertEquals(cache.stage, _stage);
		Assert.assertEquals(cache.star, _star);
		Assert.assertEquals(cache.status, _status);	
		Assert.assertEquals(cache.sex, _sex);
		Assert.assertEquals(cache.auth, _auth);
		
		Assert.assertEquals(cache.statusconfig, _statusconfig);
		Assert.assertEquals(cache.basicconfig, _basicconfig);
		Assert.assertEquals(cache.profileprivacy, _profileprivacy);
		
		Assert.assertEquals(cache.homeprovince, _homeprovince);
		Assert.assertEquals(cache.homecity, _homecity);
		Assert.assertEquals(cache.birthyear, _birthyear);
		Assert.assertEquals(cache.birthmonth, _birthmonth);
		Assert.assertEquals(cache.birthday, _birthday);
		Assert.assertEquals(cache.univ, _univ);
		Assert.assertEquals(cache.mainurl, _mainurl);
		Assert.assertEquals(cache.headurl, _headurl);
		Assert.assertEquals(cache.tinyurl, _tinyurl);
	}
	
	private void checkO(){
		
		List<UserApiCacheManagerPrx> managers=getProxies(_id);
		for(UserApiCacheManagerPrx ucmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer, UserApiCacheData> map=ucmp.getUserApiCacheList(ids);
			for(Entry<Integer,UserApiCacheData> entry:map.entrySet()){
				checkO(entry.getValue());
			}
		}
		
		UserApiCacheAdapter adapter=UserApiCacheAdapter.getInstance();
		UserApiCache data = adapter.getUserApiCache(_id);
		checkO(data);
		
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer, UserApiCache> map=adapter.getUserApiCacheList(ids);
		for(Entry<Integer,UserApiCache> entry:map.entrySet()){
			checkO(entry.getValue());
		}
	}
	
	private static class MyUserPassportInfo extends UserPassportInfo {
	}
	
	private static class MyUserPassportFactory implements UserPassportFactory<MyUserPassportInfo> {
		public MyUserPassportInfo create() {
			return new MyUserPassportInfo();
		}
	}
	
	private static class MyUserBasicInfo extends UserBasicInfo{
		private static final long serialVersionUID = 1L;
		
	}
	
	private static class MyUserBasicFactory implements UserBasicFactory<MyUserBasicInfo>{
		public MyUserBasicInfo create() {
			return new MyUserBasicInfo();
		}		
	}
	
	private static class MyUserConfigInfo extends UserConfigInfo{
		
	}
	
	private static class MyUserUrlInfo extends UserUrlInfo{
	}
	
	private static class MyUserUrlFactory implements UserUrlFactory<MyUserUrlInfo>{
		public MyUserUrlInfo create() {
			return new MyUserUrlInfo();
		}
	}
	
	private static class MyUserNameInfo extends UserNameInfo{
	}
	
	private static class MyUserNameFactory implements UserNameFactory<MyUserNameInfo>{
		public MyUserNameInfo create() {
			return new MyUserNameInfo();
		}
	}
	
	private static class MyUserRightInfo extends UserRightInfo{
	}
	
	private static class MyUserRightFactory implements UserRightFactory<MyUserRightInfo>{
		public MyUserRightInfo create() {
			return new MyUserRightInfo();
		}
	}
	
}
