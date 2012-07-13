package mop.hi.oce.domain.model;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import xce.util.tools.DateFormatter;


public class TribeAdmin {
	public static String ID = "ID";

	public static String TRIBE = "TRIBE";

	public static String USR = "USR";

	public static String DISC = "DISC";

	public static String TIME = "TIME";

	public int id;

	public int tribe;

	public int usr;

	public int disc;

	public Date time;

	public static TribeAdmin parse(Map<String, String> properties) {
		TribeAdmin result = new TribeAdmin();
		result.setId(Integer.parseInt(properties.get(TribeAdmin.ID)));
		result.setTribe(Integer.parseInt(properties.get(TribeAdmin.TRIBE)));
		result.setUsr(Integer.parseInt(properties.get(TribeAdmin.USR)));
		result.setDisc(Integer.parseInt(properties.get(TribeAdmin.DISC)));
		result.setTime(DateFormatter.parse(properties.get(TribeAdmin.TIME)));
		result.getProperties().clear();
		return result;
	}

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("TribeAdmin:\n");
		str.append("id = ").append(getId()).append("\n");
		str.append("tribe = ").append(getTribe()).append("\n");
		str.append("usr = ").append(getUsr()).append("\n");
		str.append("disc = ").append(getDisc()).append("\n");
		str.append("time = ").append(getTime()).append("\n");
		return str.toString();
	}

	private Map<String, String> updatedProperties;

	public Map<String, String> getProperties() {
		if (updatedProperties == null) {
			updatedProperties = new HashMap<String, String>();
		}
		return updatedProperties;
	}

	public int getDisc() {
		return disc;
	}

	public void setDisc(int disc) {
		this.disc = disc;
		getProperties().put(TribeAdmin.DISC, String.valueOf(getDisc()));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		getProperties().put(TribeAdmin.ID, String.valueOf(getId()));
	}

	public Date getTime() {
		return time;
	}

	public void setTime(Date time) {
		this.time = time;
		getProperties().put(TribeAdmin.TIME, DateFormatter.format(getTime()));
	}

	public int getTribe() {
		return tribe;
	}

	public void setTribe(int tribe) {
		this.tribe = tribe;
		getProperties().put(TribeAdmin.TRIBE, String.valueOf(getTribe()));
	}

	public int getUsr() {
		return usr;
	}

	public void setUsr(int usr) {
		this.usr = usr;
		getProperties().put(TribeAdmin.USR, String.valueOf(getUsr()));
	}

}
