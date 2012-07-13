#! /usr/bin/env python
import sys, traceback, Ice
import datetime
import time
Ice.loadSlice("-I/opt/Ice-3.2/slice -I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/XFeed.ice ../UtilSlice/slice/TaskManagerAPI.ice" )
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
    while True:
        for i in range(0, 10):
            manager = xce.svcapi.TaskManagerAPIPrx.checkedCast(ic.stringToProxy("TMA@XFeedDispatcherN" + str(i)))
            #manager.config(-6, 2, 500, 131072)
            size = manager.size(5)
            print "Initializing proxy for ending with", i, size
            #if size > 0:
            #    manager.clear(-6)
        time.sleep(2)

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

