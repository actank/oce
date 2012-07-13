package com.xiaonei.xce.userdanger;

import java.util.Map;

import com.xiaonei.xce.storm.XceStormListener;

public class UserDangerListener extends XceStormListener{

    /**
     * 
     */
    private static final long serialVersionUID = 1L;

    @Override
    public void handle(Map<String, String> context) {
	UserDangerCache.getInstance().change(context);
    }

}
