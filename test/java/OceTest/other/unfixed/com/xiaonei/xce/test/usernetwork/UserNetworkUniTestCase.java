package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import junit.framework.Assert;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.UniversityInfo;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class UserNetworkUniTestCase {
	//University

	private 		int _userId;
	private 		int _id;

	private int _universityId;
	private String _universityName;
	private int _enrollYear;
	private String _dorm;
	private String _department;
	private String _typeOfCourse;
	
	public UserNetworkUniTestCase (int id, int userId, int universityId, String universityName,int enrollYear,  String dorm, String department,String typeOfCourse){
		_id = id;
		_userId = userId;
		_universityId= universityId;
		_universityName = universityName ;
		_enrollYear = enrollYear ;
		
		_dorm = dorm;
		_department = department ;
		_typeOfCourse = typeOfCourse;
		
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
					String sql="replace into university_info(id,userid,university_id,university_name,enroll_year,dorm,department,type_of_course) values ("+_id+","+_userId+","+_universityId+",'"+_universityName+"','"+_enrollYear+"','"+_dorm+"','"+_department+"','"+_typeOfCourse+"')";
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
		_apt.reloadUniversityInfoList(_userId);
		Thread.sleep(2000);
		
		List<UniversityInfo> info1 = _apt.getUniversityInfoList(_userId);
		checkSet(info1);
		Thread.sleep(2000);
	
	//check set mem
		UniversityInfo newInfo1 = new UniversityInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setUniversityId(_universityId);
		newInfo1.setUniversityName(_universityName);
		newInfo1.setEnrollYear(_enrollYear);
		newInfo1.setDorm(_dorm);
		newInfo1.setDepartment(_department);
		newInfo1.setTypeOfCourse(_typeOfCourse);
		_apt.setUniversityInfo(newInfo1);
		Thread.sleep(2000);
		
		List<UniversityInfo> info2 = _apt.getUniversityInfoList(_userId);
		checkSet(info2);
		Thread.sleep(2000);
	
	//check set db
		_apt.reloadUniversityInfoList(_userId);
		Thread.sleep(2000);
		
		List<UniversityInfo> info3 = _apt.getUniversityInfoList(_userId);
		checkSet(info3);
		Thread.sleep(2000);
	
	//check remove db
	
		 _apt.reloadUniversityInfoList(_userId);
		Thread.sleep(2000);
		
		List<UniversityInfo> info4 = _apt.getUniversityInfoList(_userId);
		checkSet(info4);
		Thread.sleep(2000);
		
	//check remove db
		_apt.reloadUniversityInfoList(_userId);
		Thread.sleep(2000);
		
		List<UniversityInfo> info5 = _apt.getUniversityInfoList(_userId);
		checkSet(info5);
		Thread.sleep(2000);
		
	//check add mem
		UniversityInfo newInfo2 = new UniversityInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setUniversityId(_universityId);
		newInfo1.setUniversityName(_universityName);
		newInfo1.setEnrollYear(_enrollYear);
		newInfo1.setDorm(_dorm);
		newInfo1.setDepartment(_department);
		newInfo1.setTypeOfCourse(_typeOfCourse);
		_apt.addUniversityInfo(newInfo2);
		Thread.sleep(2000);
		
		List<UniversityInfo> info6 = _apt.getUniversityInfoList(_userId);
		checkSet(info6);
		Thread.sleep(2000);
	
	//check add db
		_apt.reloadUniversityInfoList(_userId);
		Thread.sleep(2000);
		
		List<UniversityInfo> info7 = _apt.getUniversityInfoList(_userId);
		checkSet(info7);
		Thread.sleep(2000);
		
		
	}
	
	private void checkSet(List<UniversityInfo> infoList){
		UniversityInfo info = new UniversityInfo();
		for (int i=0; i< infoList.size(); ++i){
			if(infoList.get(i).getUniversityId() == _universityId)
				info=infoList.get(i);
		}
		Assert.assertEquals(info.getUserId(), _userId);
		Assert.assertEquals(info.getUniversityId(), _universityId);
		Assert.assertEquals(info.getUniversityName(), _universityName);		
		Assert.assertEquals(info.getEnrollYear(), _enrollYear);
		Assert.assertEquals(info.getDorm(), _dorm);
		Assert.assertEquals(info.getDepartment(), _department);
		Assert.assertEquals(info.getTypeOfCourse(), _typeOfCourse);
	}
}
