#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../OceSlice/slice/BuddyCache.ice")
import MyUtil
import xce.buddy
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"])
    manager = xce.buddy.BuddyCacheManagerPrx.checkedCast(ic.stringToProxy("M@BuddyCache1"))
    print manager.getFriendList(246110642)

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

