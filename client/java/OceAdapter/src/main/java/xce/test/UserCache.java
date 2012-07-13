package xce.test;

import java.util.Map;

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
import xce.userbase.CStateSELECTED;
import xce.userbase.CStateVIPMEMBER;
import xce.userbase.CStateGUIDEDONE;
import xce.userbase.CStateMSNIMME;
import xce.userbase.CStateEVERVIPMEMBER;

public abstract class UserCache {
	public static final int OFFLINE = xce.mediator.onlineevent.OFFLINE.value;
	public static final int ONLINE_WEBPAGGER = xce.mediator.onlineevent.ONLINEWEBPAGGER.value;
	public static final int ONLINE_XNT = xce.mediator.onlineevent.ONLINEXNT.value;

	private int id;
	private int status;

	private String name;
	private int univ;
	//private String headUrl;
	private String tinyUrl;
	private String gender;

	private int state;
	private int star;
	private int level;
	//private String doing;
	private int online;

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

	/*public String getHeadUrl() {
		return headUrl;
	}*/

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

	public int getStar() {
		return star;
	}

	public int getLevel() {
		return level;
	}

	/*public String getDoing() {
		return doing;
	}*/

	public boolean isOnline() {
		return online != OFFLINE;
	}
	
	protected void setBase(UserCacheData3 base) {
		try {
			id = base.id;
			status = base.status;
			name = base.name;
			univ = base.univ;
			//headUrl = String.valueOf(base.get(CUserCacheHEADURL.value));
			tinyUrl = base.tinyUrl;
			state = base.state;
			star = base.state&1;
			level = base.level;
			//doing = String.valueOf(base.get(CUserCacheDOING.value));
			online = base.online;
			if(2 == base.gender) {
				gender = "女生";
			}else if(1 == base.gender){
				gender = "男生";
			}else {
				gender = "";
			}
		} catch (NumberFormatException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	protected void setBase(Map<String, String> base) {
		try {
			id = Integer.valueOf(base.get(CUserCacheID.value));
			status = Integer.valueOf(base.get(CUserCacheSTATUS.value));
			name = String.valueOf(base.get(CUserCacheNAME.value));
			univ = Integer.valueOf(base.get(CUserCacheUNIV.value));
			//headUrl = String.valueOf(base.get(CUserCacheHEADURL.value));
			tinyUrl = String.valueOf(base.get(CUserCacheTINYURL.value));
			state = Integer.valueOf(base.get(CUserCacheSTATE.value));
			star = Integer.valueOf(base.get(CUserCacheSTAR.value));
			level = Integer.valueOf(base.get(CUserCacheLEVEL.value));
			//doing = String.valueOf(base.get(CUserCacheDOING.value));
			online = Integer.valueOf(base.get(CUserCacheONLINE.value));
			gender = String.valueOf(base.get(CUserCacheGENDER.value));
		} catch (NumberFormatException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
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
