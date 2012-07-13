#! /usr/bin/env python
import sys, traceback, Ice
import datetime
import time
Ice.loadSlice("-I/opt/Ice-3.2/slice -I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/XFeed.ice")
import MyUtil
import xce.xfeed
status = 0
ic = None
try:
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XceFeedMaster -p 12000:default -h XceFeedReplica1 -p 12001:default -h XceFeedReplica2 -p 12002"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.22.200.205 -p 12000"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.3.19.74 -p 12000"])
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.3.17.119 -p 12000"])
    #manager = xce.xfeed.XFeedNewsManagerPrx.checkedCast(ic.stringToProxy("XFNM@XFeedNewsTmp"))
    ids = [long(sys.argv[1])]
    print ids
    #for i in range(0, 9):
    #    manager = xce.xfeed.XFeedContentManagerPrx.checkedCast(ic.stringToProxy("XFCM@XFeedContentN" + str(i)))
    #    print manager.getXFeedDict(ids)
    manager = xce.xfeed.XFeedContentManagerPrx.checkedCast(ic.stringToProxy("XFCM@XFeedContentN0"))
    for i in range(0, 99):
    	manager.removeXFeed(long(sys.argv[1]))
        time.sleep(1)
    print manager.getXFeedDict(ids)
    
except:
    traceback.print_exc()
    status = 1
if ic:
    try:
        ic.destroy()
    except:
        traceback.print_exc()
        status = 1
sys.exit(status)

