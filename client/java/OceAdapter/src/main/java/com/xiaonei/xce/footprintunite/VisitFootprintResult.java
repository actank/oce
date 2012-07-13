package com.xiaonei.xce.footprintunite;

import java.util.ArrayList;
import java.util.List;
import xce.footprint.BaseStep;

public class VisitFootprintResult {

	private List<FootprintView> footprints = new ArrayList<FootprintView>();
	private int visitcount = 0;

	public VisitFootprintResult(int owner, BaseStep[] steps) {
		for (BaseStep step : steps) {
			FootprintView v = new FootprintView(owner, step.visitor,
					step.timestamp, null);
			this.footprints.add(v);
		}
	}

	public List<FootprintView> getFootprintViews() {
		return footprints;
	}
}
