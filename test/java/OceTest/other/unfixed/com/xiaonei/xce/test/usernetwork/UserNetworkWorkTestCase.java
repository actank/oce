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
import mop.hi.oce.domain.network.WorkplaceInfo;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class UserNetworkWorkTestCase {
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
	
	public UserNetworkWorkTestCase (int id, int userId, int regionId, String provinceName, String cityName, int regionId2, String provinceName2, String cityName2,
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
					String sql="replace into workplace_info(id,userid,workplace_id,workplace_name,address,description,province,city_id,join_year,join_month,quit_year,quit_month,type,job_title_id,position_id) values ("+_id+","+_userId+","+_workplaceId+",'"+_workplaceName+"','"+_address+"','"+_description+"','"+_province+"',"+_cityId+",'"+_joinYear+"','"+_joinMonth+"','"+_quitYear+"',"+_quitMonth+","+_type+","+_jobTitleId+",'"+_positionId+"')";
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
		
	//check reload and get	
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadWorkplaceInfoList(_userId);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info1 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info1);
		Thread.sleep(2000);
	
	//check set mem

		WorkplaceInfo newInfo1 = new WorkplaceInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setWorkplaceId(_workplaceId);
		newInfo1.setWorkplaceName(_workplaceName);
		newInfo1.setAddress(_address);
		newInfo1.setDescription(_description);
		newInfo1.setProvince(_province);
		newInfo1.setCityId(_cityId);
		newInfo1.setJoinYear(_joinYear);
		newInfo1.setJoinMonth(_joinMonth);
		newInfo1.setQuitYear(_quitYear);
		newInfo1.setQuitMonth(_quitMonth);
		newInfo1.setType(_type);
		newInfo1.setJobTitleId(_jobTitleId);
		newInfo1.setPositionId(_positionId);
		
		_apt.setWorkplaceInfo(newInfo1);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info2 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info2);
		Thread.sleep(2000);
	
	//check set db
		_apt.reloadWorkplaceInfoList(_userId);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info3 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info3);
		Thread.sleep(2000);
	
	//check remove db
	
		 _apt.reloadWorkplaceInfoList(_userId);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info4 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info4);
		Thread.sleep(2000);
		
	//check remove db
		_apt.reloadWorkplaceInfoList(_userId);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info5 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info5);
		Thread.sleep(2000);
		
	//check add mem
		WorkplaceInfo newInfo2 = new WorkplaceInfo();
		newInfo1.setUserId(_userId);
		newInfo1.setWorkplaceId(_workplaceId);
		newInfo1.setWorkplaceName(_workplaceName);
		newInfo1.setAddress(_address);
		newInfo1.setDescription(_description);
		newInfo1.setProvince(_province);
		newInfo1.setCityId(_cityId);
		newInfo1.setJoinYear(_joinYear);
		newInfo1.setJoinMonth(_joinMonth);
		newInfo1.setQuitYear(_quitYear);
		newInfo1.setQuitMonth(_quitMonth);
		newInfo1.setType(_type);
		newInfo1.setJobTitleId(_jobTitleId);
		newInfo1.setPositionId(_positionId);
		_apt.addWorkplaceInfo(newInfo2);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info6 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info6);
		Thread.sleep(2000);
	
	//check add db
		_apt.reloadWorkplaceInfoList(_userId);
		Thread.sleep(2000);
		
		List<WorkplaceInfo> info7 = _apt.getWorkplaceInfoList(_userId);
		checkSet(info7);
		Thread.sleep(2000);
		
		
	}
	
	
	private void checkSet(List<WorkplaceInfo> infoList){
		WorkplaceInfo info = new WorkplaceInfo();
		for (int i=0; i< infoList.size(); ++i){
			if(infoList.get(i).getWorkplaceId() == _workplaceId)
				info=infoList.get(i);
		}
		Assert.assertEquals(info.getUserId(), _userId);
		Assert.assertEquals(info.getWorkplaceId(), _workplaceId);
		Assert.assertEquals(info.getWorkplaceName(), _workplaceName);		
		Assert.assertEquals(info.getAddress(), _address);
		Assert.assertEquals(info.getDescription(), _description);
		Assert.assertEquals(info.getProvince(), _province);
		Assert.assertEquals(info.getCityId(), _cityId);		
		Assert.assertEquals(info.getJoinYear(), _joinYear);
		Assert.assertEquals(info.getJoinMonth(), _joinMonth);
		Assert.assertEquals(info.getQuitYear(), _quitYear);
		Assert.assertEquals(info.getQuitMonth(), _quitMonth);		
		Assert.assertEquals(info.getType(), _type);
		Assert.assertEquals(info.getJobTitleId(), _jobTitleId);		
		Assert.assertEquals(info.getPositionId(), _positionId);
	}
}
