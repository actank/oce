package mop.hi.oce.domain.network;

import java.util.List;

public class NetworkBigObject {

	protected List<mop.hi.oce.domain.network.HighSchoolInfo> highschools;

	protected List<mop.hi.oce.domain.network.UniversityInfo> univs;

	protected List<mop.hi.oce.domain.network.Network> networks;

	protected List<mop.hi.oce.domain.network.WorkplaceInfo> workspaces;

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

}
