package mop.hi.oce.domain.model;

import java.util.Date;

import mop.hi.svc.fpt.Footprint;

public class FootprintView {
	private Footprint footprint;

	public int getGuest() {
		return getFootprint().guest;
	}

	public void setGuest(int guest) {
		this.getFootprint().guest = guest;
	}

	public String getGuestName() {
		return getFootprint().guestName;
	}

	public void setGuestName(String guestName) {
		this.getFootprint().guestName = guestName;
	}

	public String getGuestUrl() {
		return getFootprint().guestHead;
	}

	public void setGuestUrl(String guestUrl) {
		this.getFootprint().guestHead = guestUrl;
	}
	
	public long getHost() {
		return getFootprint().host;
	}

	public int getHostS() {
		Long host = getFootprint().host;
		return host.intValue();
	}

	public void setHost(int host) {
		this.getFootprint().host = host;
	}
	
	public void setHost(long host) {
		this.getFootprint().host = host;
	}

	public Date getTime() {
		return new Date(getFootprint().time);
	}

	public void setTime(Date time) {
		this.getFootprint().time = time.getTime();
	}

	public Footprint getFootprint() {
		if (footprint == null) {
			footprint = new Footprint();
		}
		return footprint;
	}

	public void setFootprint(Footprint footprint) {
		this.footprint = footprint;
	}

	public String toString() {
		return "id = " + this.footprint.id + "\n" + "time = "
				+ new Date(this.footprint.time) + "\n" + "host = "
				+ this.footprint.host + "\n" + "guest = "
				+ this.footprint.guest+"\n"+this.footprint.guestHead + "\n";
	}
}
