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
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.3.17.119 -p 12000"])
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.3.19.74 -p 12000"])
    #manager = xce.xfeed.XFeedNewsManagerPrx.checkedCast(ic.stringToProxy("XFNM@XFeedNewsTmp"))
    manager = xce.xfeed.XFeedMiniManagerPrx.checkedCast(ic.stringToProxy("XFMM@XFeedMini21"))
    print manager.getXFeedSeqByType(242332421, [5], False, 0, -1)
    """
    while True:
        t1 = datetime.datetime.now()
        result = manager.getXFeedSeqByType(80578957, [], True, 0, -1)
	#print result
        t2 = datetime.datetime.now()
        t = t2 - t1
        print t
        time.sleep(1)
    """
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

