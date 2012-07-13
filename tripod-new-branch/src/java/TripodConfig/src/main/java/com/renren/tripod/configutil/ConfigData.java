package com.renren.tripod.configutil;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class ConfigData implements Cloneable {

	public static String TRIPOD_MANAGER = "Manager";
	public static String TRIPOD_PRODUCER = "Producer";
	public static String TRIPOD_CACHE = "Cache";
	public static String TRIPOD_PRODUCER_CC = "cc";
	public static String TRIPOD_PRODUCER_JAVA = "java";

	public String _name = "";
	public String _value = "";
	public String _version = "";

	public Map<String, ConfigData> _childrenDatas = new HashMap<String, ConfigData>();

	public String toString() {
		StringBuffer buff = new StringBuffer();
		buff.append("\nNAME: " + _name);
		buff.append("\nVALUE: " + _value);
		buff.append("\nVERSION: " + _version);
		for (Entry<String, ConfigData> cd : _childrenDatas.entrySet()) {
			buff.append("   " + cd.getKey().toString() + "\n");
		}
		return buff.toString();
	}

	public ConfigData clone() {
		ConfigData newData = new ConfigData();
		newData._name = _name;
		newData._value = _value;
		newData._version = _version;
		for (Entry<String, ConfigData> ent : _childrenDatas.entrySet()) {
			newData._childrenDatas.put(ent.getKey(), ent.getValue().clone());
		}
		return newData;
	}
}
