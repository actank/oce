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

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userstate.UserStateAdapter;
import com.xiaonei.xce.userstate.UserStateFactory;
import com.xiaonei.xce.userstate.UserStateInfo;

@RunWith (Parameterized.class)
public class UserStateTestCase {
	private int _id;
	private int _levelO;
	private int _starO;
	private int _starN;
	private int _levelN;
	
	public UserStateTestCase(int id,  int levelO, int starO,  int levelN, int starN){
		_id=id;
		_levelO=levelO;
		_starO=starO;
		_starN=starN;
		_levelN=levelN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,1,1,2,2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_state where id="+_id;
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
		UserStateAdapter<MyUserStateInfo> usa=new UserStateAdapter<MyUserStateInfo>(new MyUserStateFactory());
		MyUserStateInfo musiw1=new MyUserStateInfo();
		musiw1.setId(_id);
		musiw1.setLevel(_levelO);
		musiw1.setStar(_starO);
		usa.createUserState(_id, musiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, level, star, state FROM user_state WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("state"),0);
						Assert.assertEquals(rs.getInt("star"),_starO);
						Assert.assertEquals(rs.getInt("level"),_levelO);
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
			MyUserStateInfo musir1=usa.getUserState(_id);
			Assert.assertEquals(musir1.getId(),_id);
			Assert.assertEquals(musir1.getLevel(),_levelO);
			Assert.assertEquals(musir1.getStar(),_starO);
			Assert.assertEquals(musir1.getState(),0);
			Assert.assertEquals(musir1.isMobileBind(),false);
			Assert.assertEquals(musir1.isMSNIMME(),false);
			Assert.assertEquals(musir1.isSelected(),false);
			Assert.assertEquals(musir1.isVipMember(),false);
			Assert.assertEquals(musir1.isEverVipMember(), false);
			Assert.assertEquals(musir1.isSocialGraphGood(), false);
		}catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserStateInfo musiw2=new MyUserStateInfo();
		musiw2.setLevel(_levelN);
		musiw2.setStar(_starN);
		usa.setUserState(_id, musiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, level, star, state FROM user_state WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("star"),_starN);
						Assert.assertEquals(rs.getInt("level"),_levelN);
						Assert.assertEquals(rs.getInt("state"),0);
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
			MyUserStateInfo musir2=usa.getUserState(_id);
			Assert.assertEquals(musir2.getId(),_id);
			Assert.assertEquals(musir2.getLevel(),_levelN);
			Assert.assertEquals(musir2.getStar(),_starN);
			Assert.assertEquals(musir2.isMobileBind(),false);
			Assert.assertEquals(musir2.isMSNIMME(),false);
			Assert.assertEquals(musir2.isSelected(),false);
			Assert.assertEquals(musir2.isVipMember(),false);
			Assert.assertEquals(musir2.isEverVipMember(),false);
			Assert.assertEquals(musir2.isSocialGraphGood(),false);
			Assert.assertEquals(musir2.getState(),0);
		}catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
		usa.setMobileBindOn(_id);
		usa.setMSNIMMEOn(_id);
		usa.setSelectedOn(_id);
		usa.setVipMemberOn(_id);
		usa.setEverVipMemberOn(_id);
		usa.setSocialGraphOn(_id);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, star,level,state FROM user_state WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("state"),95);
						Assert.assertEquals(rs.getInt("star"),_starN);
						Assert.assertEquals(rs.getInt("level"),_levelN);
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
			MyUserStateInfo musir3=usa.getUserState(_id);
			Assert.assertEquals(musir3.getId(),_id);
			Assert.assertEquals(musir3.getLevel(),_levelN);
			Assert.assertEquals(musir3.getStar(),_starN);
			Assert.assertEquals(musir3.isMobileBind(),true);
			Assert.assertEquals(musir3.isMSNIMME(),true);
			Assert.assertEquals(musir3.isSelected(),true);
			Assert.assertEquals(musir3.isVipMember(),true);
			Assert.assertEquals(musir3.isEverVipMember(),true);
			Assert.assertEquals(musir3.isSocialGraphGood(), true);
			Assert.assertEquals(musir3.getState(),95);
		}catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
		usa.setMobileBindOff(_id);
		usa.setMSNIMMEOff(_id);
		usa.setSelectedOff(_id);
		usa.setVipMemberOff(_id);
		usa.setEverVipMemberOff(_id);
		usa.setSocialGraphOff(_id);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, star,level,state FROM user_state WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("state"),0);
						Assert.assertEquals(rs.getInt("star"),_starN);
						Assert.assertEquals(rs.getInt("level"),_levelN);
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
			MyUserStateInfo musir4=usa.getUserState(_id);
			Assert.assertEquals(musir4.getId(),_id);
			Assert.assertEquals(musir4.getLevel(),_levelN);
			Assert.assertEquals(musir4.getStar(),_starN);
			Assert.assertEquals(musir4.isMobileBind(),false);
			Assert.assertEquals(musir4.isMSNIMME(),false);
			Assert.assertEquals(musir4.isSelected(),false);
			Assert.assertEquals(musir4.isVipMember(),false);
			Assert.assertEquals(musir4.isEverVipMember(),false);
			Assert.assertEquals(musir4.isSocialGraphGood(), false);
			Assert.assertEquals(musir4.getState(),0);
		}catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserStateInfo extends UserStateInfo{
	}
	
	private static class MyUserStateFactory implements UserStateFactory<MyUserStateInfo>{
		public MyUserStateInfo create() {
			return new MyUserStateInfo();
		}
	}
}
