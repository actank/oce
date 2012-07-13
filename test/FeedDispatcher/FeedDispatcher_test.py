#! /usr/bin/env python
import sys, traceback, Ice, time, datetime
Ice.loadSlice("-I/opt/Ice/slice -I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice ../../OceSlice/slice/RFeed.ice")
from xce.feed import *
import unittest
status = 0
ic = None
ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300", "--Ice.Trace.Network=1"])
name = "FD@FeedDispatcher_new0"


if __name__ == '__main__':
    content = FeedContent()
    content.feed = 1
    content.source = 1
    content.actor = 61782
    content.type = 701
    content.xml="111111111111111111111111111111111"
    content.commentCount = 0
    content.time = 1234
    config = FeedConfig()
    config.itemSendConfig = True
    config.disableSendConfig = True
    config.expr = "f(61782)"
    config.desc = ""
    proxy = FeedDispatcherPrx.checkedCast(ic.stringToProxy(name))
    proxy.dispatch(content,config)
