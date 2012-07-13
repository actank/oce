package com.xiaonei.xce.test.userbase;

import java.sql.Connection;
import java.sql.ResultSet;
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

import xce.userbase.UserBornManagerPrx;
import xce.userbase.UserBornManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userborn.UserBornAdapter;
import com.xiaonei.xce.userborn.UserBornFactory;
import com.xiaonei.xce.userborn.UserBornInfo;

@RunWith (Parameterized.class)
public class UserBornTestCase {
	
	private int _id;             
	private String _genderO;
	private int _birth_dayO;
	private int _birth_monthO;
	private int _birth_yearO;
	private String _home_provinceO;
	private String _home_cityO;
	private String _city_codeO;
	private String _astrologyO;
	private String _genderN;
	private int _birth_dayN;
	private int _birth_monthN;
	private int _birth_yearN;
	private String _home_provinceN;
	private String _home_cityN;
	private String _city_codeN;
	private String _astrologyN;
	private int _updateId;

	public UserBornTestCase(int id, String genderO,
			int birth_dayO, int birth_monthO, int birth_yearO,
			String home_provinceO, String home_cityO, String city_codeO, 
			String astrologyO, 
			String genderN,
			int birth_dayN, int birth_monthN, int birth_yearN,
			String home_provinceN, String home_cityN, String city_codeN, 
			String astrologyN, 
			int updateId){
		_id=id;
		_genderO=genderO;
		_birth_dayO=birth_dayO;
		_birth_monthO=birth_monthO;
		_birth_yearO=birth_yearO;
		_home_provinceO=home_provinceO;
		_home_cityO=home_cityO;
		_city_codeO=city_codeO;
		_astrologyO=astrologyO;
		_genderN=genderN;
		_birth_dayN=birth_dayN;
		_birth_monthN=birth_monthN;
		_birth_yearN=birth_yearN;
		_home_provinceN=home_provinceN;
		_home_cityN=home_cityN;
		_city_codeN=city_codeN;
		_astrologyN=astrologyN;
		_updateId=updateId;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"gender1",1,1,1,"home_province1","home_city1","city_code1",
						"astrology1","gender2",2,2,2,
						"home_province2","home_city2","city_code2",
						"astrology2",6814},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_born where id="+_id;
					s.execute(sql);
					sql="delete from user_born where id="+_updateId;
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
		ObjectPrx op=Channel.newUserChannel("user").getCommunicator().stringToProxy("UBOM@UserBase"+_id%100);
		UserBornManagerPrx ubmp=UserBornManagerPrxHelper.uncheckedCast(op);
		ubmp.reloadUserBorn(_id);
		op=Channel.newUserChannel("user").getCommunicator().stringToProxy("UBOM@UserBase"+_updateId%100);
		ubmp=UserBornManagerPrxHelper.uncheckedCast(op);
		ubmp.reloadUserBorn(_updateId);
	}
	
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	void checkUpdateId(){
		UserBornAdapter<MyUserBornInfo> uba=new UserBornAdapter<MyUserBornInfo>(new MyUserBornFactory());
		MyUserBornInfo mubiw1=new MyUserBornInfo();
		mubiw1.setId(_updateId);
		uba.setUserBorn(_id, mubiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_doing");
			try{
				Statement s=c.createStatement();
				try{
					int id=-1;
					String sql="SELECT id FROM user_doing WHERE id="+_updateId;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("id"), id);
					}
					rs.close();
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
		try {
			MyUserBornInfo mubir=uba.getUserBorn(_id);
			Assert.assertEquals(mubir.getId(),_id);
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try {
			uba.getUserBorn(_updateId);
			Assert.assertFalse(true);
		} catch (Exception e) {
			Assert.assertTrue(true);
		}
	}
	
	@Test
	public void test() {
		UserBornAdapter<MyUserBornInfo> uba=new UserBornAdapter<MyUserBornInfo>(new MyUserBornFactory());
		MyUserBornInfo mubiw1=new MyUserBornInfo();
		mubiw1.setId(_id);
		mubiw1.setAstrology(_astrologyO);
		mubiw1.setBirthDay(_birth_dayO);
		mubiw1.setBirthMonth(_birth_monthO);
		mubiw1.setBirthYear(_birth_yearO);
		mubiw1.setCityCode(_city_codeO);
		mubiw1.setGender(_genderO);
		mubiw1.setHomeCity(_home_cityO);
		mubiw1.setHomeProvince(_home_provinceO);
		uba.createUserBorn(_id, mubiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT * FROM user_born WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("gender"), _genderO);
						Assert.assertEquals(rs.getInt("birth_day"), _birth_dayO);
						Assert.assertEquals(rs.getInt("birth_month"), _birth_monthO);
						Assert.assertEquals(rs.getInt("birth_year"), _birth_yearO);
						Assert.assertEquals(rs.getString("home_province"), _home_provinceO);
						Assert.assertEquals(rs.getString("home_city"), _home_cityO);
						Assert.assertEquals(rs.getString("city_code"), _city_codeO);
						Assert.assertEquals(rs.getString("astrology"), _astrologyO);
					}
					rs.close();
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
			MyUserBornInfo mubir1=uba.getUserBorn(_id);
			Assert.assertEquals(mubir1.getAstrology(),_astrologyO);
			Assert.assertEquals(mubir1.getBirthDay(),_birth_dayO);
			Assert.assertEquals(mubir1.getBirthMonth(),_birth_monthO);
			Assert.assertEquals(mubir1.getCityCode(),_city_codeO);
			Assert.assertEquals(mubir1.getGender(),_genderO);
			Assert.assertEquals(mubir1.getHomeCity(),_home_cityO);
			Assert.assertEquals(mubir1.getHomeProvince(),_home_provinceO);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserBornInfo mubiw2=new MyUserBornInfo();
		mubiw2.setAstrology(_astrologyN);
		mubiw2.setBirthDay(_birth_dayN);
		mubiw2.setBirthMonth(_birth_monthN);
		mubiw2.setBirthYear(_birth_yearN);
		mubiw2.setCityCode(_city_codeN);
		mubiw2.setGender(_genderN);
		mubiw2.setHomeCity(_home_cityN);
		mubiw2.setHomeProvince(_home_provinceN);
		uba.setUserBorn(_id, mubiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT * FROM user_born WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("gender"), _genderN);
						Assert.assertEquals(rs.getInt("birth_day"), _birth_dayN);
						Assert.assertEquals(rs.getInt("birth_month"), _birth_monthN);
						Assert.assertEquals(rs.getInt("birth_year"), _birth_yearN);
						Assert.assertEquals(rs.getString("home_province"), _home_provinceN);
						Assert.assertEquals(rs.getString("home_city"), _home_cityN);
						Assert.assertEquals(rs.getString("city_code"), _city_codeN);
						Assert.assertEquals(rs.getString("astrology"), _astrologyN);
					}
					rs.close();
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
			MyUserBornInfo mubir1=uba.getUserBorn(_id);
			Assert.assertEquals(mubir1.getAstrology(),_astrologyN);
			Assert.assertEquals(mubir1.getBirthDay(),_birth_dayN);
			Assert.assertEquals(mubir1.getBirthMonth(),_birth_monthN);
			Assert.assertEquals(mubir1.getCityCode(),_city_codeN);
			Assert.assertEquals(mubir1.getGender(),_genderN);
			Assert.assertEquals(mubir1.getHomeCity(),_home_cityN);
			Assert.assertEquals(mubir1.getHomeProvince(),_home_provinceN);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
		checkUpdateId();
	}
	
	private static class MyUserBornInfo extends UserBornInfo{

		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		
	}
	
	private static class MyUserBornFactory implements UserBornFactory<MyUserBornInfo>{
		public MyUserBornInfo create() {
			return new MyUserBornInfo();
		}		
	}
}
