#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../OceSlice/slice/BuddyCore.ice")
import MyUtil
import com.xiaonei.service.buddy
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"])
    manager = com.xiaonei.service.buddy.BuddyCoreManagerPrx.checkedCast(ic.stringToProxy("M@BuddyCore2"))
    print manager.getFriendSeq(246110642, 0, -1)

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

