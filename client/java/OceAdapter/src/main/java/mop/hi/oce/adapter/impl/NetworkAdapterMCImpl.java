package mop.hi.oce.adapter.impl;

import java.util.List;

import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.CollegeInfo;
import mop.hi.oce.domain.network.ElementarySchoolInfo;
import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.RegionInfo;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;

import mop.hi.oce.domain.network.NetworkBigData;
import mop.hi.oce.domain.network.NetworkBigObject;
import mop.hi.oce.domain.network.NetworkBigObjectN;

public class NetworkAdapterMCImpl implements NetworkAdapter {

	private NetworkReaderAdapterMCImpl _readerAdapter = null;
	private NetworkWriterAdapterMCImpl _writerAdapter = null;

	public NetworkAdapterMCImpl() {
		_readerAdapter = new NetworkReaderAdapterMCImpl();
		_writerAdapter = new NetworkWriterAdapterMCImpl();
	}

	// ***********************************************************************

	public void reloadNetworkList(int userId) {
		_writerAdapter.reloadNetworkList(userId);
	}

	public Network addNetwork(Network network) {
		return _writerAdapter.addNetwork(network);
	}

	public void removeNetwork(int userId, int networkId) {
		_writerAdapter.removeNetwork(userId, networkId);
	}

	public void removeNetworkByStage(int userId, int stage) {
		_writerAdapter.removeNetworkByStage(userId, stage);
	}

	public void setNetwork(Network network) {
		_writerAdapter.setNetwork(network);
	}

	public int getNetworkCountByStage(int userId, int stage) {
		return _readerAdapter.getNetworkCountByStage(userId, stage);
	}

	public int getValidNetworkCountByStage(int userId, int stage) {
		return _readerAdapter.getValidNetworkCountByStage(userId, stage);
	}

	public List<Network> getNetworkList(int userId) {
		return _readerAdapter.getNetworkList(userId);
	}

	public List<Network> getNetworkListByStage(int userId, int stage) {
		return _readerAdapter.getNetworkListByStage(userId, stage);
	}

	public List<Network> getNetworkListByStatus(int userId, int status) {
		return _readerAdapter.getNetworkListByStatus(userId, status);
	}

	public List<Network> getNetworkListByStageAndStatus(int userId, int stage, int[] status) {
		return _readerAdapter.getNetworkListByStageAndStatus(userId, stage, status);
	}

	public List<Network> getNetworkListByStatus(int userId, int[] status) {
		return _readerAdapter.getNetworkListByStatus(userId, status);
	}

	public Network getNetwork(int userId, int networkId) {
		return _readerAdapter.getNetwork(userId, networkId);
	}

	public Network getRegionalNetwork(int userId) {
		return _readerAdapter.getRegionalNetwork(userId);
	}

	public boolean isExistInNetwork(int userId, int networkid) {
		return _readerAdapter.isExistInNetwork(userId, networkid);
	}

	// ***********************************************************************

	public void reloadHighSchoolInfoList(int userId) {
		_writerAdapter.reloadHighSchoolInfoList(userId);
	}

	public HighSchoolInfo addHighSchoolInfo(HighSchoolInfo highSchool) {
		return _writerAdapter.addHighSchoolInfo(highSchool);
	}

	public void removeAllHighSchoolInfo(int userId) {
		_writerAdapter.removeAllHighSchoolInfo(userId);
	}

	public void setHighSchoolInfo(HighSchoolInfo highSchool) {
		_writerAdapter.setHighSchoolInfo(highSchool);
	}

	public int getHighSchoolInfoCount(int userId) {
		return _readerAdapter.getHighSchoolInfoCount(userId);
	}

	public List<HighSchoolInfo> getHighSchoolInfoList(int userId) {
		return _readerAdapter.getHighSchoolInfoList(userId);
	}

	public HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int schoolId) {
		return _readerAdapter.getHighSchoolInfoBySchoolId(userId, schoolId);
	}

	// ***********************************************************************

	public void reloadUniversityInfoList(int userId) {
		_writerAdapter.reloadUniversityInfoList(userId);
	}

	public UniversityInfo addUniversityInfo(UniversityInfo university) {
		return _writerAdapter.addUniversityInfo(university);
	}

	public void removeAllUniversityInfo(int userId) {
		_writerAdapter.removeAllUniversityInfo(userId);
	}

	public void removeUniversityInfoByUnivId(int userId, int univId) {
		_writerAdapter.removeUniversityInfoByUnivId(userId, univId);
	}

	public void setUniversityInfo(UniversityInfo universityInfo) {
		_writerAdapter.setUniversityInfo(universityInfo);
	}

	public int getUniversityInfoCount(int userId) {
		return _readerAdapter.getUniversityInfoCount(userId);
	}

	public List<UniversityInfo> getUniversityInfoList(int userId) {
		return _readerAdapter.getUniversityInfoList(userId);
	}

	public UniversityInfo getUniversityInfoByUnivId(int userId, int univId) {
		return _readerAdapter.getUniversityInfoByUnivId(userId, univId);
	}

	// ***********************************************************************

	public void reloadWorkplaceInfoList(int userId) {
		_writerAdapter.reloadWorkplaceInfoList(userId);
	}

	public WorkplaceInfo addWorkplaceInfo(WorkplaceInfo workplace) {
		return _writerAdapter.addWorkplaceInfo(workplace);
	}

	public void removeAllWorkplaceInfo(int userId) {
		_writerAdapter.removeAllWorkplaceInfo(userId);
	}

	public void removeWorkplaceInfoByWorkId(int userId, int workId) {
		_writerAdapter.removeWorkplaceInfoByWorkId(userId, workId);
	}

	public void setWorkplaceInfo(WorkplaceInfo workplace) {
		_writerAdapter.setWorkplaceInfo(workplace);
	}

	public int getWorkplaceInfoCount(int userId) {
		return _readerAdapter.getWorkplaceInfoCount(userId);
	}

	public List<WorkplaceInfo> getWorkplaceInfoList(int userId) {
		return _readerAdapter.getWorkplaceInfoList(userId);
	}

	public WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int workId) {
		return _readerAdapter.getWorkplaceInfoByWorkId(userId, workId);
	}

	// ***********************************************************************

	public void reloadCollegeInfoList(int userId) {
		_writerAdapter.reloadCollegeInfoList(userId);
	}

	public CollegeInfo addCollegeInfo(int userId, CollegeInfo collegeInfo) {
		return _writerAdapter.addCollegeInfo(userId, collegeInfo);
	}

	public void removeAllCollegeInfo(int userId) {
		_writerAdapter.removeAllCollegeInfo(userId);
	}

	public void removeCollegeInfo(int userId, int id) {
		_writerAdapter.removeCollegeInfo(userId, id);
	}

	public void setCollegeInfo(int userId, int id, CollegeInfo collegeInfo) {
		_writerAdapter.setCollegeInfo(userId, id, collegeInfo);
	}

	public int getCollegeInfoCount(int userId) {
		return _readerAdapter.getCollegeInfoCount(userId);
	}

	public List<CollegeInfo> getCollegeInfoList(int userId) {
		return _readerAdapter.getCollegeInfoList(userId);
	}

	public CollegeInfo getCollegeInfo(int userId, int id) {
		return _readerAdapter.getCollegeInfo(userId, id);
	}

	public CollegeInfo getCollegeInfoByCollegeId(int userId, int id) {
		return _readerAdapter.getCollegeInfoByCollegeId(userId, id);
	}

	// ***********************************************************************

	public void reloadElementarySchoolInfo(int userId) {
		_writerAdapter.reloadElementarySchoolInfo(userId);
	}

	public ElementarySchoolInfo addElementarySchoolInfo(int userId, ElementarySchoolInfo info) {
		return _writerAdapter.addElementarySchoolInfo(userId, info);
	}

	public void removeAllElementarySchoolInfo(int userId) {
		_writerAdapter.removeAllElementarySchoolInfo(userId);
	}

	public void removeElementarySchoolInfo(int userId) {
		_writerAdapter.removeElementarySchoolInfo(userId);
	}

	public void removeElementarySchoolInfo(int userId, int id) {
		_writerAdapter.removeElementarySchoolInfo(userId, id);
	}

	public void setElementarySchoolInfo(ElementarySchoolInfo info) {
		_writerAdapter.setElementarySchoolInfo(info);
	}

	public List<ElementarySchoolInfo> getElementarySchoolInfoList(int userId) {
		return _readerAdapter.getElementarySchoolInfoList(userId);
	}

	public ElementarySchoolInfo getElementarySchoolInfo(int userId) {
		return _readerAdapter.getElementarySchoolInfo(userId);
	}

	public ElementarySchoolInfo getElementarySchoolInfo(int userId, int id) {
		return _readerAdapter.getElementarySchoolInfo(userId, id);
	}

	public ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id) {
		return _readerAdapter.getElementarySchoolInfoBySchoolId(userId, id);
	}

	// ***********************************************************************

	public void reloadJuniorHighSchoolInfo(int userId) {
		_writerAdapter.reloadJuniorHighSchoolInfo(userId);
	}

	public JuniorHighSchoolInfo addJuniorHighSchoolInfo(int userId, JuniorHighSchoolInfo info) {
		return _writerAdapter.addJuniorHighSchoolInfo(userId, info);
	}

	public void removeAllJuniorHighSchoolInfo(int userId) {
		_writerAdapter.removeAllJuniorHighSchoolInfo(userId);
	}

	public void removeJuniorHighSchoolInfo(int userId) {
		_writerAdapter.removeJuniorHighSchoolInfo(userId);
	}

	public void removeJuniorHighSchoolInfo(int userId, int id) {
		_writerAdapter.removeJuniorHighSchoolInfo(userId, id);
	}

	public void setJuniorHighSchoolInfo(JuniorHighSchoolInfo info) {
		_writerAdapter.setJuniorHighSchoolInfo(info);
	}

	public List<JuniorHighSchoolInfo> getJuniorHighSchoolInfoList(int userId) {
		return _readerAdapter.getJuniorHighSchoolInfoList(userId);
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId) {
		return _readerAdapter.getJuniorHighSchoolInfo(userId);
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId, int id) {
		return _readerAdapter.getJuniorHighSchoolInfo(userId, id);
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id) {
		return _readerAdapter.getJuniorHighSchoolInfoBySchoolId(userId, id);
	}

	// ***********************************************************************

	public void reloadRegionInfo(int userId) {
		_writerAdapter.reloadRegionInfo(userId);
	}

	public RegionInfo addRegionInfo(RegionInfo info) {
		return _writerAdapter.addRegionInfo(info);
	}

	public void removeAllRegionInfo(int userId) {
		_writerAdapter.removeAllRegionInfo(userId);
	}

	public void removeRegionInfo(int userId, int regionId) {
		_writerAdapter.removeRegionInfo(userId, regionId);
	}

	public void setRegionInfo(RegionInfo info) {
		_writerAdapter.setRegionInfo(info);
	}

	public List<RegionInfo> getRegionInfoList(int userId) {
		return _readerAdapter.getRegionInfoList(userId);
	}

	public RegionInfo getRegionInfo(int userId, int regionId) {
		return _readerAdapter.getRegionInfo(userId, regionId);
	}

	// ***********************************************************************

	@Deprecated
	public NetworkBigObject getNetworkBigObject(int userId) {
		return _readerAdapter.getNetworkBigObject(userId);
	}

	public NetworkBigObjectN getNetworkBigObjectN(int userId) {
		return _readerAdapter.getNetworkBigObjectN(userId);
	}

	public NetworkBigData getNetworkBigData(int userId) {
		return _readerAdapter.getNetworkBigData(userId);
	}
//
//	//保留供开发者快速调用
//	public static void main(String[] args) {
//		int userId = 35178;
//		NetworkAdapter networkAdapter = new NetworkAdapterMCImpl();
//		NetworkBigData nbd = networkAdapter.getNetworkBigData(userId);
//
//		System.out.println("\ngetNetworks()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getNetworks());		
//		System.out.println("\ngetCollages()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getCollages());
//		System.out.println("\ngetElementaryschool()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getElementaryschools());
//		System.out.println("\ngetHighschools()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getHighschools());
//		System.out.println("\ngetJuniorhighschool()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getJuniorhighschools());
//		System.out.println("\ngetNetworks()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getNetworks());
//		System.out.println("\ngetUnivs()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getUnivs());
//		System.out.println("\ngetWorkspaces()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getWorkspaces());
//		System.out.println("\ngetRegions()");
//		System.out.println("--------------------------------");
//		System.out.println(nbd.getRegions());
//
//		System.exit(0);
//	}

}
