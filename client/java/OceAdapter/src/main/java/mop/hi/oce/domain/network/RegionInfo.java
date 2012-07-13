package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;

public class RegionInfo extends Updatable {
	public static final String ID = "ID";

	public static final String USERID = "USERID";

	public static final String REGION_ID = "REGION_ID";

	public static final String PROVINCE_NAME = "PROVINCE_NAME";

	public static final String CITY_NAME = "CITY_NAME";

	protected mop.hi.svc.model.RegionInfo info = new mop.hi.svc.model.RegionInfo();

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("id:").append(getId()).append("\n");
		buf.append("getUserId:").append(getUserId()).append("\n");
		buf.append("getRegionId:").append(getRegionId()).append("\n");
		buf.append("getProvinceName:").append(getProvinceName()).append("\n");
		buf.append("getCityName:").append(getCityName()).append("\n");
		// System.out.println(buf.toString());
		return buf.toString();
	}

	public mop.hi.svc.model.RegionInfo get() {
		return info;
	}

	public void set(mop.hi.svc.model.RegionInfo info) {
		this.info = info;
	}

	public int getId() {
		return info.id;
	}

	public void setId(int id) {
		this.getUpdates().put(RegionInfo.ID, String.valueOf(id));
		info.id = id;
	}

	public int getUserId() {
		return info.userId;
	}

	public void setUserId(int userId) {
		this.getUpdates().put(RegionInfo.USERID, String.valueOf(userId));
		info.userId = userId;
	}
	
	public int getRegionId() {
		return info.regionId;
	}

	public void setRegionId(int regionId) {
		this.getUpdates().put(RegionInfo.REGION_ID,
				String.valueOf(regionId));
		info.regionId = regionId;
	}
	
	public String getProvinceName() {
		return info.provinceName;
	}

	public void setProvinceName(String provinceName) {
		this.getUpdates().put(RegionInfo.PROVINCE_NAME, provinceName);
		info.provinceName = provinceName;
	}

	public String getCityName() {
		return info.cityName;
	}

	public void setCityName(String cityName) {
		this.getUpdates().put(RegionInfo.CITY_NAME, cityName);
		info.cityName = cityName;
	}

}
