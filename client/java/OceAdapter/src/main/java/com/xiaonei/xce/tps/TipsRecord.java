package com.xiaonei.xce.tps;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import xce.tps.AgeRange;
import xce.tps.Tips;
import xce.tps.TimeRange;

public class TipsRecord {
	
	public static final String FIELD_CTX_MODDIV = "moddiv";
	public static final String FIELD_CTX_BLTINTEREST = "tps_interest";
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
		SITE_RENREN(xce.tps.RENREN.value, "人人"),
		SITE_XIAONEI(xce.tps.XIAONEI.value, "校内"),
		SITE_GUIDE(xce.tps.GUIDE.value, "引导"),
		SITE_ALL(xce.tps.ALL.value, "不限");
		
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
		GENDER_MALE(xce.tps.MALE.value, "男"),
		GENDER_FEMALE(xce.tps.FEMALE.value, "女"),
		GENDER_ALL(xce.tps.ALL.value, "不限性别");
		
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
		STAR_YES(xce.tps.STARYES.value, "星级用户"),
		STAR_NO(xce.tps.STARNO.value, "非星级用户"),
		STAR_ALL(xce.tps.ALL.value, "不限星级");
		
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
		VIP_YES(xce.tps.VIPYES.value, "VIP用户"),
		VIP_NO(xce.tps.VIPNO.value, "非VIP用户"),
		VIP_ALL(xce.tps.ALL.value, "不限VIP");
		
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
		STAGE_NONE(xce.tps.STAGENONE.value, "无网络"),
		STAGE_MIDDLESCHOOL(xce.tps.STAGEMIDDLESCHOOL.value, "中学"),
		STAGE_COLLAGE(xce.tps.STAGECOLLAGE.value, "大学"),
		STAGE_WHITECOLLAR(xce.tps.STAGEWHITECOLLAR.value, "白领");
		
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
		USER_CONTACT(xce.tps.USERCONTACT.value, "联系方式"),
		USER_FAVOR(xce.tps.USERFAVOR.value, "兴趣爱好"),
		USER_UNIV(xce.tps.USERUNIV.value, "学校"),
		USER_CAREER(xce.tps.USERCAREER.value, "工作");		
		
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
	
	// TipsBoard变量
	private Tips tips;
	
	public TipsRecord() {
		tips = new Tips();
		tips.tipsid = 0;
		tips.completes = new int[0];
		tips.uncompletes = new int[0];
		tips.star = xce.tps.ALL.value;
		tips.vip = xce.tps.ALL.value;
		tips.website = xce.tps.ALL.value;
		tips.gender = xce.tps.ALL.value;
		tips.weight = 10;
		tips.ages = new AgeRange[0];
		tips.onlinetime = new TimeRange[0];
		tips.idgiven = new int[0];
		tips.idmods = new int[0];
		tips.text = "";
		tips.persist = false;
		tips.maxshow = -1;
		tips.online = false;
		tips.ctxs = new HashMap<String, int[]>();
	}
	
	public TipsRecord(Tips tips) {
		this.tips = tips;
	}	

	public Tips getTips() {
		if (tips == null) {
			tips = new Tips();
			tips.tipsid = 0;
			tips.completes  = new int[0];
			tips.uncompletes = new int[0];
			tips.star = xce.tps.ALL.value;
			tips.vip = xce.tps.ALL.value;
			tips.website = xce.tps.ALL.value;
			tips.weight = 10;
			tips.ages = new AgeRange[0];
			tips.onlinetime = new TimeRange[0];
			tips.idgiven = new int[0];
			tips.idmods = new int[0];
			tips.text = "";
			tips.persist = false;
			tips.maxshow = -1;
			tips.online = false;
			tips.ctxs = new HashMap<String, int[]>();
		}
		return tips;
	}

	public void setTips(Tips tips) {
		this.tips = tips;
	}
	
	public void setTipsCtxs(HashMap<String, int[]> temp_ctxs) {
		this.tips.ctxs = temp_ctxs;
	}

	// ----- 各成员存取 ----------------	
	public int getTipsId() {
		return this.tips.tipsid;
	}
	
	public void setText(String text) {
		tips.text = text;
	}
	
	public String getText() {
		return tips.text;
	}
	
	public void setTitle(String title) {
		tips.title = title;
	}
	
	public String getTitle() {
		return tips.title;
	}
	
	public void setWebSite(WebSite website) {
		tips.website = website.value;
	}
	
	public WebSite getWebSite() {
		if ( tips.website == WebSite.SITE_RENREN.value) {
			return WebSite.SITE_RENREN;
		} else if ( tips.website == WebSite.SITE_XIAONEI.value) {
			return WebSite.SITE_XIAONEI;
		} else {
			return WebSite.SITE_ALL;
		}		
	}
	
	public void setGender(Gender gender) {
		tips.gender = gender.value;
	}
	
	public Gender getGender() {
		if ( tips.gender == Gender.GENDER_MALE.value) {
			return Gender.GENDER_MALE;
		} else if ( tips.gender == Gender.GENDER_FEMALE.value) {
			return Gender.GENDER_FEMALE;
		} else {
			return Gender.GENDER_ALL;
		}
	}
	
	public void setStar(UserStar star) {
		tips.star = star.value;
	}
	
	public void setUserStage(UserStage[] stages) {
		if ( stages != null ) {
			int[] stageInts = new int[stages.length];
			for (int i = 0; i < stages.length; ++i) {
				stageInts[i] = stages[i].value;
			}
			tips.ctxs.put(xce.tps.CTXUSERSTAGE.value, stageInts);
		}		
	}
	
	public UserStage[] getUserStage() {
		int[] stageInts = null;
		if ( tips.ctxs.containsKey(xce.tps.CTXUSERSTAGE.value)) {
			stageInts = tips.ctxs.get(xce.tps.CTXUSERSTAGE.value);
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
		if ( tips.star == UserStar.STAR_YES.value) {
			return UserStar.STAR_YES;
		} else if ( tips.star == UserStar.STAR_NO.value) {
			return UserStar.STAR_NO;
		} else {
			return UserStar.STAR_ALL;
		}
	}
	
	public void setVip(UserVip vip) {
		tips.vip = vip.value;
	}
	
	public UserVip getVip() {
		if ( tips.vip == UserVip.VIP_YES.value) {
			return UserVip.VIP_YES;
		} else if ( tips.vip == UserVip.VIP_NO.value ) {
			return UserVip.VIP_NO;
		} else {
			return UserVip.VIP_ALL;
		}		
	}
	
	public UserComplete[] getCompletes() {
		Vector<UserComplete> result = new Vector<UserComplete>();
		for (int i = 0; i < tips.completes.length; ++i) {
			if ( tips.completes[i] == UserComplete.USER_CONTACT.value) {
				result.add(UserComplete.USER_CONTACT);
			} else if ( tips.completes[i] == UserComplete.USER_FAVOR.value) {
				result.add(UserComplete.USER_FAVOR);
			} else if ( tips.completes[i] == UserComplete.USER_UNIV.value) {
				result.add(UserComplete.USER_UNIV);
			} else if ( tips.completes[i] == UserComplete.USER_CAREER.value) {
				result.add(UserComplete.USER_CAREER);
			}
		}
		UserComplete[] comp = new UserComplete[result.size()];
		return result.toArray(comp);		
	}
	
	public void setCompletes(UserComplete[] comp) {		
		tips.completes = new int[comp.length];
		for (int i = 0; i < comp.length; ++i) {
			tips.completes[i] = comp[i].value;
		}				
	}
	
	public UserComplete[] getUncompletes() {
		Vector<UserComplete> result = new Vector<UserComplete>();
		for (int i = 0; i < tips.uncompletes.length; ++i) {
			if ( tips.uncompletes[i] == UserComplete.USER_CONTACT.value) {
				result.add(UserComplete.USER_CONTACT);
			} else if ( tips.uncompletes[i] == UserComplete.USER_FAVOR.value) {
				result.add(UserComplete.USER_FAVOR);
			} else if ( tips.uncompletes[i] == UserComplete.USER_UNIV.value) {
				result.add(UserComplete.USER_UNIV);
			} else if ( tips.uncompletes[i] == UserComplete.USER_CAREER.value) {
				result.add(UserComplete.USER_CAREER);
			}
		}
		UserComplete[] comp = new UserComplete[result.size()];
		return result.toArray(comp);
	}
	
	public void setUncompletes(UserComplete[] comp) {		
		tips.uncompletes = new int[comp.length];
		for (int i = 0; i < comp.length; ++i) {
			tips.uncompletes[i] = comp[i].value;
		}				
	}
	
	public AgeSeg[] getAgeScope() {
		AgeSeg[] ages = new AgeSeg[tips.ages.length];
		for ( int i = 0 ; i < tips.ages.length; ++i ) {
			AgeSeg as = new AgeSeg(tips.ages[i].minAge, tips.ages[i].maxAge);
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
		tips.ages = ageRangeSeq;
	}
	
	public TimeSeg[] getTimeScope() {
		TimeSeg[] times = new TimeSeg[tips.onlinetime.length];		
		for ( int i = 0 ; i < tips.onlinetime.length; ++i ) {
			TimeSeg ts = new TimeSeg(tips.onlinetime[i].beginTime, tips.onlinetime[i].endTime);
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
		tips.onlinetime = timeRangeSeq;
	}
	
	public int[] getIdmods() {
		return tips.idmods;
	}
	
	public void setIdmods(int[] idmods) {
		tips.idmods = idmods;
	}
	
	public int getModDiv() {
		int moddiv = MOD_DIV_DEFAULT;
		if ( tips.ctxs.get(FIELD_CTX_MODDIV) != null ) {
			if ( tips.ctxs.get(FIELD_CTX_MODDIV).length >= 1 ) {
				if ( tips.ctxs.get(FIELD_CTX_MODDIV)[0] > 0 ) {
					moddiv = tips.ctxs.get(FIELD_CTX_MODDIV)[0];
				}				
			}
		}
		return moddiv;
	}
	
	public void setModDiv(int moddiv) {
		if ( moddiv <= 0 ) {
			System.out.println("[TipsRecord.setMods] moddiv " + moddiv + " is invalid");
			return ;
		}
		if ( tips.ctxs.get(FIELD_CTX_MODDIV) == null ) {
			int[] moddivs = new int[]{moddiv};
			tips.ctxs.put(FIELD_CTX_MODDIV, moddivs);
		} else if ( tips.ctxs.get(FIELD_CTX_MODDIV).length <= 0 ) {
			int[] moddivs = new int[]{moddiv};
			tips.ctxs.remove(FIELD_CTX_MODDIV);
			tips.ctxs.put(FIELD_CTX_MODDIV, moddivs);			
		} else {
			tips.ctxs.get(FIELD_CTX_MODDIV)[0] = moddiv;			
		}		
	}
	
	public void setInterestUserDbTable(String table) {		
		String BltInterestTag = FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV;
		List<String> toClear = new ArrayList<String>();
		for ( String ctxKey :tips.ctxs.keySet() ) {
			if ( ctxKey.startsWith(BltInterestTag)) {
				toClear.add(ctxKey);				
			}
		}
		for ( String clearKey : toClear ) {
			tips.ctxs.remove(clearKey);
		}
		
		if ( ! table.equals("") ) {
			String ctxKey =  FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV + table;
			int[] value = new int[]{0};
			tips.ctxs.put(ctxKey, value);
		}
	}
	
	public String getInterestUserDbTable() {
		String BltInterestTag = FIELD_CTX_BLTINTEREST + FIELD_CTX_BLTINTEREST_DIV;
		
		System.out.println("BltInterestTag:" + BltInterestTag);
		
		for ( String ctxKey :tips.ctxs.keySet() ) {
			if ( ctxKey.startsWith(BltInterestTag)) {				
				String table = ctxKey.substring(BltInterestTag.length(), ctxKey.length());
				return table;
			}
		}
		return "";
	}
	
	public int[] getIdgiven() {
		return tips.idgiven;
	}
	
	public void setIdgiven(int[] idgiven) {
		tips.idgiven = idgiven;
	}	
	
	public void setAppUse(int[] appuse) {
		tips.appuse = appuse;
	}
	
	public int[] getAppUse() {
		return tips.appuse;
	}
	
	public void setAppUnuse(int[] appunuse) {
		tips.appunuse = appunuse;
	}
	
	public int[] getAppUnuse() {
		return tips.appunuse;
	}
	
	public int getWeight() {
		return tips.weight;
	}
	
	public void setWeight(int weight) {
		tips.weight = weight;
	}
	
	public boolean getPersist() {
		return tips.persist;
	}
	
	public void setPersist(boolean persist) {
		tips.persist = persist;
	}
	
	public int getMaxShow() {
		return tips.maxshow;
	}
	
	public void setMaxShow(int maxshow) {
		tips.maxshow = maxshow;
	}
	
	public boolean getOnline() {
		return tips.online;
	}
	
	public void setOnline(boolean online) {
		tips.online = online;
	}	

	public String toString() {
		return "id = " + this.tips.tipsid + "\n" 
				+ "online = " + this.tips.online + "\n"
				+ "html=" + this.tips.text + "\n";
	}
}
