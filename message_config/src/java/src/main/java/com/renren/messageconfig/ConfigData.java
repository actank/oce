package com.renren.messageconfig;

import java.util.ArrayList;
import java.util.List;

/**
 * 配置信息
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class ConfigData {

    static final String DEFAULT_VERSION = "";

    private String name; // znode path
    private String version; // znode value
    private List<ConfigData> children;

    public ConfigData(String name, String version) {
	this.name = name;
	this.version = version;
	this.children = new ArrayList<ConfigData>();
    }

    public List<String> getChildrenNames() {
	List<String> list = new ArrayList<String>();
	for (ConfigData msg : children) {
	    list.add(msg.getName());
	}
	return list;
    }

    public void addChild(String name, String v) {
	ConfigData msg = new ConfigData(name, v);
	children.add(msg);
    }

    public String getName() {
	return name;
    }

    public void setName(String name) {
	this.name = name;
    }

    public String getVersion() {
	return version;
    }

    public void setVersion(String version) {
	this.version = version;
    }

    public String toString() {
	StringBuffer sb = new StringBuffer();
	sb.append("ConfigData name:").append(name).append("\t");
	sb.append("version:").append(version).append("\t");
	sb.append("children names:").append(getChildrenNames());
	return sb.toString();
    }

}
