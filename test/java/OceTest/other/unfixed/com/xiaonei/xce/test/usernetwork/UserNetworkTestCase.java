package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import junit.framework.Assert;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.RegionInfo;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class UserNetworkTestCase {
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
	private String _joinTime;
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
	
	public UserNetworkTestCase (int id, int userId, int regionId, String provinceName, String cityName, int regionId2, String provinceName2, String cityName2,
			int universityId, String universityName, String dorm, String department, String typeOfCourse, int enrollYear,
			int infoId, int stage, String joinTime, int status, String networkName, int infoType, int networkId,
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
		_joinTime = joinTime;
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
						1,1,"joinTime1",1,"networkName1",1,1,1,"workplaceName1","address1","description1","province1",1,2006,7,2009,7,1,1,1},
				});
	}
	
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into region_info(id,userid,region_id,province_name,city_name) values ("+_id+","+_userId+","+_regionId+",'"+_provinceName+"','"+_cityName+"')";
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
		
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadRegionInfo(_userId);
		Thread.sleep(2000);
		
		RegionInfo info1 = _apt.getRegionInfo(_userId, _regionId);
		System.out.println(_userId + "  " + _regionId);
		check1(info1);
		Thread.sleep(2000);
		
		RegionInfo newInfo1 = new RegionInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setRegionId(_regionId);
		newInfo1.setProvinceName(_provinceName2);
		newInfo1.setCityName(_cityName2);
		_apt.setRegionInfo(newInfo1);
		//_apt.reloadRegionInfo(_userId);
		Thread.sleep(2000);
		
		RegionInfo info2 = _apt.getRegionInfo(_userId, _regionId);
		checkSet(info2);
		Thread.sleep(2000);
		
		_apt.removeRegionInfo(_userId, _regionId2);
		Thread.sleep(2000);
		
		
		RegionInfo newInfo2 = new RegionInfo();
		newInfo2.setUserId(_userId);
		newInfo2.setProvinceName(_provinceName2);
		newInfo2.setCityName(_cityName2);
		_apt.addRegionInfo(newInfo2);
		//_apt.reloadRegionInfo(_userId);
		List<RegionInfo> regionlist = _apt.getRegionInfoList(_userId);
		for (int i=0; i< regionlist.size(); ++i){
			if(regionlist.get(i).getRegionId() == _regionId2)
				check2(regionlist.get(i));
		}
		Thread.sleep(2000);
		
		_apt.removeAllRegionInfo(_userId);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}	
	}
	
	private void check1(RegionInfo info){
		Assert.assertEquals(info.getId(), _id);
		Assert.assertEquals(info.getUserId(), _userId);
		Assert.assertEquals(info.getRegionId(), _regionId);
		Assert.assertEquals(info.getProvinceName(), _provinceName);		
		Assert.assertEquals(info.getCityName(), _cityName);
	}
	
	private void check2(RegionInfo info){
		System.out.println("userId:" + info.getUserId());
		Assert.assertEquals(info.getUserId(), _userId);
		System.out.println("regionId:" + info.getRegionId());
		Assert.assertEquals(info.getRegionId(), _regionId2);
		Assert.assertEquals(info.getProvinceName(), _provinceName2);		
		Assert.assertEquals(info.getCityName(), _cityName2);
	}
	
	private void checkSet(RegionInfo info){
		System.out.println("userId:" + info.getUserId());
		Assert.assertEquals(info.getUserId(), _userId);
		System.out.println("regionId:" + info.getRegionId());
		Assert.assertEquals(info.getRegionId(), _regionId);
		Assert.assertEquals(info.getProvinceName(), _provinceName2);		
		Assert.assertEquals(info.getCityName(), _cityName2);
	}
}
