package com.xiaonei.xce.footprintunite;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.globalid.GlobalId;

import xce.footprint.FootprintInfo;
import xce.footprint.HomeStep;

public class HomeFootprintResult {
	
	private List<FootprintView> footprints = new ArrayList<FootprintView>();
	private int visitcount = 0;
	public HomeFootprintResult(int owner, HomeStep[] steps, int visitcount){
		this.visitcount = visitcount;
		for(HomeStep step:steps){
			FootprintView v = new FootprintView(owner,step.visitor,step.timestamp,new GlobalId(step.gid));
			this.footprints.add(v);
		}
	}
	
	public List<FootprintView> getFootprintViews(){
		return footprints;
	}
	
	public int getVisitCount() {
		return visitcount;
	}
}
