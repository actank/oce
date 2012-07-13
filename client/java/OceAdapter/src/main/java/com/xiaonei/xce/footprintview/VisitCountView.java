package com.xiaonei.xce.footprintview;

import java.util.Date;

import xce.footprint.VisitCountInfo;


public class VisitCountView {
    private int timestamp;
    private int count;
    private int state;
    
    VisitCountView(VisitCountInfo info) {
        this.timestamp = info.timestamp;
        this.count = info.count;
        this.state = info.state;
    }
    
    public Date getTime() {
        Date fpttime = new Date((long) (this.timestamp) * 1000L);
        return fpttime;
    }
    
    public int getCount() {
        return this.count;
    }
    
    public boolean isLimit() {
        return this.state == xce.footprint.STATELIMIT.value ? true:false;
    }
    
    public boolean isSpammer() {
        return this.state == xce.footprint.STATESPAMMER.value ? true:false;
    }
}
