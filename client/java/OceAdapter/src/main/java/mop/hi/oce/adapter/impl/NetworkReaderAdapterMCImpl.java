package mop.hi.oce.adapter.impl;

import mop.hi.svc.model.NetworkReaderPrx;
import mop.hi.svc.model.NetworkReaderPrxHelper;

import mop.hi.oce.domain.network.CollegeInfo;
import mop.hi.oce.domain.network.ElementarySchoolInfo;
import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.domain.network.Network;
import mop.hi.oce.domain.network.RegionInfo;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;
import mop.hi.oce.domain.network.NetworkBigObject;
import mop.hi.oce.domain.network.NetworkBigObjectN;
import mop.hi.oce.domain.network.NetworkBigData;

import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.clusterstate.ReplicatedClusterMCAdapter;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.Oce;

import java.util.ArrayList;
import java.util.List;

public class NetworkReaderAdapterMCImpl extends ReplicatedClusterMCAdapter {

	private static final String ctr_endpoints = "ControllerUserNetworkReader";
	private static int _interval = 120;

	public NetworkReaderAdapterMCImpl() {
		super(ChannelFactory.getChannel(ChannelType.OCE), ctr_endpoints, _interval);
	}

	protected NetworkReaderPrx getUserNetworkReader(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			return NetworkReaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
		} else {
			return null;
		}
	}

	protected NetworkReaderPrx getUserNetworkReaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			return NetworkReaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
		} else {
			return null;
		}
	}

	// ***********************************************************************

	public int getNetworkCountByStage(int userId, int stage) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkCountByStage userId: " + userId + " stage: " + stage);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			int r = prx.getNetworkCountByStage(userId, stage);
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkCountByStage [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public int getValidNetworkCountByStage(int userId, int stage) {

		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getValidNetworkCountByStage userId: " + userId + " stage: " + stage);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			int r = prx.getValidNetworkCountByStage(userId, stage);
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getValidNetworkCountByStage [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}

	}

	public List<Network> getNetworkList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<Network>();
			}
			List<Network> r;
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.Network[] nw = prx.getNetworkSeq(userId);
			ArrayList<Network> networks = new ArrayList<Network>();
			for (int i = 0; i < nw.length; ++i) {
				Network n = new Network();
				n.set(nw[i]);
				networks.add(n);
			}

			r = networks;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkList [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<Network> getNetworkListByStage(int userId, int stage) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkListByStage userId: " + userId + " stage: " + stage);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<Network>();
			}
			List<Network> r;
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.Network[] nw = prx.getNetworkSeqByStage(userId, stage);
			ArrayList<Network> networks = new ArrayList<Network>();
			for (int i = 0; i < nw.length; ++i) {
				Network n = new Network();
				n.set(nw[i]);
				networks.add(n);
			}

			r = networks;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkListByStage [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<Network> getNetworkListByStatus(int userId, int status) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkListByStatus userId: " + userId + " status: " + status);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<Network>();
			}
			List<Network> r;
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.Network[] nw = prx.getNetworkSeqByStatus(userId, status);
			ArrayList<Network> networks = new ArrayList<Network>();
			for (int i = 0; i < nw.length; ++i) {
				Network n = new Network();
				n.set(nw[i]);
				networks.add(n);
			}
			r = networks;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkListByStatus [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<Network> getNetworkListByStageAndStatus(int userId, int stage, int[] status) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkListByStageAndStatus userId: " + userId + " stage: " + stage);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<Network>();
			}
			prx = getUserNetworkReader(userId);
			List<Network> networks = new ArrayList<Network>();
			mop.hi.svc.model.Network[] obj = prx.getNetworkSeqByStageAndStatus(userId, stage, status);
			for (int i = 0; i < obj.length; i++) {
				Network nw = new Network();
				nw.set(obj[i]);
				networks.add(nw);
			}
			return networks;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkListByStageAndStatus [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<Network> getNetworkListByStatus(int userId, int[] status) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkListByStatus userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<Network>();
			}
			prx = getUserNetworkReader(userId);
			List<Network> networks = new ArrayList<Network>();
			mop.hi.svc.model.Network[] obj = prx.getNetworkSeqByStatuses(userId, status);
			for (int i = 0; i < obj.length; i++) {
				Network nw = new Network();
				nw.set(obj[i]);
				networks.add(nw);
			}
			return networks;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkListByStatus [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public Network getNetwork(int userId, int networkId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetwork userId: " + userId + " networkId: " + networkId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new Network();
			}
			mop.hi.svc.model.Network nw = null;
			try {
				prx = getUserNetworkReader(userId);
				nw = prx.getNetworkByNetId(userId, networkId);
			} catch (Exception e) {
				return null;
			}
			if (nw == null)
				return null;

			Network res = new Network();
			res.set(nw);
			return res;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetwork [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public Network getRegionalNetwork(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getRegionalNetwork userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new Network();
			}
			mop.hi.svc.model.Network nw = null;
			try {
				prx = getUserNetworkReader(userId);
				nw = prx.getRegionalNetwork(userId);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getRegionalNetwork [ " + prx + "] id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				return null;
			}
			if (nw == null)
				return null;
			Network res = new Network();
			res.set(nw);
			return res;
		} finally {
			cost.endFinally();
		}
	}

	public boolean isExistInNetwork(int userId, int networkId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.isExistInNetwork userId: " + userId + " networkId: " + networkId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return false;
			}
			prx = getUserNetworkReader(userId);
			boolean r = prx.isExistInNetwork(userId, networkId);
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".isExistInNetwork [" + prx + " ]userId=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public int getHighSchoolInfoCount(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getHighSchoolInfoCount userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			return prx.getHighSchoolInfoCount(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getHighSchoolInfoCount [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<HighSchoolInfo> getHighSchoolInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getHighSchoolInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<HighSchoolInfo>();
			}
			List<HighSchoolInfo> r;
			ArrayList<HighSchoolInfo> res = new ArrayList<HighSchoolInfo>();
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.HighSchoolInfo[] seq = prx.getHighSchoolInfoSeq(userId);
			for (int i = 0; i < seq.length; ++i) {
				HighSchoolInfo info = new HighSchoolInfo();
				info.set(seq[i]);
				res.add(info);
			}
			r = res;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getHighSchoolInfoList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int schoolId) {

		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getHighSchoolInfoBySchoolId userId: " + userId + " schoolid: " + schoolId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new HighSchoolInfo();
			}
			mop.hi.svc.model.HighSchoolInfo school = null;
			try {
				prx = getUserNetworkReader(userId);
				school = prx.getHighSchoolInfoBySchoolId(userId, schoolId);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getHighSchoolInfoBySchoolId [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getHighSchoolInfoBySchoolId " + " id=" + userId + " " + e);
				return null;
			}
			if (school == null)
				return null;

			HighSchoolInfo info = new HighSchoolInfo();
			info.set(school);
			return info;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public int getUniversityInfoCount(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getUniversityInfoCount userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			int r = prx.getUniversityInfoCount(userId);
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getUniversityInfoCount [ " + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<UniversityInfo> getUniversityInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getUniversityInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<UniversityInfo>();
			}
			List<UniversityInfo> r;
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.UniversityInfo[] seq = prx.getUniversityInfoSeq(userId);

			ArrayList<UniversityInfo> res = new ArrayList<UniversityInfo>();
			for (int i = 0; i < seq.length; ++i) {
				UniversityInfo ui = new UniversityInfo();
				ui.set(seq[i]);
				res.add(ui);
			}
			r = res;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getUniversityInfoList [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public UniversityInfo getUniversityInfoByUnivId(int userId, int univId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getUniversityInfoByUnivId userId: " + userId + " univid: " + univId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new UniversityInfo();
			}
			mop.hi.svc.model.UniversityInfo univ = null;
			try {
				prx = getUserNetworkReader(userId);
				univ = prx.getUniversityInfoByUnivId(userId, univId);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getUniversityInfoByUnivId [ " + prx + "]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				return null;
			}
			if (univ == null)
				return null;
			UniversityInfo info = new UniversityInfo();
			info.set(univ);
			return info;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public int getWorkplaceInfoCount(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getWorkplaceInfoCount userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			return prx.getWorkplaceInfoCount(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getWorkplaceInfoCount [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<WorkplaceInfo> getWorkplaceInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getWorkplaceInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<WorkplaceInfo>();
			}
			List<WorkplaceInfo> r;
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.WorkspaceInfo[] infos = prx.getWorkspaceInfoSeq(userId);
			ArrayList<WorkplaceInfo> res = new ArrayList<WorkplaceInfo>();

			for (int i = 0; i < infos.length; ++i) {
				WorkplaceInfo wi = new WorkplaceInfo();
				wi.set(infos[i]);
				res.add(wi);
			}
			r = res;
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getWorkplaceInfoList [ " + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int workId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getWorkplaceInfoByWorkId userId: " + userId + " workId: " + workId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new WorkplaceInfo();
			}
			mop.hi.svc.model.WorkspaceInfo wk = null;

			try {
				prx = getUserNetworkReader(userId);
				wk = prx.getWorkspaceInfoByWorkId(userId, workId);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getWorkplaceInfoByWorkId [ " + prx + "] id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				return null;
			}
			if (wk == null)
				return null;
			WorkplaceInfo info = new WorkplaceInfo();
			info.set(wk);
			return info;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public int getCollegeInfoCount(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getCollegeInfoCount userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			prx = getUserNetworkReader(userId);
			return prx.getCollegeInfoCount(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfoCount [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public List<CollegeInfo> getCollegeInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getCollegeInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<CollegeInfo>();
			}
			prx = getUserNetworkReader(userId);
			List<CollegeInfo> r = new ArrayList<CollegeInfo>();
			mop.hi.svc.model.CollegeInfo[] seq = prx.getCollegeInfoSeq(userId);
			for (int i = 0; i < seq.length; ++i) {
				CollegeInfo ui = new CollegeInfo();
				ui.setInfo(seq[i]);
				r.add(ui);
			}
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfoList [" + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public CollegeInfo getCollegeInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getCollegeInfo userId: " + userId + " id: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new CollegeInfo();
			}
			CollegeInfo result = null;
			mop.hi.svc.model.CollegeInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getCollegeInfo(userId, id);
				result = new CollegeInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfo [ " + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public CollegeInfo getCollegeInfoByCollegeId(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getCollegeInfoByCollegeId userId: " + userId + " collegeid: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new CollegeInfo();
			}
			CollegeInfo result = null;
			mop.hi.svc.model.CollegeInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getCollegeInfoByCollegeId(userId, id);
				result = new CollegeInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfoByCollegeId [ " + prx + "] id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getCollegeInfoByCollegeId " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public List<ElementarySchoolInfo> getElementarySchoolInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getElementarySchoolInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<ElementarySchoolInfo>();
			}
			List<ElementarySchoolInfo> r = new ArrayList<ElementarySchoolInfo>();
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.ElementarySchoolInfo[] seq = prx.getElementarySchoolInfoSeq(userId);
			for (int i = 0; i < seq.length; ++i) {
				ElementarySchoolInfo ui = new ElementarySchoolInfo();
				ui.setInfo(seq[i]);
				r.add(ui);
			}
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfoList [ " + prx + " ] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public ElementarySchoolInfo getElementarySchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getElementarySchoolInfo userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ElementarySchoolInfo();
			}
			ElementarySchoolInfo result = null;
			try {
				prx = getUserNetworkReader(userId);
				mop.hi.svc.model.ElementarySchoolInfo info = prx.getElementarySchoolInfo(userId);
				if (info == null) {
					return null;
				}
				result = new ElementarySchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfo [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public ElementarySchoolInfo getElementarySchoolInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getElementarySchoolInfo userId: " + userId + " id: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ElementarySchoolInfo();
			}
			ElementarySchoolInfo result = null;
			mop.hi.svc.model.ElementarySchoolInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getElementarySchoolInfoById(userId, id);
				result = new ElementarySchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfo [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getElementarySchoolInfoBySchoolId userId: " + userId + " schoolid: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ElementarySchoolInfo();
			}
			ElementarySchoolInfo result = null;
			mop.hi.svc.model.ElementarySchoolInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getElementarySchoolInfoBySchoolId(userId, id);
				result = new ElementarySchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfoBySchoolId [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getElementarySchoolInfoBySchoolId " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public List<JuniorHighSchoolInfo> getJuniorHighSchoolInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getJuniorHighSchoolInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<JuniorHighSchoolInfo>();
			}
			List<JuniorHighSchoolInfo> r = new ArrayList<JuniorHighSchoolInfo>();
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.JuniorHighSchoolInfo[] seq = prx.getJuniorHighSchoolInfoSeq(userId);
			for (int i = 0; i < seq.length; ++i) {
				JuniorHighSchoolInfo ui = new JuniorHighSchoolInfo();
				ui.setInfo(seq[i]);
				r.add(ui);
			}
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfoList [ " + prx + " ] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getJuniorHighSchoolInfo userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new JuniorHighSchoolInfo();
			}
			JuniorHighSchoolInfo result = null;
			try {
				prx = getUserNetworkReader(userId);
				mop.hi.svc.model.JuniorHighSchoolInfo info = prx.getJuniorHighSchoolInfo(userId);
				if (info == null) {
					return null;
				}
				result = new JuniorHighSchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfo [" + prx + "] id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getJuniorHighSchoolInfo userId: " + userId + " id: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new JuniorHighSchoolInfo();
			}
			JuniorHighSchoolInfo result = null;
			mop.hi.svc.model.JuniorHighSchoolInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getJuniorHighSchoolInfoById(userId, id);
				result = new JuniorHighSchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfo [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	public JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getJuniorHighSchoolInfoBySchoolId userId: " + userId + " schoolid: " + id);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new JuniorHighSchoolInfo();
			}
			JuniorHighSchoolInfo result = null;
			mop.hi.svc.model.JuniorHighSchoolInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getJuniorHighSchoolInfoBySchoolId(userId, id);
				result = new JuniorHighSchoolInfo();
				result.setInfo(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfoBySchoolId [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getJuniorHighSchoolInfoBySchoolId " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	public List<RegionInfo> getRegionInfoList(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getRegionInfoList userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new ArrayList<RegionInfo>();
			}
			List<RegionInfo> r = new ArrayList<RegionInfo>();
			prx = getUserNetworkReader(userId);
			mop.hi.svc.model.RegionInfo[] seq = prx.getRegionInfoSeq(userId);
			for (int i = 0; i < seq.length; ++i) {
				RegionInfo ui = new RegionInfo();
				ui.set(seq[i]);
				r.add(ui);
			}
			return r;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getRegionInfoList [ " + prx + " ]id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public RegionInfo getRegionInfo(int userId, int regionId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getRegionInfo userId: " + userId + " regionid: " + regionId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new RegionInfo();
			}
			RegionInfo result = null;
			mop.hi.svc.model.RegionInfo info;
			try {
				prx = getUserNetworkReader(userId);
				info = prx.getRegionInfo(userId, regionId);
				result = new RegionInfo();
				result.set(info);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getRegionInfo [" + prx + " ]id=" + userId + " " + e);
				return null;
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getRegionInfo " + " id=" + userId + " " + e);
				return null;
			}
			return result;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************

	@Deprecated
	public mop.hi.oce.domain.network.NetworkBigObject getNetworkBigObject(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkBigObject userId: " + userId);
		try {
			if (userId <= 0) {
				return new mop.hi.oce.domain.network.NetworkBigObject();
			}
			mop.hi.oce.domain.network.NetworkBigObject res = new NetworkBigObject();
			res.setHighschools(this.getHighSchoolInfoList(userId));
			res.setNetworks(this.getNetworkList(userId));
			res.setUnivs(this.getUniversityInfoList(userId));
			res.setWorkspaces(this.getWorkplaceInfoList(userId));
			return res;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkBigObject " + " id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public NetworkBigObjectN getNetworkBigObjectN(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkBigObjectN userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new NetworkBigObjectN();
			}
			prx = getUserNetworkReader(userId);
			NetworkBigObjectN big = NetworkBigObjectN.valueOf(prx.getBigObjectN(userId));
			return big;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkBigObjectN [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public mop.hi.oce.domain.network.NetworkBigData getNetworkBigData(int userId) {
		TimeCost cost = TimeCost.begin("NetworkReaderAdapterMCImpl.getNetworkBigData userId: " + userId);
		NetworkReaderPrx prx = null;
		try {
			if (userId <= 0) {
				return new NetworkBigData();
			}
			prx = getUserNetworkReader(userId);
			NetworkBigData big = NetworkBigData.valueOf(prx.getNetworkFullData(userId));
			return big;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNetworkBigData [" + prx + "] id=" + userId + " " + e);
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	// ***********************************************************************
	
	public static void main(String[] args) {
		
		int userId = 35178;		
		NetworkReaderAdapterMCImpl networkReaderAdapter = new NetworkReaderAdapterMCImpl();
		NetworkBigData nbd = networkReaderAdapter.getNetworkBigData(userId);

		System.out.println("\ngetNetworks()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getNetworks());		
		System.out.println("\ngetCollages()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getCollages());
		System.out.println("\ngetElementaryschool()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getElementaryschools());
		System.out.println("\ngetHighschools()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getHighschools());
		System.out.println("\ngetJuniorhighschool()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getJuniorhighschools());
		System.out.println("\ngetNetworks()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getNetworks());
		System.out.println("\ngetUnivs()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getUnivs());
		System.out.println("\ngetWorkspaces()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getWorkspaces());
		System.out.println("\ngetRegions()");
		System.out.println("--------------------------------");
		System.out.println(nbd.getRegions());
		
		System.exit(0);
	}

}
