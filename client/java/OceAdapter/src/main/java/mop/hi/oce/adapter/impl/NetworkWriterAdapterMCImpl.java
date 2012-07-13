package mop.hi.oce.adapter.impl;

import mop.hi.svc.model.NetworkWriterPrx;
import mop.hi.svc.model.NetworkWriterPrxHelper;

import mop.hi.oce.domain.network.CollegeInfo;
import mop.hi.oce.domain.network.ElementarySchoolInfo;
import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.RegionInfo;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;

import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.clusterstate.ReplicatedClusterMCAdapter;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.Oce;

public class NetworkWriterAdapterMCImpl extends ReplicatedClusterMCAdapter {

	private static final String ctr_endpoints = "ControllerUserNetworkWriter";
	private static int _interval = 120;

	public NetworkWriterAdapterMCImpl() {
		super(ChannelFactory.getChannel(ChannelType.OCE), ctr_endpoints, _interval);
	}

	protected NetworkWriterPrx getUserNetworkWriter(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			return NetworkWriterPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
		} else {
			return null;
		}
	}

	protected NetworkWriterPrx getUserNetworkWriterOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			return NetworkWriterPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
		} else {
			return null;
		}
	}

	// ***********************************************************************

	public void reloadNetworkList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadNetworkList userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadNetworkSeq(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadNetworkList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public Network addNetwork(Network network) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addNetwork");
		NetworkWriterPrx prx = null;
		try {
			if (network.getUserId() <= 0) {
				network.clearUpdates();
				return network;
			}
			prx = getUserNetworkWriter(network.getUserId());
			int id = prx.addNetwork(network.getUserId(), network.getUpdates());
			network.setId(id);
			network.clearUpdates();

			return network;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addNetwork [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeNetwork(int userId, int networkId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeNetwork userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(userId);
			prx.removeNetwork(userId, networkId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeNetwork [ " + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeNetworkByStage(int userId, int stage) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeNetworkByStage userid: " + userId + " stage: " + stage);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeNetworkByStage(userId, stage);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeNetworkByStage [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setNetwork(Network network) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setNetwork");
		NetworkWriterPrx prx = null;
		try {
			if (network.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriter(network.getUserId());
			prx.setNetwork(network.getUserId(), network.getNetworkId(), network.getUpdates());
			network.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setNetwork [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadHighSchoolInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadHighSchoolInfoList userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadHighSchoolInfoSeq(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadHighSchoolInfoList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public HighSchoolInfo addHighSchoolInfo(HighSchoolInfo highSchool) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addHighSchoolInfo");
		NetworkWriterPrx prx = null;
		try {
			if (highSchool.getUserId() <= 0) {
				highSchool.clearUpdates();
				return highSchool;
			}
			prx = getUserNetworkWriter(highSchool.getUserId());
			int id = prx.addHighSchoolInfo(highSchool.getUserId(), highSchool.getUpdates());
			highSchool.setId(id);
			highSchool.clearUpdates();

			return highSchool;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addHighSchoolInfo [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllHighSchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllHighSchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllHighSchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllHighSchoolInfo [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setHighSchoolInfo(HighSchoolInfo highSchool) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setHighSchoolInfo");
		NetworkWriterPrx prx = null;
		try {
			if (highSchool.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(highSchool.getUserId());
			prx.setHighSchoolInfo(highSchool.getUserId(), highSchool.getId(), highSchool.getUpdates());
			highSchool.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setHighSchoolInfo [ " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadUniversityInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadUniversityInfoList userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadUniversityInfoSeq(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadUniversityInfoList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public UniversityInfo addUniversityInfo(UniversityInfo university) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addUniversityInfo");
		NetworkWriterPrx prx = null;
		try {
			if (university.getUserId() <= 0) {
				university.clearUpdates();
				return university;
			}
			prx = getUserNetworkWriter(university.getUserId());
			int id = prx.addUniversityInfo(university.getUserId(), university.getUpdates());
			university.setId(id);
			university.clearUpdates();

			return university;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addUniversityInfo [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllUniversityInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllUniversityInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllUniversityInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllUniversityInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeUniversityInfoByUnivId(int userId, int univId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeUniversityInfoByUnivId userid: " + userId + " univid: " + univId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeUniversityInfoByUnivId(userId, univId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeUniversityInfoByUnivId [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUniversityInfo(UniversityInfo universityInfo) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setUniversityInfo");
		NetworkWriterPrx prx = null;
		try {
			if (universityInfo.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(universityInfo.getUserId());
			prx.setUniversityInfo(universityInfo.getUserId(), universityInfo.getId(), universityInfo.getUpdates());
			universityInfo.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setUniversityInfo [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadWorkplaceInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadWorkplaceInfoList userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadWorkplaceInfoSeq(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadWorkplaceInfoList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public WorkplaceInfo addWorkplaceInfo(WorkplaceInfo workplace) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addWorkplaceInfo");
		NetworkWriterPrx prx = null;
		try {
			if (workplace.getUserId() <= 0) {
				workplace.clearUpdates();
				return workplace;
			}
			prx = getUserNetworkWriter(workplace.getUserId());
			int id = prx.addWorkspaceInfo(workplace.getUserId(), workplace.getUpdates());
			workplace.setId(id);
			workplace.clearUpdates();

			return workplace;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addWorkplaceInfo [" + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllWorkplaceInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllWorkplaceInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllWorkplaceInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllWorkplaceInfo [ " + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeWorkplaceInfoByWorkId(int userId, int workId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.getHighSchoolInfoBySchoolId userid: " + userId + " workid: " + workId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeWorkplaceInfoByWorkId(userId, workId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeWorkplaceInfoByWorkId [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setWorkplaceInfo(WorkplaceInfo workplace) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setWorkplaceInfo");
		NetworkWriterPrx prx = null;
		try {
			if (workplace.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(workplace.getUserId());
			prx.setWorkspaceInfo(workplace.getUserId(), workplace.getId(), workplace.getUpdates());
			workplace.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setWorkplaceInfo [ " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadCollegeInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadCollegeInfoList userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadCollegeInfoSeq(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadCollegeInfoList [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public CollegeInfo addCollegeInfo(int userId, CollegeInfo collegeInfo) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addCollegeInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				collegeInfo.clearUpdates();
				return collegeInfo;
			}
			prx = getUserNetworkWriter(userId);
			int id = prx.addCollegeInfo(userId, collegeInfo.getUpdates());
			collegeInfo.setId(id);
			collegeInfo.clearUpdates();
			return collegeInfo;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addCollegeInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllCollegeInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllCollegeInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllCollegeInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllCollegeInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeCollegeInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeCollegeInfo userid: " + userId + " id: " + id);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeCollegeInfo(userId, id);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeCollegeInfo [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setCollegeInfo(int userId, int id, CollegeInfo collegeInfo) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setCollegeInfo userid: " + userId + " id: " + id);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.setCollegeInfo(userId, id, collegeInfo.getUpdates());
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setCollegeInfo [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadElementarySchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadElementarySchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadElementarySchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadElementarySchoolInfo [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public ElementarySchoolInfo addElementarySchoolInfo(int userId, ElementarySchoolInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addElementarySchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return info;
			}
			prx = getUserNetworkWriter(userId);
			int id = prx.addElementarySchoolInfo(userId, info.getUpdates());
			info.setId(id);
			info.clearUpdates();

			return info;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addElementarySchoolInfo [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllElementarySchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllEmenetarySchoolInfo userId: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllElementarySchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllElementarySchoolInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeElementarySchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeElementarySchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeElementarySchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeElementarySchoolInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeElementarySchoolInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeElementarySchoolInfo userid: " + userId + " id: " + id);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeElementarySchoolInfoById(userId, id);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeElementarySchoolInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setElementarySchoolInfo(ElementarySchoolInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setElementarySchoolInfo");
		NetworkWriterPrx prx = null;
		try {
			if (info.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(info.getUserId());
			prx.setElementarySchoolInfo(info.getUserId(), info.getId(), info.getUpdates());
			info.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setElementarySchoolInfo [ " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadJuniorHighSchoolInfo(int userId) {

		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.reloadJuniorHighSchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadJuniorHighSchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadJuniorHighSchoolInfo [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public JuniorHighSchoolInfo addJuniorHighSchoolInfo(int userId, JuniorHighSchoolInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addJuniorHighSchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return info;
			}
			prx = getUserNetworkWriter(userId);
			int id = prx.addJuniorHighSchoolInfo(userId, info.getUpdates());
			info.setId(id);
			info.clearUpdates();

			return info;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addJuniorHighSchoolInfo [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllJuniorHighSchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllEmenetarySchoolInfo userId: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllJuniorHighSchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllJuniorHighSchoolInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeJuniorHighSchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeJuniorHighSchoolInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeJuniorHighSchoolInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeJuniorHighSchoolInfo [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeJuniorHighSchoolInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeJuniorHighSchoolInfo userid: " + userId + " id: " + id);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeJuniorHighSchoolInfoById(userId, id);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeJuniorHighSchoolInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setJuniorHighSchoolInfo(JuniorHighSchoolInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setJuniorHighSchoolInfo");
		NetworkWriterPrx prx = null;
		try {
			if (info.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(info.getUserId());
			prx.setJuniorHighSchoolInfo(info.getUserId(), info.getId(), info.getUpdates());
			info.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setJuniorHighSchoolInfo [ " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public void reloadRegionInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.getHighSchoolInfoBySchoolId userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.reloadRegionInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".reloadRegionInfo [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public RegionInfo addRegionInfo(RegionInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.addRegionInfo");
		NetworkWriterPrx prx = null;
		try {
			if (info.getUserId() <= 0) {
				return info;
			}
			prx = getUserNetworkWriter(info.getUserId());
			int id = prx.addRegionInfo(info.getUserId(), info.getUpdates());
			info.setId(id);
			info.clearUpdates();
			return info;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addRegionInfo " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeAllRegionInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeAllRegionInfo userid: " + userId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeAllRegionInfo(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeAllRegionInfo [ " + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void removeRegionInfo(int userId, int regionId) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.removeRegionInfo userid: " + userId + " regionid: " + regionId);
		NetworkWriterPrx prx = null;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserNetworkWriter(userId);
			prx.removeRegionInfo(userId, regionId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".removeRegionInfo [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setRegionInfo(RegionInfo info) {
		TimeCost cost = TimeCost.begin("NetworkWriterAdapterMCImpl.setRegionInfo");
		NetworkWriterPrx prx = null;
		try {
			if (info.getUserId() <= 0) {
				return;
			}
			prx = getUserNetworkWriterOneway(info.getUserId());
			prx.setRegionInfo(info.getUserId(), info.getRegionId(), info.getUpdates());
			info.clearUpdates();
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setRegionInfo [ " + prx + "]" + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

}
