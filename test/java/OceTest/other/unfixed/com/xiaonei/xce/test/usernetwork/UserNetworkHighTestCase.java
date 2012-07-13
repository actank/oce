package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import junit.framework.Assert;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.HighSchoolInfo;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class UserNetworkHighTestCase {
	//HighSchool
private 		int _id;
private 		int _userId;
private 		int _highschoolId;
private 		String _highschoolName;
private 		int _enrollYear;
private 		String _hClass1;
private 		String _hClass2;
private 		String _hClass3;
	
	public UserNetworkHighTestCase (int id, int userId, int highschoolId, String highschoolName,  int enrollYear, String hClass1, String hClass2,String hClass3){
		_id = id;
		_userId = userId;
		_highschoolId = highschoolId ;
		_highschoolName = highschoolName ;
		_enrollYear = enrollYear ;
		
		_hClass1 = hClass1 ;
		_hClass2 = hClass2 ;
		_hClass3 = hClass3 ;
		
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6815,400000473,10,"provinceName1",22,"provinceName2","cityName2","universityName1"},
				});
	}
	
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into high_school_info(id,userid,high_school_id,high_school_name,enroll_year,h_class1,h_class2,h_class3) values ("+_id+","+_userId+","+_highschoolId+",'"+_highschoolName+"','"+_enrollYear+"','"+_hClass1+"','"+_hClass2+"','"+_hClass3+"')";
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
	public void test() throws InterruptedException {
	//check reload and get	
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadHighSchoolInfoList(_userId);
		Thread.sleep(2000);
		
		HighSchoolInfo info1 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info1);
		Thread.sleep(2000);
	
	//check set mem
		HighSchoolInfo newInfo1 = new HighSchoolInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setHighSchoolId(_highschoolId);
		newInfo1.setHighSchoolName(_highschoolName);
		newInfo1.setEnrollYear(_enrollYear);
		newInfo1.setHClass1(_hClass1);
		newInfo1.setHClass2(_hClass2);
		newInfo1.setHClass3(_hClass3);
		_apt.setHighSchoolInfo(newInfo1);
		Thread.sleep(2000);
		
		HighSchoolInfo info2 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info2);
		Thread.sleep(2000);
	
	//check set db
		_apt.reloadHighSchoolInfoList(_userId);
		Thread.sleep(2000);
		
		HighSchoolInfo info3 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info3);
		Thread.sleep(2000);
	
	//check remove db
	
		 _apt.reloadHighSchoolInfoList(_userId);
		Thread.sleep(2000);
		
		HighSchoolInfo info4 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info4);
		Thread.sleep(2000);
		
	//check remove db
		_apt.reloadHighSchoolInfoList(_userId);
		Thread.sleep(2000);
		
		HighSchoolInfo info5 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info5);
		Thread.sleep(2000);
		
	//check add mem
		HighSchoolInfo newInfo2 = new HighSchoolInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setHighSchoolId(_highschoolId);
		newInfo1.setHighSchoolName(_highschoolName);
		newInfo1.setEnrollYear(_enrollYear);
		newInfo1.setHClass1(_hClass1);
		newInfo1.setHClass2(_hClass2);
		newInfo1.setHClass3(_hClass3);
		_apt.addHighSchoolInfo(newInfo2);
		Thread.sleep(2000);
		
		HighSchoolInfo info6 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info6);
		Thread.sleep(2000);
	
	//check add db
		_apt.reloadHighSchoolInfoList(_userId);
		Thread.sleep(2000);
		
		HighSchoolInfo info7 = _apt.getHighSchoolInfoBySchoolId(_userId, _highschoolId);
		checkSet(info7);
		Thread.sleep(2000);
		
		
	}
	
	private void checkSet(HighSchoolInfo info){
		Assert.assertEquals(info.getUserId(), _userId);
		Assert.assertEquals(info.getHighSchoolId(), _highschoolId);
		Assert.assertEquals(info.getHighSchoolName(), _highschoolName);		
		Assert.assertEquals(info.getEnrollYear(), _enrollYear);
		Assert.assertEquals(info.getHClass1(), _hClass1);
		Assert.assertEquals(info.getHClass2(), _hClass2);
		Assert.assertEquals(info.getHClass3(), _hClass3);
	}
}
