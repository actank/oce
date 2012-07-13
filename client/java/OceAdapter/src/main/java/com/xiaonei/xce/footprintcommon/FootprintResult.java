package com.xiaonei.xce.footprintcommon;

import java.util.ArrayList;
import java.util.List;

import xce.footprint.BaseStep;

import com.xiaonei.xce.footprintunite.FootprintView;

public class FootprintResult {
	private List<FootprintView> footprints = new ArrayList<FootprintView>();
	public FootprintResult(int owner, BaseStep[] steps) {
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