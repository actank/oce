package com.xiaonei.xce.offerfriends;

import xce.offerfriends.BaseTraitAreaEnrollSame;
import xce.offerfriends.BaseTraitCommonFriend;
import xce.offerfriends.BaseTraitSameInfoWorkPlace;
import xce.offerfriends.BaseTraitSameInfoUniv;
import xce.offerfriends.BaseTraitSameInfoHighSchool;
import xce.offerfriends.BaseTraitSameInfoJuniorSchool;
import xce.offerfriends.BaseTraitSameInfoElementarySchool;
import xce.offerfriends.BaseTraitIPSame;

public abstract class Offer {
	private int _userId;
	private int _weight;
	private int _sign;

        public static enum OfferType {
                BASETYPE_COMMONFRIEND(BaseTraitCommonFriend.value,"BASETYPE_COMMONFRIEND")//
                , BASETYPE_SAMEINFO_WORKPLACE(BaseTraitSameInfoWorkPlace.value,"BASETYPE_SAMEINFO_WORKPLACE")//
                , BASETYPE_SAMEINFO_UNIV(BaseTraitSameInfoUniv.value,"BASETYPE_SAMEINFO_UNIV")//
                , BASETYPE_SAMEINFO_HIGHSCHOOL(BaseTraitSameInfoHighSchool.value,"BASETYPE_SAMEINFO_HIGHSCHOOL")//
                , BASETYPE_SAMEINFO_JUNIORSCHOOL(BaseTraitSameInfoJuniorSchool.value,"BASETYPE_SAMEINFO_JUNIORSCHOOL")//
                , BASETYPE_SAMEINFO_ELEMENTARYSCHOOL(BaseTraitSameInfoElementarySchool.value,"BASETYPE_SAMEINFO_ELEMENTARYSCHOOL")//
                , BASETYPE_IPSAME(BaseTraitIPSame.value,"BASETYPE_IPSAME")//
                , BASETYPE_SAMEAREASAMEENROLL(BaseTraitAreaEnrollSame.value, "BASETYPE_SAMEAREASAMEENROLL")
                ;
                public final int pos;// the bit in sign
                public final String desc;// the score level

                private OfferType(int pos, String desc) {
                        this.pos = pos;
                        this.desc = desc;
                }

                public int getPos() {
			return pos;
                }

                public String getDesc() {
			return desc;
                }

        }

	public void setBase(int userId, int weight, int sign){
		_userId = userId;
		_weight = weight;
		_sign = sign;
	}
	
	public int getUserId(){
		return _userId;
	}
	public int getWeight(){
		return _weight;
	}
	
/*
	public Map<String, String> getInfos(){
		Map<String,String> res = new HashMap<String,String>();
		for( OfferType t : OfferType.values() ){
			String info = t.getTypeInfo( _infos );
			if( info!=null ){
				res.put(t.getDesc(),info);
			}
		}
		return res;
	}
*/
	
	public boolean isCommonFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_COMMONFRIEND.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	
	public boolean isSameWorkFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_SAMEINFO_WORKPLACE.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	public boolean isSameUnivFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_SAMEINFO_UNIV.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	public boolean isSameHighSchoolFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_SAMEINFO_HIGHSCHOOL.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	public boolean isSameJuniorSchoolFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_SAMEINFO_JUNIORSCHOOL.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	public boolean isSameElementorySchoolFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_SAMEINFO_ELEMENTARYSCHOOL.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	
	public boolean isSameIPFriend(){
		if( (_sign&(1<<OfferType.BASETYPE_IPSAME.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
	
	public boolean isSameAreaSameEnroll() {
		if( (_sign&(1<<OfferType.BASETYPE_SAMEAREASAMEENROLL.getPos())) != 0 ){
			return true;
		} 
		return false;
	}
}
