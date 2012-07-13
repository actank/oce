package com.xiaonei.xce.usercache;

import java.util.List;
import java.util.Map;

import xce.distcache.usercache.UserCache.UserCacheData;

import xce.userbase.CStateCHARITYINVITED;
import xce.userbase.CStateAPPLYSPAMMER; 
import xce.userbase.CStateVEST;
import xce.userbase.CStateINWHITELIST;
import xce.userbase.CStateCHARITY;
import xce.userbase.CStateEVERVIPMEMBER;
import xce.userbase.CStateGUIDEDONE;
import xce.userbase.CStateKEEPUSE;
import xce.userbase.CStateGAMEMANAGER;
import xce.userbase.CStateMSNIMME;
import xce.userbase.CStateSELECTED;
import xce.userbase.CStateSOCIALGRAPHGOOD;
import xce.userbase.CStateVIPMEMBER;
import xce.userbase.CStateEXPERIMENT;
import xce.userbase.CStateACTIVE;
import xce.userbase.CStateZOMBIE;
import xce.userbase.CStateGROUPMEMBER;
import xce.userbase.CStateCHANNELSPOKER;
import xce.usercache.CUserCacheGENDER;
import xce.usercache.CUserCacheID;
import xce.usercache.CUserCacheLEVEL;
import xce.usercache.CUserCacheNAME;
import xce.usercache.CUserCacheONLINE;
import xce.usercache.CUserCacheSTAR;
import xce.usercache.CUserCacheSTATE;
import xce.usercache.CUserCacheSTATUS;
import xce.usercache.CUserCacheTINYURL;
import xce.usercache.CUserCacheUNIV;
import xce.usercache.UserCacheData3;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.vipiconinfo.VipIconInfo;

public abstract class UserCache {
	public static final int OFFLINE = xce.mediator.onlineevent.OFFLINE.value;
	public static final int ONLINE_WEBPAGGER = xce.mediator.onlineevent.ONLINEWEBPAGGER.value - 1;
	public static final int ONLINE_XNT = xce.mediator.onlineevent.ONLINEXNT.value - 1;
	public static final int ONLINE_WAP = xce.mediator.onlineevent.ONLINEWAP.value - 1;
	
	private int id;
	private int status;

	private String name;
	private int univ;
	private String tinyUrl;
	private String gender;

	private int state;
	private int star;
	private int level;
	private int online;
	
	private List<VipIconInfo> vipIconInfoList;
	
	public List<VipIconInfo> getVipIconInfoList() {
		return vipIconInfoList;
	}
	
	public void setVipIconInfoList(List<VipIconInfo> vipIconInfoList) {
		this.vipIconInfoList = vipIconInfoList;
	}

	public int getId() {
		return id;
	}
	
	public String getGender() {
		return gender;
	}

	public int getStatus() {
		return status;
	}

	public String getName() {
		return name;
	}

	public int getUniv() {
		return univ;
	}

	public String getTinyUrl() {
		return tinyUrl;
	}

	public int getState() {
		return state;
	}

	public boolean isEverVipMember() {
		return 0 != (state & (1 << CStateEVERVIPMEMBER.value));
	}

	public boolean isGudeDone() {
		return 0 != (state & (1 << CStateGUIDEDONE.value));
	}

	public boolean isSelected() {
		return 0 != (state & (1 << CStateSELECTED.value));
	}

	public boolean isVipMember() {
		return 0 != (state & (1 << CStateVIPMEMBER.value));
	}
	
	public boolean isMSNIMME(){
		return 0 != (state & (1 << CStateMSNIMME.value));
	}

	public boolean isGuideDone(){
		return 0 != (state & (1 << CStateGUIDEDONE.value));
	}

	public boolean isSocialGraphGood(){
		return 0 != (state & (1 << CStateSOCIALGRAPHGOOD.value));
	}

	public boolean isKeepUse(){
		return 0 != (state & (1 << CStateKEEPUSE.value));
	}
	
	public boolean isCharity() {
		return 0 != (state & (1 << CStateCHARITY.value));
	}

	 public boolean isCharityInvited() {
	    return 0 != (state & (1 << CStateCHARITYINVITED.value));
	  }

	 public boolean isApplySpammer() {
			return 0 != (state & (1 << CStateAPPLYSPAMMER.value));
	 }

	 public boolean isVest() {
			return 0 != (state & (1 << CStateVEST.value));
	 }
	 
	 public boolean isExperiment() {
			return 0 != (state & (1 << CStateEXPERIMENT.value));
	 }

	 public boolean isGameManager() {
			return 0 != (state & (1 << CStateGAMEMANAGER.value));
	 }

	 public boolean isActive() {
			return 0 != (state & (1 << CStateACTIVE.value));
	 }

	 public boolean isInWhiteList() {
			return 0 != (state & (1 << CStateINWHITELIST.value));
	 }

	 public boolean isZombie() {
			return 0 != (state & (1 << CStateZOMBIE.value));
	 }
	 
   public boolean isGroupMember() {
			return 0 != (state & (1 << CStateGROUPMEMBER.value));
	 }
   
   public boolean isChannelSpoker() {
			return 0 != (state & (1 << CStateCHANNELSPOKER.value));
	 }
	 
	public int getStar() {
		return star;
	}

	public int getLevel() {
		return level;
	}

	public boolean isOnline() {
		return 0 != (online & (1 << ONLINE_WEBPAGGER)) || 0 != (online & (1 << ONLINE_XNT));
	}
	
	public boolean isWapOnline() {
		return 0 != (online & (1 << ONLINE_WAP));
	}
	
	public void setBase(UserCacheData base) {
		try {
			id = (int)base.getId();
			status = base.getStatus();
			name = base.getName();
			univ = base.getUniv();
			tinyUrl = base.getTinyUrl();
			state = base.getState();
			star = state&1;
			level = base.getLevel();
			online = base.getOnline();
			if(2 == base.getGender()) {
				gender = "女生";
			}else if(1 == base.getGender()){
				gender = "男生";
			}else {
				gender = "";
			}
		} catch (NumberFormatException e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		}
	}
	
	public void setBase(UserCacheData3 base) {
		try {
			id = base.id;
			status = base.status;
			name = base.name;
			univ = base.univ;
			tinyUrl = base.tinyUrl;
			state = base.state;
			star = base.state&1;
			level = base.level;
			online = base.online;
			if(2 == base.gender) {
				gender = "女生";
			}else if(1 == base.gender){
				gender = "男生";
			}else {
				gender = "";
			}
		} catch (NumberFormatException e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		}
	}
	
	protected void setBase(Map<String, String> base) {
		try {
			id = Integer.valueOf(base.get(CUserCacheID.value));
			status = Integer.valueOf(base.get(CUserCacheSTATUS.value));
			name = String.valueOf(base.get(CUserCacheNAME.value));
			univ = Integer.valueOf(base.get(CUserCacheUNIV.value));
			tinyUrl = String.valueOf(base.get(CUserCacheTINYURL.value));
			state = Integer.valueOf(base.get(CUserCacheSTATE.value));
			star = Integer.valueOf(base.get(CUserCacheSTAR.value));
			level = Integer.valueOf(base.get(CUserCacheLEVEL.value));
			online = Integer.valueOf(base.get(CUserCacheONLINE.value));
			gender = String.valueOf(base.get(CUserCacheGENDER.value));
		} catch (NumberFormatException e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() , e);
		}
	}

	@Override
	public String toString() {
		return "[UserCache:id=" + id + ";status=" + status + ";name=" + name
				+ ";univ=" + univ + ";tinyUrl="
				+ tinyUrl + ";state=" + state + ";gender=" + gender +";star=" + star + ";level="
				+ level + ";online=" + online + "]";
	}

}
