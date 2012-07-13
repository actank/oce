package com.renren.xce.socialgraph.userfeature.async;

import java.util.ArrayList;
import java.util.List;

import xce.socialgraph.util.Clogging;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

/**
 * 任务
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class UserFeatureMessage implements Runnable {

    private static final int COLUMN_SIZE = 8;

    public static final String None = "None";
    public static final String ELE_SCHOOL = "ELE_SCHOOL";
    public static final String HIGH_SCHOOL = "HIGH_SCHOOL";
    public static final String UNIV = "UNIV";
    public static final String WORKPLACE = "WORKPLACE";
    public static final String BIRTH = "BIRTH";
    public static final String CITY = "CITY";
    public static final String GENDER = "GENDER";

    private static final String[] FEATURE_SEQ = new String[] { None,
	    ELE_SCHOOL, HIGH_SCHOOL, UNIV, WORKPLACE, BIRTH, CITY,
	    GENDER };

    private int userId;

    private List<IntStringTuple> featureList;

    /**
     * line format
     * Userid\telementary_school_info\thigh_school_info\tuniversity_info
     * \tworkplace_info\tuser_birth\tuser_city\tuser_gender <br/>
     * such as <br/>
     * 100018468 - -4.05 -1.30 - -1.51 -2.00 -0.67
     * 
     * @param line
     */
    public UserFeatureMessage(String line) {
	featureList = new ArrayList<IntStringTuple>();
	String[] splits = line.split("\t");
	if (splits.length == COLUMN_SIZE) {
	    try {
		userId = Integer.parseInt(splits[0]);
		if (userId < 1) {
		    throw new IllegalArgumentException("userid(" + userId
			    + " is invalid!");
		}
	    } catch (Exception e) {
		Clogging.Error(e.toString() + " line:" + line);
		return;
	    }
	    for (int i = 1; i < COLUMN_SIZE; ++i) {
		String v = splits[i];
		String featureStr = FEATURE_SEQ[i];
		try {
		    double d = Double.parseDouble(v);
		    int intValue = (int) (d * 100);
		    featureList.add(new IntStringTuple(intValue, featureStr));
		} catch (Exception e) {
		    continue;
		}
	    }
	}
    }

    public String toString() {
	StringBuffer sb = new StringBuffer();
	sb.append("userId:").append(userId).append("\t").append(
		featureList.toString());
	return sb.toString();
    }

    public static boolean isTarget(String target) {
	boolean flag = false;
	for (String s : FEATURE_SEQ) {
	    if (s.equals(target)) {
		flag = true;
	    }
	}
	return flag;
    }

    public void run() {
	if (featureList.isEmpty()) {
	    Clogging.Info("userId:" + userId + ", featureList is empty");
	    return;
	}

	Items.Builder itemsBuilder = Items.newBuilder();
	// 增量更新
	byte[] oldBin = BusinessCacheAdapter.getInstance().get(
		BusinessCacheAdapter.USERFEATURE, userId);
	if (oldBin != null && oldBin.length != 0) {
	    try {
		Items oldItems = Items.parseFrom(oldBin);
		for (Item oldItem : oldItems.getItemsList()) {
		    if (oldItem.hasMessage()) {
			String message = oldItem.getMessage();
			if (!isTarget(message)) {
			    itemsBuilder.addItems(oldItem);
			}
		    }
		}
	    } catch (InvalidProtocolBufferException e) {
		e.printStackTrace();
	    }
	}

	for (IntStringTuple tuple : featureList) {
	    Item.Builder itemBuilder = Item.newBuilder();
	    itemBuilder.setId(tuple.getIntValue());
	    itemBuilder.setMessage(tuple.getStrValue());
	    itemsBuilder.addItems(itemBuilder.build());
	}
	Items items = itemsBuilder.build();
	if (items.getSerializedSize() > 0) {
	    BusinessCacheAdapter.getInstance().set(
		    BusinessCacheAdapter.USERFEATURE, userId,
		    items.toByteArray());
	}
	StringBuffer sb = new StringBuffer();
	sb.append("userId:").append(userId).append("\t");
	sb.append("items.getItemsCount():").append(items.getItemsCount());
	Clogging.Info(sb.toString());
    }

    /**
     * 
     */
    private class IntStringTuple {
	private int intValue;
	private String strValue;

	public IntStringTuple(int intValue, String strValue) {
	    super();
	    this.intValue = intValue;
	    this.strValue = strValue;
	}

	public int getIntValue() {
	    return intValue;
	}

	public void setIntValue(int intValue) {
	    this.intValue = intValue;
	}

	public String getStrValue() {
	    return strValue;
	}

	public void setStrValue(String strValue) {
	    this.strValue = strValue;
	}

	public String toString() {
	    StringBuffer sb = new StringBuffer();
	    sb.append("intValue:").append(intValue).append("\t").append(
		    "strValue:").append(strValue);
	    return sb.toString();
	}
    }

}
