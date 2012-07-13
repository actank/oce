package com.xiaonei.xce.test.xfeeddispatcher;

import com.xiaonei.xce.xfeed.XFeedContent;
import com.xiaonei.xce.xfeed.XFeedFactory;

public class XFeedTestFactory implements XFeedFactory {

	public XFeedContent create() {
		return new XFeedContent();
	}

}
