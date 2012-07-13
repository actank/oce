package com.xiaonei.xce.minigroup;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.mngp.MiniGroup;

public class MiniGroupInfo {
  private long id = 0;
  private String name = "";
  private String icon = "";
  private final Map<String, String> extraInfo = new HashMap<String, String>();
  
  /***
   * 如果name或者icon中任何一个为null, 那么构造函数会构造出一个默认的MiniGroupInfo [id = 0, name = "", icon = ""]
   * @param id
   * @param name
   * @param icon
   */
  public MiniGroupInfo(long id, String name, String icon) {
    if (name != null && icon != null) {
      this.id = id;
      this.name = name;
      this.icon = icon;
    }
  }
  
  public MiniGroupInfo(MiniGroup miniGroup) {
    if (miniGroup != null && miniGroup.name != null && miniGroup.icon != null) {
      this.id = miniGroup.id;
      this.name = miniGroup.name;
      this.icon = miniGroup.icon;
      if (miniGroup.extraInfo != null) {
        for (Entry<String, String> entry : miniGroup.extraInfo.entrySet()) {
          this.extraInfo.put(entry.getKey(), entry.getValue());
        }
      }
    }
  }
  
  public long getId() {
    return id;
  }
  
  public String getName() {
    return name;
  }
  
  public String getIcon() {
    return icon;
  }
  
  public boolean containsKey(String key) {
    return extraInfo.containsKey(key);
  }
  
  public String getValue(String key) {
    return extraInfo.get(key);
  }
  
  public void addKeyValue(String key, String value) {
    extraInfo.put(key, value);
  }
  
  public void deleteKey(String key) {
    extraInfo.remove(key);
  }
  
  public Map<String, String> getExtraInfo() {
    return new HashMap<String, String>(extraInfo);
  }
  
  @Override
  public String toString() {
    return "MiniGroupInfo { id =  " + id + ", name = " + name + ", icon = " + icon + " extraInfo = "
            + extraInfo + " }";
  }
  
  @Override
  public boolean equals(Object obj) {
    if (obj == null || !(obj instanceof MiniGroupInfo)) {
      return false;
    } else {
      return toString().equals(obj.toString());
    }
  }
  
  public static List<MiniGroupInfo> formMiniGroupInfos(MiniGroup[] miniGroups) {
    List<MiniGroupInfo> results = new ArrayList<MiniGroupInfo>();
    if (miniGroups != null) {
      for (MiniGroup tempMiniGroup : miniGroups) {
        results.add(new MiniGroupInfo(tempMiniGroup));
      }
    } else {
      System.out.println("miniGroups from server is null");
    }
    return results;
  }
  
  private static MiniGroup fromMiniGroup(MiniGroupInfo miniGroupInfo) {
    MiniGroup result = new MiniGroup();
    result.extraInfo = new HashMap<String, String>();
    if (miniGroupInfo == null || miniGroupInfo.name == null || miniGroupInfo.icon == null) {
      result.id = 0;
      result.name = "";
      result.icon = "";
    } else {
      result.id = miniGroupInfo.id;
      result.name = miniGroupInfo.name;
      result.icon = miniGroupInfo.icon;
      result.extraInfo.putAll(miniGroupInfo.getExtraInfo());
    }
    return result;
  }
  
  public static MiniGroup[] formMiniGroups(List<MiniGroupInfo> miniGroupInfos) {
    if (miniGroupInfos == null || miniGroupInfos.isEmpty()) {
      return new MiniGroup[0];
    }
    MiniGroup[] result = new MiniGroup[miniGroupInfos.size()];
    for (int i = 0; i < result.length; ++i) {
      result[i] = fromMiniGroup(miniGroupInfos.get(i));
    }
    return result;
  }
}
