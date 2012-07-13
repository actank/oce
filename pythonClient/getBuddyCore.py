#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/BuddyCore.ice")
import MyUtil
import com.xiaonei.service
import com.xiaonei.service.buddy
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    #ic = Ice.initialize(sys.argv)
    proxy = []
    #for i in range(0, 10):
    #print "Initializing proxy for ending with", i
    proxy.append( com.xiaonei.service.buddy.BuddyCoreManagerPrx.checkedCast(ic.stringToProxy("M@BuddyCore6")) )
    #uid=int(200353733)
    #users[uid % 10].reloadUser(200353733)
    f = file("getBuddyCore.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            eid=0#uid % 10
            print eid, uid
            print proxy[eid].getFriendSeq(uid, 0, -1);
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

