package mop.hi.oce.domain.network;

import java.util.ArrayList;
import java.util.List;

public class NetworkBigData {

	protected List<mop.hi.oce.domain.network.HighSchoolInfo> highschools;

	protected List<mop.hi.oce.domain.network.UniversityInfo> univs;

	protected List<mop.hi.oce.domain.network.Network> networks;

	protected List<mop.hi.oce.domain.network.WorkplaceInfo> workspaces;

	protected List<mop.hi.oce.domain.network.CollegeInfo> collages;

	protected List<mop.hi.oce.domain.network.ElementarySchoolInfo> elementaryschools;

	protected List<mop.hi.oce.domain.network.JuniorHighSchoolInfo> juniorhighschools;

	protected List<mop.hi.oce.domain.network.RegionInfo> regions;

	public List<mop.hi.oce.domain.network.HighSchoolInfo> getHighschools() {
		return highschools;
	}

	public List<mop.hi.oce.domain.network.Network> getNetworks() {
		return networks;
	}

	public List<mop.hi.oce.domain.network.UniversityInfo> getUnivs() {
		return univs;
	}

	public List<mop.hi.oce.domain.network.WorkplaceInfo> getWorkspaces() {
		return workspaces;
	}

  public List<mop.hi.oce.domain.network.RegionInfo> getRegions() {
    return regions;
  }

	public void setHighschools(
			List<mop.hi.oce.domain.network.HighSchoolInfo> highschools) {
		this.highschools = highschools;
	}

	public void setNetworks(List<mop.hi.oce.domain.network.Network> networks) {
		this.networks = networks;
	}

	public void setUnivs(List<mop.hi.oce.domain.network.UniversityInfo> univs) {
		this.univs = univs;
	}

  public void setRegions(List<mop.hi.oce.domain.network.RegionInfo> regions) {
    this.regions = regions;
  }

	public void setWorkspaces(
			List<mop.hi.oce.domain.network.WorkplaceInfo> workspaces) {
		this.workspaces = workspaces;
	}

	public List<mop.hi.oce.domain.network.CollegeInfo> getCollages() {
		return collages;
	}

	public void setCollages(List<mop.hi.oce.domain.network.CollegeInfo> collages) {
		this.collages = collages;
	}

	public List<mop.hi.oce.domain.network.ElementarySchoolInfo> getElementaryschools() {
		return elementaryschools;
	}

	public void setElementaryschools(List<mop.hi.oce.domain.network.ElementarySchoolInfo> elementaryschools) {
		this.elementaryschools = elementaryschools;
	}

	public List<mop.hi.oce.domain.network.JuniorHighSchoolInfo> getJuniorhighschools() {
		return juniorhighschools;
	}

	public void setJuniorhighschools(List<mop.hi.oce.domain.network.JuniorHighSchoolInfo> juniorhighschools) {
		this.juniorhighschools = juniorhighschools;
	}

	public static NetworkBigData valueOf(
			mop.hi.svc.model.NetworkData bigData) {
		NetworkBigData result = new NetworkBigData();
		{
			List<CollegeInfo> collages = new ArrayList<CollegeInfo>();
			for (mop.hi.svc.model.CollegeInfo info : bigData.colleges) {
				CollegeInfo collage = new CollegeInfo();
				collage.setInfo(info);
				collages.add(collage);
			}
			result.setCollages(collages);
		}
		{
      List<ElementarySchoolInfo> schools = new ArrayList<ElementarySchoolInfo>();
      for (mop.hi.svc.model.ElementarySchoolInfo info : bigData.elementaries) {
        ElementarySchoolInfo school = new ElementarySchoolInfo();
        school.setInfo(info);
        schools.add(school);
      }
      result.setElementaryschools(schools);
		}
		{
			List<HighSchoolInfo> highschools = new ArrayList<HighSchoolInfo>();
			for (mop.hi.svc.model.HighSchoolInfo info : bigData.highschools) {
				HighSchoolInfo highschool = new HighSchoolInfo();
				highschool.set(info);
				highschools.add(highschool);
			}
			result.setHighschools(highschools);
		}
		{
  		List<JuniorHighSchoolInfo> highschools = new ArrayList<JuniorHighSchoolInfo>();
			for (mop.hi.svc.model.JuniorHighSchoolInfo info : bigData.juniorhighschools) {
				JuniorHighSchoolInfo highschool = new JuniorHighSchoolInfo();
				highschool.setInfo(info);
				highschools.add(highschool);
			}
			result.setJuniorhighschools(highschools);
		}
		{
			List<Network> networks = new ArrayList<Network>();
			for (mop.hi.svc.model.Network info : bigData.networks) {
				Network network = new Network();
				network.set(info);
				networks.add(network);
			}
			result.setNetworks(networks);
		}
		{
			List<UniversityInfo> univs = new ArrayList<UniversityInfo>();
			for (mop.hi.svc.model.UniversityInfo info : bigData.universities) {
				UniversityInfo university = new UniversityInfo();
				university.set(info);
				univs.add(university);
			}
			result.setUnivs(univs);
		}
		{
			List<WorkplaceInfo> workspaces = new ArrayList<WorkplaceInfo>();
			for (mop.hi.svc.model.WorkspaceInfo info : bigData.workspaces) {
				WorkplaceInfo workplace = new WorkplaceInfo();
				workplace.set(info);
				workspaces.add(workplace);
			}
			result.setWorkspaces(workspaces);
		}
    {
      List<RegionInfo> regions = new ArrayList<RegionInfo>();
      for (mop.hi.svc.model.RegionInfo info : bigData.regions) {
        RegionInfo region = new RegionInfo();
        region.set(info);
        regions.add(region);
      }
      result.setRegions(regions);
    }
		return result;
	}

}
