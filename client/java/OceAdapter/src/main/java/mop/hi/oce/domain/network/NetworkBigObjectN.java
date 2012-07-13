package mop.hi.oce.domain.network;

import java.util.ArrayList;
import java.util.List;

public class NetworkBigObjectN {

	protected List<mop.hi.oce.domain.network.HighSchoolInfo> highschools;

	protected List<mop.hi.oce.domain.network.UniversityInfo> univs;

	protected List<mop.hi.oce.domain.network.Network> networks;

	protected List<mop.hi.oce.domain.network.WorkplaceInfo> workspaces;

	protected List<mop.hi.oce.domain.network.CollegeInfo> collages;

	protected mop.hi.oce.domain.network.ElementarySchoolInfo elementaryschool;

	protected mop.hi.oce.domain.network.JuniorHighSchoolInfo juniorhighschool;

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

	public mop.hi.oce.domain.network.ElementarySchoolInfo getElementaryschool() {
		return elementaryschool;
	}

	public void setElementaryschool(
			mop.hi.oce.domain.network.ElementarySchoolInfo elementaryschool) {
		this.elementaryschool = elementaryschool;
	}

	public mop.hi.oce.domain.network.JuniorHighSchoolInfo getJuniorhighschool() {
		return juniorhighschool;
	}

	public void setJuniorhighschool(
			mop.hi.oce.domain.network.JuniorHighSchoolInfo juniorhighschool) {
		this.juniorhighschool = juniorhighschool;
	}

	public static NetworkBigObjectN valueOf(
			mop.hi.svc.model.NetworkBigObjectN bigObjectN) {
		NetworkBigObjectN result = new NetworkBigObjectN();
		{
			List<CollegeInfo> collages = new ArrayList<CollegeInfo>();
			for (mop.hi.svc.model.CollegeInfo info : bigObjectN.collages) {
				CollegeInfo collage = new CollegeInfo();
				collage.setInfo(info);
				collages.add(collage);
			}
			result.setCollages(collages);
		}
		{
			if(bigObjectN.elementary!=null){
				ElementarySchoolInfo elementarySchool = new ElementarySchoolInfo();
				elementarySchool.setInfo(bigObjectN.elementary);
				result.setElementaryschool(elementarySchool);
			}else{
				result.setElementaryschool(null);
			}
		}
		{
			List<HighSchoolInfo> highschools = new ArrayList<HighSchoolInfo>();
			for (mop.hi.svc.model.HighSchoolInfo info : bigObjectN.highschools) {
				HighSchoolInfo highschool = new HighSchoolInfo();
				highschool.set(info);
				highschools.add(highschool);
			}
			result.setHighschools(highschools);
		}
		{
			if(bigObjectN.juniorhighschool!=null){
				JuniorHighSchoolInfo juniorhighschool = new JuniorHighSchoolInfo();
				juniorhighschool.setInfo(bigObjectN.juniorhighschool);
				result.setJuniorhighschool(juniorhighschool);
			}else {
				result.setJuniorhighschool(null);
			}
		}
		{
			List<Network> networks = new ArrayList<Network>();
			for (mop.hi.svc.model.Network info : bigObjectN.networks) {
				Network network = new Network();
				network.set(info);
				networks.add(network);
			}
			result.setNetworks(networks);
		}
		{
			List<UniversityInfo> univs = new ArrayList<UniversityInfo>();
			for (mop.hi.svc.model.UniversityInfo info : bigObjectN.universitys) {
				UniversityInfo university = new UniversityInfo();
				university.set(info);
				univs.add(university);
			}
			result.setUnivs(univs);
		}
		{
			List<WorkplaceInfo> workspaces = new ArrayList<WorkplaceInfo>();
			for (mop.hi.svc.model.WorkspaceInfo info : bigObjectN.workspaces) {
				WorkplaceInfo workplace = new WorkplaceInfo();
				workplace.set(info);
				workspaces.add(workplace);
			}
			result.setWorkspaces(workspaces);
		}
		return result;
	}

}
