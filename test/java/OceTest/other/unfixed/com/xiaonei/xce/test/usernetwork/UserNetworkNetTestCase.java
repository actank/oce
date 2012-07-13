package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.List;

import junit.framework.Assert;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.Network;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class UserNetworkNetTestCase {
	//Region
	private int _id;
	private int _userId;
	private int _regionId;
	private String _provinceName;
	private String _cityName;
	
	private int _regionId2;
	private String _provinceName2;
	private String _cityName2;
	//University
	private int _universityId;
	private String _universityName;
	private String _dorm;
	private String _department;
	private String _typeOfCourse;
	private int _enrollYear;
	//Network
	private int _infoId;
	private int _stage;
	//private Date _joinTime;
	private int _status;
	private String _networkName;
	private int _infoType;
	private int _networkId;
	//Workplace
	private int _workplaceId;
	private String _workplaceName;
	private String _address;
	private String _description;
	private String _province;
	private int _cityId;
	private int _joinYear;
	private int _joinMonth;
	private int _quitYear;
	private int _quitMonth;
	private int _type;
	private int _jobTitleId;
	private int _positionId;
	
	public UserNetworkNetTestCase (int id, int userId, int regionId, String provinceName, String cityName, int regionId2, String provinceName2, String cityName2,
			int universityId, String universityName, String dorm, String department, String typeOfCourse, int enrollYear,
			int infoId, int stage,  int status, String networkName, int infoType, int networkId,
			int workplaceId, String workplaceName, String address, String description, String province, int cityId,
			int joinYear, int joinMonth, int quitYear, int quitMonth, int type, int jobTitleId, int positionId){
		_id = id;
		_userId = userId;
		_regionId = regionId;
		_provinceName = provinceName;
		_cityName = cityName;
		
		_regionId2 = regionId2;
		_provinceName2 = provinceName2;
		_cityName2 = cityName2;
		
		_universityId = universityId;
		_universityName = universityName;
		_dorm = dorm;
		_department = department;
		_typeOfCourse = typeOfCourse;
		_enrollYear = enrollYear;
		
		_infoId = infoId;
		_stage = stage;
		//_joinTime = joinTime;
		_status = status;
		_networkName = networkName;
		_infoType = infoType;
		_networkId = networkId;
		
		_workplaceId = workplaceId;
		_workplaceName = workplaceName;
		_address = address;
		_description = description;
		_province = province;
		_cityId = cityId;
		_joinYear = joinYear;
		_joinMonth = joinMonth;
		_quitYear = quitYear;
		_quitMonth = quitMonth;
		_type =type;
		_jobTitleId = jobTitleId;
		_positionId = positionId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6815,400000473,10,"provinceName1","cityName1",22,"provinceName2","cityName2",1004,"universityName1","dorm1","department1","typeOfCourse1",2006,
						1,1,1,"networkName1",1,1,1,"workplaceName1","address1","description1","province1",1,2006,7,2009,7,1,1,1},
				});
	}
	
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into network_history(id,userid,stage,info_id,status,network_name,network_id) values ("+_id+","+_userId+","+_stage+",'"+_infoId+"','"+_status+"','"+_networkName+"',"+_networkId+")";
					System.out.println(sql);
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
	//check get mem	
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadNetworkList(_userId);
		Thread.sleep(2000);
		
		Network info1 = _apt.getNetwork(_userId, _networkId);
		System.out.println(_userId + "  " + _networkId);
		checkSet(info1);
		Thread.sleep(2000);
	
	//check set mem	
		Network newInfo1 = new Network();
		newInfo1.setUserId(_userId);
		newInfo1.setNetworkId(_networkId);
		newInfo1.setNetworkName(_networkName);
		newInfo1.setStage(_stage);
		newInfo1.setInfoId(_infoId);
		newInfo1.setStatus(_status);
		_apt.setNetwork(newInfo1);
		Thread.sleep(2000);
		
		Network info2 = _apt.getNetwork(_userId, _networkId);
		checkSet(info2);
		Thread.sleep(2000);

	//check set db	
		_apt.reloadNetworkList(_userId);
		
		Network info3 = _apt.getNetwork(_userId, _networkId);
		checkSet(info3);
		Thread.sleep(2000);
	
	//check remove mem	

		_apt.removeNetwork(_userId, _networkId);
		Thread.sleep(2000);
		
		Network info4 = _apt.getNetwork(_userId, _networkId);
		if(info4==null){
			Assert.assertTrue(true);
		}else {
			Assert.assertTrue(false);
		}
		/*checkSet(info4);
		Thread.sleep(2000);*/
		
	//check remove db

		_apt.reloadNetworkList(_userId);
		Thread.sleep(2000);
		
		Network info5 = _apt.getNetwork(_userId, _networkId);
		if(info5==null){
			Assert.assertTrue(true);
		}
		
	
	//check add mem
		
		Network newInfo2 = new Network();
		newInfo2.setUserId(_userId);
		newInfo2.setNetworkId(_networkId);
		newInfo2.setNetworkName(_networkName);
		newInfo2.setStage(_stage);
		newInfo2.setInfoId(_infoId);
		newInfo2.setStatus(_status);
		_apt.addNetwork(newInfo2);
		Thread.sleep(2000);
		
		Network info6 = _apt.getNetwork(_userId, _networkId);
		checkSet(info6);
		Thread.sleep(2000);
	//check add db	
		
		_apt.reloadNetworkList(_userId);
		Thread.sleep(2000);
		
		Network info7 = _apt.getNetwork(_userId, _networkId);
		checkSet(info7);
		Thread.sleep(2000);
	
	//check removebystage mem
		
		_apt.removeNetworkByStage(_userId,_stage);
		Thread.sleep(2000);
		
		Network info8 = _apt.getNetwork(_userId, _networkId);
		if (info8==null){
			Assert.assertTrue(true);
		}else {
			Assert.assertTrue(false);
		}
		Thread.sleep(2000);
		
	//check removebystage db
		
		_apt.reloadNetworkList(_userId);
		Thread.sleep(2000);
		
		Network info9 = _apt.getNetwork(_userId, _networkId);
		if (info9==null){
			Assert.assertTrue(true);
		}else {
			Assert.assertTrue(false);
		}
		Thread.sleep(2000);
		
	}
	
	private void checkSet(Network info){
		Assert.assertEquals(info.getUserId(), _userId);
		Assert.assertEquals(info.getStage(), _stage);
		Assert.assertEquals(info.getStatus(), _status);		
		Assert.assertEquals(info.getInfoId(), _infoId);
		Assert.assertEquals(info.getNetworkId(), _networkId);
		Assert.assertEquals(info.getNetworkName(), _networkName);		
	}
}
