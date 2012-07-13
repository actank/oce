package com.xiaonei.xce.blt;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import xce.blt.AgeRange;
import xce.blt.Bulletin;
import xce.blt.TimeRange;

public class BulletinRecord {
	
	public static final String FIELD_CTX_MODDIV = "moddiv";
	public static final String FIELD_CTX_BLTINTEREST = "blt_interest";
	public static final String FIELD_CTX_BLTINTEREST_DIV = "-.-";
	public static final int MOD_DIV_DEFAULT = 10;
	
	public class AgeSeg {
		
		public AgeSeg() {
			this.minAge = -1;
			this.maxAge = -1;
		};
		
		public AgeSeg(int minAge, int maxAge) {
			this.minAge = minAge;
			this.maxAge = maxAge;
		};
		
		public int minAge;
		public int maxAge;
	};
	
	public class TimeSeg {
		
		public TimeSeg() {
			this.beginTime = new Date(0);
			this.endTime = new Date(0);
		};
		
		public TimeSeg(int beginTime, int endTime) {
			this.beginTime = new Date(((long)beginTime)*1000L);
			this.endTime = new Date(((long)endTime)*1000L);
		};
		
		public Date beginTime;
		public Date endTime;
	};
	
	// ------- 定义4个相关的枚举变量 -----------
	public enum WebSite { 
		SITE_REREN(xce.blt.RENREN.value, "人人"),
		SITE_KAIXIN(xce.blt.KAIXIN.value, "开心"),
		SITE_GUIDE(xce.blt.GUIDE.value, "引导"),
		SITE_ALL(xce.blt.ALL.value, "不限");
		
		int value;
		String name;
		WebSite(int value, String name){
			this.value = value;			
			this.name = name;
		}
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	};
	
	public enum Gender {
		GENDER_MALE(xce.blt.MALE.value, "男"),
		GENDER_FEMALE(xce.blt.FEMALE.value, "女"),
		GENDER_ALL(xce.blt.ALL.value, "不限性别");
		
		int value;
		String name;
		Gender(int value, String name) {
			this.value = value;
			this.name = name;
		}	
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	};
	
	public enum UserStar {
		STAR_YES(xce.blt.STARYES.value, "星级用户"),
		STAR_NO(xce.blt.STARNO.value, "非星级用户"),
		STAR_ALL(xce.blt.ALL.value, "不限星级");
		
		int value;
		String name;
		UserStar(int value, String name) {
			this.value = value;
			this.name = name;
		}
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	};
	
	public enum UserVip {
		VIP_YES(xce.blt.VIPYES.value, "VIP用户"),
		VIP_NO(xce.blt.VIPNO.value, "非VIP用户"),
		VIP_ALL(xce.blt.ALL.value, "不限VIP");
		
		int value;
		String name;
		UserVip(int value, String name) {
			this.value = value;
			this.name = name;
		}
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	};	
	
	public enum UserStage {
		STAGE_NONE(xce.blt.STAGENONE.value, "无网络"),
		STAGE_MIDDLESCHOOL(xce.blt.STAGEMIDDLESCHOOL.value, "中学"),
		STAGE_COLLAGE(xce.blt.STAGECOLLAGE.value, "大学"),
		STAGE_WHITECOLLAR(xce.blt.STAGEWHITECOLLAR.value, "白领");
		
		int value;
		String name;
		UserStage(int value, String name) {
			this.value = value;
			this.name = name;
		}
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	}
	
	public enum UserComplete {
		USER_CONTACT(xce.blt.USERCONTACT.value, "联系方式"),
		USER_FAVOR(xce.blt.USERFAVOR.value, "兴趣爱好"),
		USER_UNIV(xce.blt.USERUNIV.value, "学校"),
		USER_CAREER(xce.blt.USERCAREER.value, "工作");		
		
		int value;
		String name;
		UserComplete(int value, String name) {
			this.value = value;
			this.name = name;
		}
		
		public int getValue() {
            return value;
		}

		public String getName() {
            return name;
		}
	};	
	
	// BulletinBoard变量
	private Bulletin bulletin;
	
	public BulletinRecord() {
		bulletin = new Bulletin();
		bulletin.bulletinid = 0;
		bulletin.completes = new int[0];
		bulletin.uncompletes = new int[0];
		bulletin.star = xce.blt.ALL.value;
		bulletin.vip = xce.blt.ALL.value;
		bulletin.website = xce.blt.ALL.value;
		bulletin.gender = xce.blt.ALL.value;
		bulletin.weight = 10;
		bulletin.ages = new AgeRange[0];
		bulletin.onlinetime = new TimeRange[0];
		bulletin.idgiven = new int[0];
		bulletin.idmods = new int[0];
		bulletin.text = "";
		bulletin.persist = false;
		bulletin.maxshow = -1;
		bulletin.online = false;
		bulletin.ctxs = new HashMap<String, int[]>();
	}
	
	public BulletinRecord(Bulletin bulletin) {
		this.bulletin = bulletin;
	}	

	public Bulletin getBulletin() {
		if (bulletin == null) {
			bulletin = new Bulletin();
			bulletin.bulletinid = 0;
			bulletin.completes  = new int[0];
			bulletin.uncompletes = new int[0];
			bulletin.star = xce.blt.ALL.value;
			bulletin.vip = xce.blt.ALL.value;
			bulletin.website = xce.blt.ALL.value;
			bulletin.weight = 10;
			bulletin.ages = new AgeRange[0];
			bulletin.onlinetime = new TimeRange[0];
			bulletin.idgiven = new int[0];
			bulletin.idmods = new int[0];
			bulletin.text = "";
			bulletin.persist = false;
			bulletin.maxshow = -1;
			bulletin.online = false;
			bulletin.ctxs = new HashMap<String, int[]>();
		}
		return bulletin;
	}

	public void setBulletin(Bulletin bulletin) {
		this.bulletin = bulletin;
	}
	
	public void setBulletinCtxs(HashMap<String, int[]> temp_ctxs) {
		this.bulletin.ctxs = temp_ctxs;
	}

	// ----- 各成员存取 ----------------	
	public int getBulletinId() {
		return this.bulletin.bulletinid;
	}
	
	public void setText(String text) {
		bulletin.text = text;
	}
	
	public String getText() {
		return bulletin.text;
	}
	
	public void setTitle(String title) {
		bulletin.title = title;
	}
	
	public String getTitle() {
		return bulletin.title;
	}
	
	public void setWebSite(WebSite website) {
		bulletin.website = website.value;
	}
	
	public WebSite getWebSite() {
		if ( bulletin.website == WebSite.SITE_REREN.value) {
			return WebSite.SITE_REREN;
		} else if ( bulletin.website == WebSite.SITE_KAIXIN.value) {
			return WebSite.SITE_KAIXIN;
		} else {
			return WebSite.SITE_ALL;
		}		
	}
	
	public void setGender(Gender gender) {
		bulletin.gender = gender.value;
	}
	
	public Gender getGender() {
		if ( bulletin.gender == Gender.GENDER_MALE.value) {
			return Gender.GENDER_MALE;
		} else if ( bulletin.gender == Gender.GENDER_FEMALE.value) {
			return Gender.GENDER_FEMALE;
		} else {
			return Gender.GENDER_ALL;
		}
	}
	
	public void setStar(UserStar star) {
		bulletin.star = star.value;
	}
	
	public void setUserStage(UserStage[] stages) {
		if ( stages != null ) {
			int[] stageInts = new int[stages.length];
			for (int i = 0; i < stages.length; ++i) {
				stageInts[i] = stages[i].value;
			}
			bulletin.ctxs.put(xce.blt.CTXUSERSTAGE.value, stageInts);
		}		
	}
	
	public UserStage[] getUserStage() {
		int[] stageInts = null;
		if ( bulletin.ctxs.containsKey(xce.blt.CTXUSERSTAGE.value)) {
			stageInts = bulletin.ctxs.get(xce.blt.CTXUSERSTAGE.value);
			UserStage[] stages = new UserStage[stageInts.length];
			for ( int i = 0; i < stageInts.length; ++i) {
				if ( stageInts[i] == UserStage.STAGE_NONE.value) {
					stages[i] = UserStage.STAGE_NONE;
				} else if ( stageInts[i] == UserStage.STAGE_MIDDLESCHOOL.value) {
					stages[i] = UserStage.STAGE_MIDDLESCHOOL;
				} else if ( stageInts[i] == UserStage.STAGE_COLLAGE.value) {
					stages[i] = UserStage.STAGE_COLLAGE;
				} else if ( stageInts[i] == UserStage.STAGE_WHITECOLLAR.value) {
					stages[i] = UserStage.STAGE_WHITECOLLAR;
				} else {
					stages[i] = null;					
				}
			}
			return stages;			
		} else {
			return new UserStage[0];
		}
	}
	
	
	public UserStar getStar() {
		if ( bulletin.star == UserStar.STAR_YES.value) {
			return UserStar.STAR_YES;
		} else if ( bulletin.star == UserStar.STAR_NO.value) {
			return UserStar.STAR_NO;
		} else {
			return UserStar.STAR_ALL;
		}
	}
	
	public void setVip(UserVip vip) {
		bulletin.vip = vip.value;
	}
	
	public UserVip getVip() {
		if ( bulletin.vip == UserVip.VIP_YES.value) {
			return UserVip.VIP_YES;
		} else if ( bulletin.vip == UserVip.VIP_NO.value ) {
			return UserVip.VIP_NO;
		} else {
			return UserVip.VIP_ALL;
		}		
	}
	
	public UserComplete[] getCompletes() {
		Vector<UserComplete> result = new Vector<UserComplete>();
		for (int i = 0; i < bulletin.completes.length; ++i) {
			if ( bulletin.completes[i] == UserComplete.USER_CONTACT.value) {
				result.add(UserComplete.USER_CONTACT);
			} else if ( bulletin.completes[i] == UserComplete.USER_FAVOR.value) {
				result.add(UserComplete.USER_FAVOR);
			} else if ( bulletin.completes[i] == UserComplete.USER_UNIV.value) {
				result.add(UserComplete.USER_UNIV);
			} else if ( bulletin.completes[i] == UserComplete.USER_CAREER.value) {
				result.add(UserComplete.USER_CAREER);
			}
		}
		UserComplete[] comp = new UserComplete[result.size()];
		return result.toArray(comp);		
	}
	
	public void setCompletes(UserComplete[] comp) {		
		bulletin.completes = new int[comp.length];
		for (int i = 0; i < comp.length; ++i) {
			bulletin.completes[i] = comp[i].value;
		}				
	}
	
	public UserComplete[] getUncompletes() {
		Vector<UserComplete> result = new Vector<UserComplete>();
		for (int i = 0; i < bulletin.uncompletes.length; ++i) {
			if ( bulletin.uncompletes[i] == UserComplete.USER_CONTACT.value) {
				result.add(UserComplete.USER_CONTACT);
			} else if ( bulletin.uncompletes[i] == UserComplete.USER_FAVOR.value) {
				result.add(UserComplete.USER_FAVOR);
			} else if ( bulletin.uncompletes[i] == UserComplete.USER_UNIV.value) {
				result.add(UserComplete.USER_UNIV);
			} else if ( bulletin.uncompletes[i] == UserComplete.USER_CAREER.value) {
				result.add(UserComplete.USER_CAREER);
			}
		}
		UserComplete[] comp = new UserComplete[result.size()];
		return result.toArray(comp);
	}
	
	public void setUncompletes(UserComplete[] comp) {		
		bulletin.uncompletes = new int[comp.length];
		for (int i = 0; i < comp.length; ++i) {
			bulletin.uncompletes[i] = comp[i].value;
		}				
	}
	
	public AgeSeg[] getAgeScope() {
		AgeSeg[] ages = new AgeSeg[bulletin.ages.length];
		for ( int i = 0 ; i < bulletin.ages.length; ++i ) {
			AgeSeg as = new AgeSeg(bulletin.ages[i].minAge, bulletin.ages[i].maxAge);
			ages[i] = as;
		}
		return ages;
	}
	
	public void setAgeScope(AgeSeg[] ages) {
		Vector<AgeRange> ageScope = new Vector<AgeRange>(); 
		for ( int i = 0; i < ages.length; ++i ) {
			if ( ages[i].minAge < 0 ) {
				ages[i].minAge = 0;
			}
			if (  ( ages[i].maxAge < 0) || (ages[i].minAge > ages[i].maxAge) ) {
				continue;
			}			
			AgeRange ageRange = new AgeRange();
			ageRange.minAge = ages[i].minAge;
			ageRange.maxAge = ages[i].maxAge;
			ageScope.add(ageRange);			
		}
		AgeRange[] ageRangeSeq = new AgeRange[ageScope.size()];
		ageRangeSeq = ageScope.toArray(ageRangeSeq);
		bulletin.ages = ageRangeSeq;
	}
	
	public TimeSeg[] getTimeScope() {
		TimeSeg[] times = new TimeSeg[bulletin.onlinetime.length];		
		for ( int i = 0 ; i < bulletin.onlinetime.length; ++i ) {
			TimeSeg ts = new TimeSeg(bulletin.onlinetime[i].beginTime, bulletin.onlinetime[i].endTime);
			times[i] = ts;
		}
		return times;
	}
	
	public void setTimeScope(TimeSeg[] times) {	
		
		Vector<TimeRange> timeScope = new Vector<TimeRange>(); 
		for ( int i = 0; i < times.length; ++i ) {
			/* Date now = new Date();
			if ( times[i].endTime.before(now) ) {
				continue;
			}*/						
			TimeRange timeRange = new TimeRange();			
			Long bt = times[i].beginTime.getTime()/1000;
			Long et = times[i].endTime.getTime()/1000;			
			timeRange.beginTime = bt.intValue();			
			timeRange.endTime = et.intValue();			
			timeScope.add(timeRange);	
		}
		TimeRange[] timeRangeSeq = new TimeRange[timeScope.size()];
		timeRangeSeq = timeScope.toArray(timeRangeSeq);
		bulletin.onlinetime = timeRangeSeq;
	}
	
	public int[] getIdmods() {
		return bulletin.idmods;
	}
	
	public void setIdmods(int[] idmods) {
		bulletin.idmods = idmods;
	}
	
	public int getModDiv() {
		int moddiv = MOD_DIV_DEFAULT;
		if ( bulletin.ctxs.get(FIELD_CTX_MODDIV) != null ) {
			if ( bulletin.ctxs.get(FIELD_CTX_MODDIV).length >= 1 ) {
				if ( bulletin.ctxs.get(FIELD_CTX_MODDIV)[0] > 0 ) {
					moddiv = bulletin.ctxs.get(FIELD_CTX_MODDIV)[0];
				}				
			}
		}
		return moddiv;
	}
	
	public void setModDiv(int moddiv) {
		if ( moddiv <= 0 ) {
			System.out.println("[BulletinRecord.setMods] moddiv " + moddiv + " is invalid");
			return ;
		}
		if ( bulletin.ctxs.get(FIELD_CTX_MODDIV) == null ) {
			int[] moddivs = new int[]{moddiv};
			bulletin.ctxs.put(FIELD_CTX_MODDIV, moddivs);
		} else if ( bulletin.ctxs.get(FIELD_CTX_MODDIV).length <= 0 ) {
			int[] moddivs = new int[]{moddiv};
			bulletin.ctxs.remove(FIELD_CTX_MODDIV);
			bulletin.ctxs.put(FIELD_CTX_MODDIV, moddivs);			
		} else {
			bulletin.ctxs.get(FIELD_CTX_MODDIV)[0] = moddiv;			
		}		
	}
	
	public void setInterestUserDbTable(String table) {		
		String BltInterestTag = FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV;
		List<String> toClear = new ArrayList<String>();
		for ( String ctxKey :bulletin.ctxs.keySet() ) {
			if ( ctxKey.startsWith(BltInterestTag)) {
				toClear.add(ctxKey);				
			}
		}
		for ( String clearKey : toClear ) {
			bulletin.ctxs.remove(clearKey);
		}
		
		if ( ! table.equals("") ) {
			String ctxKey =  FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV + table;
			int[] value = new int[]{0};
			bulletin.ctxs.put(ctxKey, value);
		}
	}
	
	public String getInterestUserDbTable() {
		String BltInterestTag = FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV;
		
		System.out.println("BltInterestTag:" + BltInterestTag);
		
		for ( String ctxKey :bulletin.ctxs.keySet() ) {
			if ( ctxKey.startsWith(BltInterestTag)) {				
				String table = ctxKey.substring(BltInterestTag.length(), ctxKey.length());
				return table;
			}
		}
		return "";
	}
	
	public int[] getIdgiven() {
		return bulletin.idgiven;
	}
	
	public void setIdgiven(int[] idgiven) {
		bulletin.idgiven = idgiven;
	}	
	
	public void setAppUse(int[] appuse) {
		bulletin.appuse = appuse;
	}
	
	public int[] getAppUse() {
		return bulletin.appuse;
	}
	
	public void setAppUnuse(int[] appunuse) {
		bulletin.appunuse = appunuse;
	}
	
	public int[] getAppUnuse() {
		return bulletin.appunuse;
	}
	
	public int getWeight() {
		return bulletin.weight;
	}
	
	public void setWeight(int weight) {
		bulletin.weight = weight;
	}
	
	public boolean getPersist() {
		return bulletin.persist;
	}
	
	public void setPersist(boolean persist) {
		bulletin.persist = persist;
	}
	
	public int getMaxShow() {
		return bulletin.maxshow;
	}
	
	public void setMaxShow(int maxshow) {
		bulletin.maxshow = maxshow;
	}
	
	public boolean getOnline() {
		return bulletin.online;
	}
	
	public void setOnline(boolean online) {
		bulletin.online = online;
	}	

	public String toString() {
		return "id = " + this.bulletin.bulletinid + "\n" 
				+ "online = " + this.bulletin.online + "\n"
				+ "html=" + this.bulletin.text + "\n";
	}
}
