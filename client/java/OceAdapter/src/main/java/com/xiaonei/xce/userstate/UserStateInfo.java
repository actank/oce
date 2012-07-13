package com.xiaonei.xce.userstate;

import mop.hi.oce.domain.Updatable;
import xce.userbase.CStateCHARITYINVITED;
import xce.userbase.CStateCHARITY;
import xce.userbase.CStateEVERVIPMEMBER;
import xce.userbase.CStateGUIDEDONE;
import xce.userbase.CStateMOBILEBIND;
import xce.userbase.CStateVEST;
import xce.userbase.CStateDisneyVIP;
import xce.userbase.CStateMSNIMME;
import xce.userbase.CStateCOUPLESSPACE;
import xce.userbase.CStatePAGEMANAGER;
import xce.userbase.CStateSELECTED;
import xce.userbase.CStateKEEPUSE;
import xce.userbase.CStateSOCIALGRAPHGOOD;
import xce.userbase.CStateVIPMEMBER;
import xce.userbase.CStateAPPLYSPAMMER;
import xce.userbase.CStateGAMEMANAGER;
import xce.userbase.CStateHIDEADS;
import xce.userbase.CStateDOUBLEACCOUNT;
import xce.userbase.CStateSAMEPASSWORD;
import xce.userbase.CStateEXPERIMENT;
import xce.userbase.CStateJUMPLOGIC;
import xce.userbase.CStateLUNARBIRTH;
import xce.userbase.CStateGIFTMEMBER;
import xce.userbase.CStateMOBILEMEMBER;
import xce.userbase.CStateINWHITELIST;
import xce.userbase.CStateZOMBIE;
import xce.userbase.CStateGROUPMEMBER;
import xce.userbase.CStateCHANNELSPOKER;
import xce.userbase.CStateV6;
import xce.userbase.CUSID;
import xce.userbase.CUSLEVEL;
import xce.userbase.CUSSTAR;
import xce.userbase.UserStateData;

public class UserStateInfo extends Updatable {
	private int id;
	private int state;
	private int star;
	private int level;

	public int getState() {
		return state;
	}

	public boolean isSelected() {
		return 0 != (state & (1 << CStateSELECTED.value));
	}

	public boolean isVipMember() {
		return 0 != (state & (1 << CStateVIPMEMBER.value));
	}

	public boolean isMSNIMME() {
		return 0 != (state & (1 << CStateMSNIMME.value));
	}

	public boolean isMobileBind() {
		return 0 != (state & (1 << CStateMOBILEBIND.value));
	}
	
	public boolean isEverVipMember() {
		return 0 != (state & (1 << CStateEVERVIPMEMBER.value));
	}

	public boolean isGuideDone() {
		return 0 != (state & (1 << CStateGUIDEDONE.value));
	}
	
	public boolean isSocialGraphGood() {
		return 0 != (state & (1 << CStateSOCIALGRAPHGOOD.value));
	}
	
	public boolean isKeepUse() {
		return 0 != (state & (1 << CStateKEEPUSE.value));
	}
	
	public boolean isCharity() {
		return 0 != (state & (1 << CStateCHARITY.value));
	}

	public boolean isCharityInvited() {
		return 0 != (state & (1 << CStateCHARITYINVITED.value));
	}
	
	public boolean isDoubleAccount() {
	    return 0 != (state & (1 << CStateDOUBLEACCOUNT.value));
	}

	public boolean isInWhiteList() {
	    return 0 != (state & (1 << CStateINWHITELIST.value));
	}
	
  public boolean isGroupMember() {
	    return 0 != (state & (1 << CStateGROUPMEMBER.value));
	}
  
  public boolean isChannelSpoker() {
	    return 0 != (state & (1 << CStateCHANNELSPOKER.value));
	}

	public boolean isHideAds() {
	    return 0 != (state & (1 << CStateHIDEADS.value));
	}

	public boolean isVest() {
	    return 0 != (state & (1 << CStateVEST.value));
	}
	
	public boolean isDisneyVIP() {
	    return 0 != (state & (1 << CStateDisneyVIP.value));
	}
  
	public boolean isGameManager() {
	    return 0 != (state & (1 << CStateGAMEMANAGER.value));
	}

	public boolean isSamePassword() {
	    return 0 != (state & (1 << CStateSAMEPASSWORD.value));
	}

	public boolean isCouplesSpace() {
	    return 0 != (state & (1 << CStateCOUPLESSPACE.value));
	}

	public boolean isPageManager() {
	    return 0 != (state & (1 << CStatePAGEMANAGER.value));
	}
	
	public boolean isExperiment() {
	    return 0 != (state & (1 << CStateEXPERIMENT.value));
	}

	public boolean isJumpLogic() {
	    return 0 != (state & (1 << CStateJUMPLOGIC.value));
	}
	
  public boolean isLunarBirth() {
	    return 0 != (state & (1 << CStateLUNARBIRTH.value));
	}

	public boolean isApplySpammer() {
	    return 0 != (state & (1 << CStateAPPLYSPAMMER.value));
	}
  
	public boolean isGiftMember() {
	    return 0 != (state & (1 << CStateGIFTMEMBER.value));
	}
	
	public boolean isMobileMember() {
	    return 0 != (state & (1 << CStateMOBILEMEMBER.value));
	}
  
	public boolean isZombie() {
	    return 0 != (state & (1 << CStateZOMBIE.value));
	}
	
  public boolean isV6() {
	    return 0 != (state & (1 << CStateV6.value));
	}
	
	public int getStar() {
		return star;
	}

	public void setStar(int star) {
		this.star = star;
		getUpdates().put(CUSSTAR.value, String.valueOf(star));
	}

	public int getLevel() {
		return level;
	}

	public void setLevel(int level) {
		this.level = level;
		getUpdates().put(CUSLEVEL.value, String.valueOf(level));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		getUpdates().put(CUSID.value, String.valueOf(id));
	}

	public UserStateInfo parse(UserStateData source) {
		id = source.id;
		state = source.state;
		star = source.star;
		level = source.level;

		return this;
	}

	@Override
	public String toString() {
		return "[UserState:id=" + id + ";state=" + state + ";star=" + star
				+ ";level=" + level + "]";
	}

}
