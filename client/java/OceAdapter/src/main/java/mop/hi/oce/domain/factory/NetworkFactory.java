package mop.hi.oce.domain.factory;

import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;

public interface NetworkFactory {
	public Network createNetwork();

	public HighSchoolInfo createHighSchoolInfo();

	public UniversityInfo createUniversityInfo();

	public WorkplaceInfo createWorkplaceInfo();
}
