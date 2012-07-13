package mop.hi.oce.domain.model;

import com.xiaonei.xce.log.Oce;
import java.io.Serializable;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import xce.util.tools.DateFormatter;


public class TribeMember implements Serializable {
	public static String ID = "ID";

	public static String TRIBE = "TRIBE";

	public static String MEMBER = "MEMBER";

	public static String ADMIN = "ADMIN";

	public static String FLAG = "FLAG";

	public static String DISC = "DISC";

	public static String TIME = "TIME";

	public static String SELECTED = "SELECTED";

	public int id;

	public int tribe;

	public int member;

	public int admin;

	public int flag;

	public int disc;

	public Date time;

	public int selected;

	public static TribeMember parse(Map<String, String> properties) {
		TribeMember result = new TribeMember();
		try {
			result.setId(Integer.parseInt(properties.get(TribeMember.ID)));
		} catch (Exception e) {
			Oce.getLogger().error("mop.hi.oce.domain.model.TribeMember" + " " + e);
		}
		try {
			result
					.setTribe(Integer.parseInt(properties
							.get(TribeMember.TRIBE)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result.setMember(Integer.parseInt(properties
					.get(TribeMember.MEMBER)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result
					.setAdmin(Integer.parseInt(properties
							.get(TribeMember.ADMIN)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result.setFlag(Integer.parseInt(properties.get(TribeMember.FLAG)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result.setDisc(Integer.parseInt(properties.get(TribeMember.DISC)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result.setTime(DateFormatter
					.parse(properties.get(TribeMember.TIME)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		try {
			result.setSelected(Integer.parseInt(properties
					.get(TribeMember.SELECTED)));
		} catch (Exception e) {
			// Oce.getLogger().error(this.getClass().getName() + e);
		}
		return result;
	}

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("TribeMember:\n");
		str.append("id = ").append(getId()).append("\n");
		str.append("tribe = ").append(getTribe()).append("\n");
		str.append("member = ").append(getMember()).append("\n");
		str.append("admin = ").append(getAdmin()).append("\n");
		str.append("flag = ").append(getFlag()).append("\n");
		str.append("disc = ").append(getDisc()).append("\n");
		str.append("time = ").append(getTime()).append("\n");
		str.append("selected = ").append(getSelected()).append("\n");
		return (str.toString());
	}

	private Map<String, String> properties;

	public Map<String, String> getProperties() {
		if (properties == null) {
			properties = new HashMap<String, String>();
		}
		return properties;
	}

	public int getAdmin() {
		return admin;
	}

	public void setAdmin(int admin) {
		this.admin = admin;
		getProperties().put(TribeMember.ADMIN, String.valueOf(admin));
	}

	public int getDisc() {
		return disc;
	}

	public void setDisc(int disc) {
		this.disc = disc;
		getProperties().put(TribeMember.DISC, String.valueOf(disc));
	}

	public int getFlag() {
		return flag;
	}

	public void setFlag(int flag) {
		this.flag = flag;
		getProperties().put(TribeMember.FLAG, String.valueOf(flag));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		// id cannot be set in database;
		// getProperties().put(TribeMember.ID, String.valueOf(getId()));
	}

	public int getMember() {
		return member;
	}

	public void setMember(int member) {
		this.member = member;
		getProperties().put(TribeMember.MEMBER, String.valueOf(member));
	}

	public int getSelected() {
		return selected;
	}

	public void setSelected(int selected) {
		this.selected = selected;
		getProperties().put(TribeMember.SELECTED, String.valueOf(selected));
	}

	public Date getTime() {
		return time;
	}

	public void setTime(Date time) {
		this.time = time;
		getProperties().put(TribeMember.TIME, DateFormatter.format(time));
	}

	public int getTribe() {
		return tribe;
	}

	public void setTribe(int tribe) {
		this.tribe = tribe;
		getProperties().put(TribeMember.TRIBE, String.valueOf(tribe));
	}

}
