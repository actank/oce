package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;

public class WorkplaceInfo extends Updatable {
	public static final String ID = "ID";

	public static final String USERID = "USERID";

	public static final String WORKPLACE_ID = "WORKPLACE_ID";

	public static final String WORKPLACE_NAME = "WORKPLACE_NAME";

	public static final String JOIN_MONTH = "JOIN_MONTH";

	public static final String JOIN_YEAR = "JOIN_YEAR";

	public static final String ADDRESS = "ADDRESS";

	public static final String DESCRIPTION = "DESCRIPTION";

	public static final String PROVINCE = "PROVINCE";

	public static final String CITY_ID = "CITY_ID";

	public static final String CITY_NAME = "CITY_NAME";

	public static final String QUIT_YEAR = "QUIT_YEAR";

	public static final String QUIT_MONTH = "QUIT_MONTH";

	public static final String TYPE = "TYPE";

	public static final String JOB_TITLE_ID = "JOB_TITLE_ID";
	
	public static final String POSITION_ID = "POSITION_ID";
	
	protected mop.hi.svc.model.WorkspaceInfo info = new mop.hi.svc.model.WorkspaceInfo();

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append(ID).append("=").append(String.valueOf(getId())).append("\n");
		str.append(USERID).append("=").append(String.valueOf(getUserId())).append("\n");
		str.append(WORKPLACE_ID).append("=").append(String.valueOf(getWorkplaceId())).append("\n");
		str.append(WORKPLACE_NAME).append("=").append(String.valueOf(getWorkplaceName())).append("\n");
		str.append(JOIN_MONTH).append("=").append(String.valueOf(getJoinMonth())).append("\n");
		str.append(JOIN_YEAR).append("=").append(String.valueOf(getJoinYear())).append("\n");
		str.append(ADDRESS).append("=").append(String.valueOf(getAddress())).append("\n");
		str.append(DESCRIPTION).append("=").append(String.valueOf(getDescription())).append("\n");
		str.append(PROVINCE).append("=").append(String.valueOf(getProvince())).append("\n");
		str.append(CITY_ID).append("=").append(String.valueOf(getCityId())).append("\n");
		str.append(CITY_NAME).append("=").append(String.valueOf(getCityName())).append("\n");
		str.append(QUIT_MONTH).append("=").append(String.valueOf(getQuitMonth())).append("\n");
		str.append(QUIT_YEAR).append("=").append(String.valueOf(getQuitYear())).append("\n");
		str.append(TYPE).append("=").append(String.valueOf(getType())).append("\n");
		str.append(JOB_TITLE_ID).append("=").append(String.valueOf(getJobTitleId())).append("\n");
		str.append(POSITION_ID).append("=").append(String.valueOf(getPositionId())).append("\n");
		return str.toString();
	}

	public mop.hi.svc.model.WorkspaceInfo get() {
		return info;
	}

	public void set(mop.hi.svc.model.WorkspaceInfo info) {
		this.info = info;
	}

	public int getId() {
		return info.id;
	}

	public void setId(int id) {
		this.getUpdates().put(WorkplaceInfo.ID, String.valueOf(id));
		info.id = id;
	}

	public int getJoinMonth() {
		return info.joinMonth;
	}

	public void setJoinMonth(int joinMonth) {
		this.getUpdates().put(WorkplaceInfo.JOIN_MONTH, String.valueOf(joinMonth));
		info.joinMonth = joinMonth;
	}

	public int getJoinYear() {
		return info.joinYear;
	}

	public void setJoinYear(int joinYear) {
		this.getUpdates().put(WorkplaceInfo.JOIN_YEAR, String.valueOf(joinYear));
		info.joinYear = joinYear;
	}

	public String getAddress() {
		return info.address;
	}

	public void setAddress(String address) {
		this.getUpdates().put(WorkplaceInfo.ADDRESS, address);
		info.address = address;
	}

	public String getDescription() {
		return info.description;
	}

	public void setDescription(String description) {
		this.getUpdates().put(WorkplaceInfo.DESCRIPTION, description);
		info.description = description;
	}

	public int getWorkplaceId() {
		return info.workplaceId;
	}

	public void setWorkplaceId(int workplaceId) {
		this.getUpdates().put(WorkplaceInfo.WORKPLACE_ID, String.valueOf(workplaceId));
		info.workplaceId = workplaceId;
	}

	public String getWorkplaceName() {
		return info.workplaceName;
	}

	public void setWorkplaceName(String workplaceName) {
		this.getUpdates().put(WorkplaceInfo.WORKPLACE_NAME, workplaceName);
		info.workplaceName = workplaceName;
	}

	public int getUserId() {
		return info.userId;

	}

	public void setUserId(int userId) {
		this.getUpdates().put(WorkplaceInfo.USERID, String.valueOf(userId));
		info.userId = userId;
	}

	public String getProvince() {
		return info.province;

	}

	public void setProvince(String province) {
		this.getUpdates().put(WorkplaceInfo.PROVINCE, province);
		info.province = province;
	}

	public int getCityId() {
		return info.cityId;

	}

	public void setCityId(int cityId) {
		this.getUpdates().put(WorkplaceInfo.CITY_ID, String.valueOf(cityId));
		info.cityId = cityId;
	}

	public String getCityName() {
		return info.cityName;

	}

	public void setCityName(String name) {
		this.getUpdates().put(WorkplaceInfo.CITY_NAME, name);
		info.cityName = name;
	}

	public int getQuitYear() {
		return info.quitYear;

	}

	public void setQuitYear(int year) {
		this.getUpdates().put(WorkplaceInfo.QUIT_YEAR, String.valueOf(year));
		info.quitYear = year;
	}

	public int getQuitMonth() {
		return info.quitMonth;

	}

	public void setQuitMonth(int month) {
		this.getUpdates().put(WorkplaceInfo.QUIT_MONTH, String.valueOf(month));
		info.quitMonth = month;
	}

	public int getType() {
		return info.type;

	}

	public void setType(int type) {
		this.getUpdates().put(WorkplaceInfo.TYPE, String.valueOf(type));
		info.type = type;
	}
	
	//public static final String INDUSTRY_ID = "INDUSTRY_ID";
	public int getJobTitleId() {
		return info.jobTitleId;

	}

	public void setJobTitleId(int jobTitleId) {
		this.getUpdates().put(WorkplaceInfo.JOB_TITLE_ID, String.valueOf(jobTitleId));
		info.jobTitleId = jobTitleId;
	}
	
	//public static final String POSITON_ID = "POSITON_ID";
	public int getPositionId() {
		return info.positionId;

	}

	public void setPositionId(int positionId) {
		this.getUpdates().put(WorkplaceInfo.POSITION_ID, String.valueOf(positionId));
		info.positionId = positionId;
	}
	

}
