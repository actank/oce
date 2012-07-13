package com.xiaonei.xce.footprintunite;

import java.util.Date;

import com.xiaonei.xce.globalid.GlobalId;

import xce.footprint.FootprintInfo;
import xce.footprint.HomeInfo;
import xce.footprint.HomeStep;

public class FootprintView {

	private int owner;
	private int visitor;
	private int timestamp;
	private GlobalId gid;

	public FootprintView(int owner, int visitor, int timestamp, GlobalId gid) {
		this.owner = owner;
		this.visitor = visitor;
		this.gid = gid;
		this.timestamp = timestamp;
	}

	private FootprintInfo view;

	public int getVisitor() {
		return visitor;
	}

	public void setVisitor(int visitor) {
		this.visitor = visitor;
	}

	public int getOwner() {
		return owner;
	}

	public void setOwner(int owner) {
		this.owner = owner;
	}

	public FootprintInfo getFootprint() {
		if (view == null) {
			view = new FootprintInfo();
			view.owner = this.owner;
			view.visitor = this.visitor;
			view.timestamp = this.timestamp;
			if (this.gid == null)
				view.gid = 0;
			else
				view.gid = this.gid.toLong();
		}
		return view;
	}

	public void setSource(GlobalId gid) {
		this.gid = gid;
	}

	public GlobalId getSource() {
		return gid;
	}

	public Date getTime() {
		Date fpttime = new Date((long) (this.timestamp) * 1000L);
		return fpttime;
	}
}
