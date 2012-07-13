package mop.hi.oce.domain.model;

import xce.fptn.Footprint;
import xce.fptn.FootprintTiny;
import java.util.Date;

public class FootprintNewView {
	private Footprint footprint;

	public int getGuest() {
		return getFootprint().tinyinfo.guest;
	}

	public void setGuest(int guest) {
		this.getFootprint().tinyinfo.guest = guest;
	}

	public int getHost() {
		return getFootprint().host;
	}

	public void setHost(int host) {
		this.getFootprint().host = host;
	}

	public void setTime(Date fpttime) {
		this.getFootprint().tinyinfo.timestamp = (int)(fpttime.getTime()/1000L);	
	}

	public Date getTime() {
		Date fpttime = new Date((long)(this.getFootprint().tinyinfo.timestamp)*1000L);
		return fpttime;
	}

	public Footprint getFootprint() {
		if (footprint == null) {
			footprint = new Footprint();			
		}
		if ( footprint.tinyinfo ==null ) {
			footprint.tinyinfo = new FootprintTiny();
		}
		return footprint;
	}

	public void setFootprint(Footprint footprint) {
		this.footprint = footprint;
	}

	public String toString() {
		return "host = "+ this.footprint.host + "\n" + "guest = "+ this.footprint.tinyinfo.guest+"\n";
	}
}
