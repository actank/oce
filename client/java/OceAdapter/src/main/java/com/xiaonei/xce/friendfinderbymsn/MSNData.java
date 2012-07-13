package com.xiaonei.xce.friendfinderbymsn;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;
import java.util.Map.Entry;

import com.xiaonei.xce.log.Oce;


public class MSNData{
	private Map<Integer,List<Integer> > _puamap;
	private Map<Integer,List<Integer> > _pukmap;

	public Map<Integer,List<Integer> > getPUAmap() {
		return _puamap;
	}

	public Map<Integer,List<Integer> > getPUKmap() {
		return _pukmap;
	}

	public List<Integer> getPUAidsByType(int type) {
		return _puamap.get(type);
	}

	public List<Integer> getPUKidsByType(int type) {
		return _pukmap.get(type);
	}

	protected void setBase(xce.friendfinder.MSNdata base) {
                for( Entry<Integer, int[]> one : base.puamap.entrySet()){
                        int key = one.getKey();
                        List<Integer> value = new ArrayList<Integer>();
                        for( int two : one.getValue() ){
                                value.add(two);
                                Oce.getLogger().info(two);
                        }
                        _puamap.put(key, value);
                }
                for( Entry<Integer, int[]> one : base.pukmap.entrySet()){
                        int key = one.getKey();
                        List<Integer> value = new ArrayList<Integer>();
                        for( int two : one.getValue() ){
                                value.add(two);
                                Oce.getLogger().info(two);
                        }
                        _pukmap.put(key, value);
                }
	}
}
