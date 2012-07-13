package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/*
 * class NotifyContent{
	long id;		  //notifyID
	long source;  //用于合并删除
	int owner;		//回复页面的主人
	int from;		  //通知触发者

	int type;		  //通知类型, 最高位在内存中表示是否有压缩，压缩最高位为1, 低16位表示回复提示类型， 高15位为通知类型
	int time;
	string props;	//Key-value data
};
 */

public class NotifyBody {
	public static final String SOURCE = "source";
	public static final String OWNER = "owner";
	public static final String FROMID = "from";
	public static final String TOID = "to";
	public static final String TYPE = "type";
	public static final String TIME = "time";
	public static final String SCHEMAID = "schema_id";
	
	private Map<String,String> _props = new HashMap <String,String>();
	private List<Integer> _toids = new ArrayList<Integer>();
	
	public void setSource(long source){
		_props.put(SOURCE, String.valueOf(source));
	}
	public void setOwner(int owner){
		_props.put(OWNER, String.valueOf(owner));
	}
	public void setFromId(int fromId){
		_props.put(FROMID, String.valueOf(fromId));
	}
	/*
	 * 如果发给多个人，可在调用多次
	 */
	public void addToid(int toid){
		_toids.add(toid);
	}
	public void setType(int type){
		_props.put(TYPE, String.valueOf(type));
	}
	public void setTime(long time){
		_props.put(TIME, String.valueOf((int)(time/1000)));
	}
	public void setSchemaId(int schemaId){
		_props.put(SCHEMAID, String.valueOf(schemaId));
	}
	public void setValue(String key, String value){
		_props.put(key, value);
	}
	
	
	Map<String,String> getProps(){
		StringBuffer buf = new StringBuffer();
		for(int i = 0 ;i < _toids.size(); ++i){
			if(i>0){
				buf.append(',');
			}
			buf.append(_toids.get(i));
		}
		_props.remove(TOID);
		if(_toids.size() != 0){
			_props.put(TOID, buf.toString());
		}
		return _props;
	}
}
