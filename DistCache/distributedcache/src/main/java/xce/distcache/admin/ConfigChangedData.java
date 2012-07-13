package xce.distcache.admin;

import java.util.List;

public class ConfigChangedData {
    private List<ConfigData> deletedatas;
    private List<ConfigData> adddatas;
    private List<ConfigData> updatedatas;

    public ConfigChangedData(List<ConfigData> deletedatas,
	    List<ConfigData> adddatas, List<ConfigData> updatedatas) {
	this.deletedatas = deletedatas;
	this.adddatas = adddatas;
	this.updatedatas = updatedatas;
    }

    public List<ConfigData> getDeletedatas() {
	return deletedatas;
    }

    public List<ConfigData> getAdddatas() {
	return adddatas;
    }

    public List<ConfigData> getUpdatedatas() {
	return updatedatas;
    }
}
