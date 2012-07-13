package com.xiaonei.xcetest.invoke;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;
import java.util.List;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.NetworkAdapterImpl;
import mop.hi.oce.domain.network.CollegeInfo;
import mop.hi.oce.domain.network.ElementarySchoolInfo;
import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.NetworkBigObjectN;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;
import MyUtil.ObjectCache;

import com.xiaonei.xce.XceAdapter;

public class NetworkTester extends TestCase {
	NetworkAdapterImpl adapter = new NetworkAdapterImpl();

	public void testgetUniversityInfo() {

		System.out.println(adapter.getUniversityInfoList(32580219).get(0)
				.toString());
	}

	public void testgetHighSchoolInfo() {

		System.out.println(adapter.getHighSchoolInfoList(32580219).get(0)
				.toString());
	}

	public void testgetWorkplaceInfo() {

		System.out.println(adapter.getWorkplaceInfoList(32580219).get(0)
				.toString());
	}

	public void testaddUniversityInfo() {
		UniversityInfo info = new UniversityInfo();
		info.setDepartment("计算机学院");
		info.setDorm("信息学部8栋308");
		info.setEnrollYear(2005);
		info.setUserId(32580219);
		info.setUniversityId(0001);
		info.setUniversityName("武汉大学");
		adapter.addUniversityInfo(info);

		Connection conn = null;
		ResultSet rs = null;
		Statement stmt = null;
		try {
			conn = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_COMMON);
			if (conn == null) {
				System.out.println("conn is empty");
			}

			stmt = conn.createStatement();

			if (stmt == null) {
				System.out.println("stmt is empty");
			}

			rs = stmt
					.executeQuery("select * from university_info where userid = "
							+ info.getUserId());

			if (rs == null) {
				System.out.println("rs is empty");
			}

			assertTrue(rs.next());

		} catch (SQLException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				rs.close();
				stmt.close();
				conn.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}

	}

	public void testaddHighSchoolInfo() {
		HighSchoolInfo info = new HighSchoolInfo();
		info.setEnrollYear(2002);
		info.setHClass1("高一20班");
		info.setHClass2("高二2班");
		info.setHClass3("高三2班");
		info.setHighSchoolId(0001);
		info.setHighSchoolName("江苏省东台中学");
		info.setUserId(32580219);
		adapter.addHighSchoolInfo(info);

		Connection conn = null;
		ResultSet rs = null;
		Statement stmt = null;
		try {
			conn = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_COMMON);
			if (conn == null) {
				System.out.println("conn is empty");
			}

			stmt = conn.createStatement();

			if (stmt == null) {
				System.out.println("stmt is empty");
			}

			rs = stmt
					.executeQuery("select * from high_school_info where userid = "
							+ info.getUserId());

			if (rs == null) {
				System.out.println("rs is empty");
			}

			assertTrue(rs.next());

		} catch (SQLException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				rs.close();
				stmt.close();
				conn.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}

	}

	public void testaddWorkplaceInfo() {
		WorkplaceInfo info = new WorkplaceInfo();
		info.setAddress("朝阳区静安中心");
		info.setDescription("校内网");
		info.setJoinMonth(1);
		info.setJoinYear(2008);
		info.setUserId(32580219);
		info.setWorkplaceId(0001);
		info.setWorkplaceName("XiaoNei");
		info.setJobTitleId(888);
		info.setPositionId(999);
		info.setCityId(001);
		info.setCityName("北京");
		info.setProvince("北京");
		info.setType(1);
		info.setQuitMonth(1);
		info.setQuitYear(2008);
		adapter.addWorkplaceInfo(info);

		Connection conn = null;
		ResultSet rs = null;
		Statement stmt = null;
		try {
			conn = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_COMMON);
			if (conn == null) {
				System.out.println("conn is empty");
			}

			stmt = conn.createStatement();

			if (stmt == null) {
				System.out.println("stmt is empty");
			}

			rs = stmt
					.executeQuery("select * from workplace_info where userid = "
							+ info.getUserId());

			if (rs == null) {
				System.out.println("rs is empty");
			}

			assertTrue(rs.next());

		} catch (SQLException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				rs.close();
				stmt.close();
				conn.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}

	}

	public void testReload() throws Exception {
		adapter.reloadNetworkList(142443436);
		adapter.reloadElementarySchoolInfo(242322747);
		adapter.reloadJuniorHighSchoolInfo(242322747);
	}

	public void testNetwork() throws Exception {
		List<Network> aa = adapter.getNetworkList(142443436);
		for (Network network : aa) {
			System.out.print(network.getStatus());
			System.out.print("---");
			System.out.print(network.getId());
			System.out.print("---");
			System.out.print(network.getNetworkId());
			System.out.print("---");
			System.out.print(network.getInfoId());
			System.out.print("---");
			System.out.println(network.getNetworkName());
			// System.out.println(adapter.getNetworkByNetId(200000108,
			// network.getNetworkId()));
		}
	}

	public void testHighSchool() throws Exception {
		List<HighSchoolInfo> bb = adapter.getHighSchoolInfoList(230872262);
		for (HighSchoolInfo info : bb) {
			System.out.println(info.getHClass1() + "---" + info.getHClass2());
		}
	}

	public void testCollege() throws Exception {
		int userId = 242322747;
		int id = 80026254;
		CollegeInfo info = adapter.getCollegeInfo(userId, id);
		System.out.println(info.getCollegeName());

		List<CollegeInfo> infos = adapter.getCollegeInfoList(userId);
		for (CollegeInfo a : infos) {
			System.out.println(a.getCollegeId());
		}
	}

	public void testWorkplace() throws Exception {
		List<WorkplaceInfo> bb = adapter.getWorkplaceInfoList(20000);
		for (WorkplaceInfo info : bb) {
			System.out.println(info.getWorkplaceName() + "-----"
					+ info.getJoinYear() + "---" + info.getJoinMonth()
					+ "JOBTITLEID---" + info.getJobTitleId() + "Positon---"
					+ info.getPositionId());
		}
	}

	public void testBigObject() {
		NetworkBigObjectN obj = adapter.getNetworkBigObjectN(200353733);
		System.out.println(obj);
	}

	public void testGet() {
		List<Network> aa = adapter.getNetworkList(200000053);
		List<HighSchoolInfo> b = adapter.getHighSchoolInfoList(32580219);
		HighSchoolInfo b1 = b.get(0);
		b1.setHClass1("abcd");
		adapter.setHighSchoolInfo(b1);

		List<UniversityInfo> u = adapter.getUniversityInfoList(32580219);
		UniversityInfo u1 = u.get(0);
		System.out.print(u1.toString());
		u1.setDepartment("sss");

		adapter.setUniversityInfo(u1);

		Network a = aa.get(0);
		System.out.println(a.getStatus());
		a.setStatus(3);
		adapter.setNetwork(a);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println(a.getStatus());
		System.out.println(a.getStage());

		List<Network> list1 = adapter.getNetworkListByStage(400000165, 10);
		List<Network> list2 = adapter.getNetworkListByStage(400000165, 20);
		List<Network> list3 = adapter.getNetworkListByStage(400000165, 30);

		for (Network network : list1) {
			System.out.print(network.getStatus());
			System.out.print("---");
			System.out.print(network.getId());
			System.out.print("---");
			System.out.println(network.getInfoId());
		}
		for (Network network : list2) {
			System.out.print(network.getStatus());
			System.out.print("---");
			System.out.print(network.getId());
			System.out.print("---");
			System.out.println(network.getInfoId());
		}
		for (Network network : list3) {
			System.out.print(network.getStatus());
			System.out.print("---");
			System.out.print(network.getId());
			System.out.print("---");
			System.out.println(network.getInfoId());
		}
	}

	public void testADD2() {
		Network nw = new Network();
		nw.setStatus(2);
		nw.setNetworkName("111111");
		nw.setInfoId(111);
		nw.setStage(30);
		nw.setUserId(111);
		nw.setJoinTime(new Date());

		// try {
		// adapter.addNetwork(nw);
		// } catch (NetworkException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }

	}

	public void testQuery() throws Exception {
		List<UniversityInfo> u = adapter.getUniversityInfoList(32580219);
		System.out.println(u.size());
		UniversityInfo u1 = u.get(0);
		System.out.print(u1.toString());
	}

	public void test10() {
		System.out.print(adapter.getValidNetworkCountByStage(128487631, -1));
	}

	public void testElementary() {
		int userId = 202911262;

		ElementarySchoolInfo obj = adapter.getElementarySchoolInfo(userId);
		System.out.println(obj.getElementarySchoolName());
//
//		ElementarySchoolInfo info = new ElementarySchoolInfo();
//		info.setUserId(200353733);
//		info.setElementarySchoolId(111);
//		info.setElementarySchoolName("心小学1");
//		info.setElementarySchoolYear(1999);
//		System.out.println(info.getUpdates().size());
//		adapter.addElementarySchoolInfo(userId, info);
		// adapter.reloadElementarySchoolInfo(userId);
	}

	public void testJunior() {
		int userId = 202911262;
		JuniorHighSchoolInfo obj = adapter.getJuniorHighSchoolInfo(userId);
		System.out.println(obj.getJunHighentarySchoolName());
//		JuniorHighSchoolInfo info = new JuniorHighSchoolInfo();
//		info.setJunHighentarySchoolId(1);
//		info.setJunHighentarySchoolName("5心中学");
//		info.setJunHighentarySchoolYear(1000);
//		info.setUserId(200353733);
//		adapter.addJuniorHighSchoolInfo(userId, info);
	}

	public void reloadJunior() {
		int userId = 200353733;
		adapter.reloadJuniorHighSchoolInfo(userId);
	}
}
