package mop.hi.oce.domain.model;

import java.util.Date;

import mop.hi.svc.gf.ADDAFRIEND;
import mop.hi.svc.gf.DIARYANDREPLY;
import mop.hi.svc.gf.FOOTPRINT;
import mop.hi.svc.gf.GREET;
import mop.hi.svc.gf.GeneralInformation;
import mop.hi.svc.gf.INVITATION;
import mop.hi.svc.gf.JOINAGROUP;
import mop.hi.svc.gf.LETTER;
import mop.hi.svc.gf.MESSAGE;
//import mop.hi.svc.gf.GeneralInformation;
/* GeneralVisitInformation has 7 fields, but only three of them are necessay: type, host ,guest
 * other fields may be used in the future
 */
public class GeneralVisitInformation implements FOOTPRINT,MESSAGE,LETTER,
DIARYANDREPLY,ADDAFRIEND,JOINAGROUP,GREET,INVITATION{
	private GeneralInformation information;
	public static int footprint = FOOTPRINT.value;
	public static int message   = MESSAGE.value;
	public static int letter    = LETTER.value;
	public static int diaryAndreply = DIARYANDREPLY.value;
	public static int addAFriend= ADDAFRIEND.value;
	public static int joinAGroup= JOINAGROUP.value;
	public static int greet     = GREET.value;
	public static int invitation= INVITATION.value;
	
	public GeneralInformation getGeneralInformation() {
		if (information == null) {
			information = new GeneralInformation();
		}
		return information;
	}
	public int getGuest() {
		return getGeneralInformation().guest;
	}
	public int getType(){
		return getGeneralInformation().type;
	}
	
	public void setType(int type){
		this.getGeneralInformation().type = type;
	}
	public void setGuest(int guest) {
		this.getGeneralInformation().guest = guest;
	}

	public String getGuestName() {
		return getGeneralInformation().guestName;
	}

	public void setGuestName(String guestName) {
		this.getGeneralInformation().guestName = guestName;
	}


	public int getHost() {
		return getGeneralInformation().host;
	}

	public void setHost(int host) {
		this.getGeneralInformation().host = host;
	}

	public Date getTime() {
		return new Date(getGeneralInformation().time);
	}

	public void setTime(Date time) {
		this.getGeneralInformation().time = time.getTime();
	}

	

	public void setGeneralInformation(GeneralInformation info) {
		this.information = info;
	}

	public String toString() {
		return "id = " + this.information.id + "\n" + "time = "
				+ new Date(this.information.time) + "\n" + "host = "
				+ this.information.host + "\n" + "guest = "
				+ this.information.guest + "\n" + "type = "
				+ this.information.type + "\n";
	}
}