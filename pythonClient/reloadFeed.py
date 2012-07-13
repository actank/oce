#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I/opt/Ice-3.2/slice -I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/Feed.ice")
import MyUtil
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    #ic = Ice.initialize(sys.argv)
    bodys = []
    minis = []
    for i in range(0, 5):
        print "Initializing proxy for ending with", i
        bodys.append( MyUtil.ObjectCachePrx.checkedCast(ic.stringToProxy("SC@FeedBody" + str(i))) )
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        minis.append( MyUtil.ObjectCachePrx.checkedCast(ic.stringToProxy("SC@FeedMini" + str(i))) )
    f = file("reloadFeed.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            eid_b=uid % 5
            eid_m=uid % 10
            print eid_b, eid_m, uid
            try:
                bodys[eid_b].removeObject(0, uid)
                minis[eid_m].removeObject(0, uid)
            except:
                traceback.print_exc();
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

