package com.xiaonei.xce.detector;

public class DetectorEntry implements Comparable<DetectorEntry> {
    private int _count;
    private long _time;
    private String _name;
    private String _description;
    
    public DetectorEntry(String name, int count, long time, String description){
	_name = name;
	_count = count;
	_time = time;
	_description = description;
    }
    
    public void setTime(long time){
	_time = time;
    }
    
    public String getName(){
	return _name;
    }
    
    public String getDescription(){
	return _description;
    }
    
    public int getCount() {
	return _count;
    }
    
    public long getTime(){
	return _time;
    }
    
    public void incCount(int count) {
	_count +=count;
    }

    @Override
    public int compareTo(DetectorEntry e) {
	if(_time == e._time) {
	    return 0;
	} else {
	    return _time < e._time ? -1 : 1;
	}
    }
}
