package mop.hi.oce.domain.model;

import java.util.List;
import java.util.ArrayList;
import xce.fptn.FootprintInfo;
import xce.fptn.Footprint;
import xce.fptn.FootprintTiny;
import mop.hi.oce.domain.model.FootprintNewView;

public class FootprintResult {
	private List<FootprintNewView> fptviews = new ArrayList<FootprintNewView>();
	private int visitcount = 0;	
	
	public FootprintResult(int hostId, FootprintTiny[] fptns, int visitcount) {	
		
		for (int i = 0; i < fptns.length; ++i) {
			FootprintNewView v = new FootprintNewView();
			Footprint footprint = new Footprint();
			footprint.tinyinfo = new FootprintTiny();
			footprint.tinyinfo.guest = fptns[i].guest;
			footprint.tinyinfo.timestamp = fptns[i].timestamp;
			footprint.host=hostId;
			v.setFootprint(footprint);
			this.fptviews.add(v);
		}		
		this.visitcount = visitcount;
	}

	public List<FootprintNewView> getFootprintViews() {
		return fptviews;
	}

	public int getVisitCount() {
		return visitcount;
	}
}
