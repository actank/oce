#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/CountCache.ice")
import MyUtil
import mop.hi.svc.count
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    #ic = Ice.initialize(sys.argv)
    endpoints = []
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        endpoints.append(mop.hi.svc.count.CountManagerPrx.checkedCast(ic.stringToProxy("MANAGER@CountCache" + str(i))))
    f = file("reloadCountCache.txt")
    while True:        
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid = int(i)
            eid = uid % 10
            print eid, uid
            endpoints[eid].reload(uid);
            endpoints[eid].getAll(uid);
    f.close()

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

