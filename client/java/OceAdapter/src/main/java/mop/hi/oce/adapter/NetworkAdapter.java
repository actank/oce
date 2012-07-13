package mop.hi.oce.adapter;

import java.util.List;

import mop.hi.oce.domain.network.CollegeInfo;
import mop.hi.oce.domain.network.ElementarySchoolInfo;
import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;
import mop.hi.oce.domain.network.RegionInfo;

public interface NetworkAdapter {
	public int getNetworkCountByStage(int userId, int stage);

	public List<Network> getNetworkList(int userId);

	public List<Network> getNetworkListByStage(int userId, int stage);

	public List<Network> getNetworkListByStatus(int userId, int status);

	public Network getNetwork(int userId, int networkId);

	public Network addNetwork(Network network);

	public void removeNetwork(int userId, int networkId);
	
	public void removeNetworkByStage(int userId, int stage);

	public void setNetwork(Network network);

	// ***********************************************************************

	public int getHighSchoolInfoCount(int userId);

	public List<HighSchoolInfo> getHighSchoolInfoList(int userId);

	public HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int schoolId);

	public HighSchoolInfo addHighSchoolInfo(HighSchoolInfo highSchool);

	public void removeAllHighSchoolInfo(int userId);

	public void setHighSchoolInfo(HighSchoolInfo highSchool);

	// ***********************************************************************

	public int getUniversityInfoCount(int userId);

	public List<UniversityInfo> getUniversityInfoList(int userId);

	public UniversityInfo getUniversityInfoByUnivId(int userId, int univId);

	public UniversityInfo addUniversityInfo(UniversityInfo university);

	public void removeAllUniversityInfo(int userId);
	
	public void removeUniversityInfoByUnivId(int userId, int univId);

	public void setUniversityInfo(UniversityInfo universityInfo);

	// ***********************************************************************

	public int getWorkplaceInfoCount(int userId);

	public List<WorkplaceInfo> getWorkplaceInfoList(int userId);

	public WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int workId);

	public WorkplaceInfo addWorkplaceInfo(WorkplaceInfo workplace);

	public void removeAllWorkplaceInfo(int userId);
	
	public void removeWorkplaceInfoByWorkId(int userId, int workId);

	public void setWorkplaceInfo(WorkplaceInfo workplace);

	// ***********************************************************************

	public void reloadCollegeInfoList(int userId);

	public CollegeInfo getCollegeInfo(int userId, int id);

	public CollegeInfo getCollegeInfoByCollegeId(int userId, int id);

	public List<CollegeInfo> getCollegeInfoList(int userId);

	public CollegeInfo addCollegeInfo(int userId, CollegeInfo collegeInfo);

	public void removeCollegeInfo(int userId, int id);

	public void removeAllCollegeInfo(int userId);

	public void setCollegeInfo(int userId, int id, CollegeInfo collegeInfo);

	public int getCollegeInfoCount(int userId);

	// ***********************************************************************

  public void removeAllElementarySchoolInfo(int userId);

  public void removeElementarySchoolInfo(int userId, int id);

  public void setElementarySchoolInfo( ElementarySchoolInfo elementaryInfo);

  public ElementarySchoolInfo getElementarySchoolInfo(int userId, int id);

  public ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id);

  public List<ElementarySchoolInfo> getElementarySchoolInfoList(int userId);

	public void reloadElementarySchoolInfo(int userId);

	public ElementarySchoolInfo getElementarySchoolInfo(int userId);

	public ElementarySchoolInfo addElementarySchoolInfo(int userId, ElementarySchoolInfo info);

	public void removeElementarySchoolInfo(int userId);

	// ***********************************************************************

  public void removeAllJuniorHighSchoolInfo(int userId);

  public void removeJuniorHighSchoolInfo(int userId, int id);

  public void setJuniorHighSchoolInfo( JuniorHighSchoolInfo juniorSchoolInfo);

  public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId, int id);

  public JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id);

  public List<JuniorHighSchoolInfo> getJuniorHighSchoolInfoList(int userId);


	public void reloadJuniorHighSchoolInfo(int userId);

	public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId);

	public JuniorHighSchoolInfo addJuniorHighSchoolInfo(int userId, JuniorHighSchoolInfo info);

	public void removeJuniorHighSchoolInfo(int userId);

	// ***********************************************************************

	// ***********************************************************************

	public void reloadRegionInfo(int userId);

	public RegionInfo getRegionInfo(int userId, int regionId);
	
	public List<RegionInfo> getRegionInfoList(int userId);
	
	public RegionInfo addRegionInfo(RegionInfo info);

	public void removeRegionInfo(int userId, int regionId);
	
	public void removeAllRegionInfo(int userId);
	
	public void setRegionInfo(RegionInfo info);

	// ***********************************************************************
	@Deprecated
	public mop.hi.oce.domain.network.NetworkBigObject getNetworkBigObject(
			int userId);

	public mop.hi.oce.domain.network.NetworkBigObjectN getNetworkBigObjectN(
			int userId);

  public mop.hi.oce.domain.network.NetworkBigData getNetworkBigData(int userId);

	public boolean isExistInNetwork(int userid, int networkid);

	public int getValidNetworkCountByStage(int userId, int stage);

	public List<Network> getNetworkListByStageAndStatus(int userid, int stage,
			int[] status);

	public List<Network> getNetworkListByStatus(int userid, int[] status);

	public void reloadNetworkList(int userId);

	public void reloadHighSchoolInfoList(int userId);

	public void reloadUniversityInfoList(int userId);

	public void reloadWorkplaceInfoList(int userId);

	public Network getRegionalNetwork(int userId);

}
