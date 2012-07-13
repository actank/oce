#! /usr/bin/env python
import sys, traceback, Ice
import time
import string
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all  -I$ICE_HOME/slice ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/KeySearch.ice")
import MyUtil
import demo
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:default -h 10.22.206.34 -p 14300"])
    serviceName = "M@KeySearch" + "";
    obj = demo.KeySearchManagerPrx.checkedCast(ic.stringToProxy(serviceName))
    obj.insert("FirstKey","insert key FirstKey first time")
    a = obj.lookup("FirstKey")
    print a
    obj.insert("FirstKey","insert key FirstKey second time")
    a = obj.lookup("FirstKey")
    print a
    obj.remove("FirstKey")
    a = obj.lookup("FirstKey")
    print a
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
