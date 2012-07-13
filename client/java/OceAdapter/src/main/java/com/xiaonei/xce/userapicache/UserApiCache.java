package com.xiaonei.xce.userapicache;

import com.xiaonei.xce.log.Oce;

import xce.userapicache.UserApiCacheData1;
import xce.distcache.userapicache.UserApiCache.UserApiCacheData;
public class UserApiCache {
    private final int SELECTED = 0;
    private final int VIPMEMBER = 1;
    private final int MSNIMME = 2;
    private final int MOBILEBIND = 3;
    private final int EVERVIPMEMBER = 4;

    public int id;
    public String name;
    public int stage;
    public int state;
    public int status;
    public String gender;
    public int auth;
    public int statusconfig;
    public int basicconfig;
    public String homeprovince;
    public String homecity;
    public int birthyear;
    public int birthmonth;
    public int birthday;
    public int univ;
    public String mainurl;
    public String headurl;
    public String tinyurl;
    public String profileprivacy;
    public int level;

    public void setBase(UserApiCacheData1 data) {
	try {
	    id = data.id;
	    name = data.name;
	    stage = data.stage;
	    state = data.state;
	    status = data.status;
	    gender = data.gender;
	    auth = data.auth;
	    statusconfig = data.statusconfig;
	    basicconfig = data.basicconfig;
	    homeprovince = data.homeprovince;
	    homecity = data.homecity;
	    birthyear = data.birthyear;
	    birthmonth = data.birthmonth;
	    birthday = data.birthday;
	    univ = data.univ;
	    mainurl = data.mainurl;
	    headurl = data.headurl;
	    tinyurl = data.tinyurl;
	    profileprivacy = data.profileprivacy;
	    level = data.level;
	} catch (NumberFormatException e) {
	    Oce.getLogger().error(this.getClass().getName(), e);
	} catch (Exception e) {
	    Oce.getLogger().error(this.getClass().getName(), e);
	}
    }
    
    public void setBase(UserApiCacheData data) {
    	try {
    	    id = (int)data.getId();
    	    name = data.getName();
    	    stage = data.getStage();
    	    state = data.getState();
    	    status = data.getStatus();
    	    gender = data.getGender();
    	    auth = data.getAuth();
    	    statusconfig = data.getStatusconfig();
    	    basicconfig = data.getBasicconfig();
    	    homeprovince = data.getHomeprovince();
    	    homecity = data.getHomecity();
    	    birthyear = data.getBirthyear();
    	    birthmonth = data.getBirthmonth();
    	    birthday = data.getBirthday();
    	    univ = data.getUniv();
    	    mainurl = data.getMainurl();
    	    headurl = data.getHeadurl();
    	    tinyurl = data.getTinyurl();
    	    profileprivacy = data.getProfileprivacy();
    	    level = data.getLevel();
    	} catch (NumberFormatException e) {
    	    Oce.getLogger().error(this.getClass().getName(), e);
    	} catch (Exception e) {
    	    Oce.getLogger().error(this.getClass().getName(), e);
    	}
        }

    public int getLevel() {
	return level;
    }

    public int getAuth() {
	return auth;
    }

    public int getBasicconfig() {
	return basicconfig;
    }

    public int getBirthday() {
	return birthday;
    }

    public int getBirthmonth() {
	return birthmonth;
    }

    public int getBirthyear() {
	return birthyear;
    }

    public String getHeadurl() {
	return headurl;
    }

    public String getHomecity() {
	return homecity;
    }

    public String getHomeprovince() {
	return homeprovince;
    }

    public int getId() {
	return id;
    }

    public String getMainurl() {
	return mainurl;
    }

    public String getName() {
	return name;
    }

    public String getGender() {
	return gender;
    }

    public String getSex() {
	return gender;
    }

    public int getStage() {
	return stage;
    }

    public int getState() {
	return state;
    }

    public int getStatus() {
	return status;
    }

    public int getStatusconfig() {
	return statusconfig;
    }

    public String getTinyurl() {
	return tinyurl;
    }

    public int getUniv() {
	return univ;
    }

    public String getProfileprivacy() {
	return profileprivacy;
    }

    public boolean isSelected() {
	return 0 != (state & (1 << SELECTED));
    }

    public boolean isMobileBind() {
	return 0 != (state & (1 << MOBILEBIND));
    }

    public boolean isMsnImMe() {
	return 0 != (state & (1 << MSNIMME));
    }

    public boolean isEverVipMember() {
	return 0 != (state & (1 << EVERVIPMEMBER));
    }

    public boolean isVipMember() {
	return 0 != (state & (1 << VIPMEMBER));
    }

}
