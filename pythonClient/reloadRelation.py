#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/Relation.ice")
import MyUtil
import mop.hi.svc.relation
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    #ic = Ice.initialize(sys.argv)
    relations = []
    #config = []
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        relations.append( mop.hi.svc.relation.InternalManagerPrx.checkedCast(ic.stringToProxy("IM@Relation" + str(i))) )
    f = file("reloadRelation.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            eid=uid % 10
            print eid, uid
            relations[eid].reloadRequest(uid);
            relations[eid].reloadBlock(uid);
            relations[eid].reloadBlocked(uid);
            relations[eid].reloadBuddy(uid);
            relations[eid].reloadRelationGroup(uid);
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

