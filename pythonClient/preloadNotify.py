import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/User.ice ../OceSlice/slice/Dispatch.ice")
import MyUtil
import mop.hi.svc.dispatch
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])

    rpls = []
    pnls = []
    gpls = []
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        rpls.append( mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyRpl" + str(i)).ice_timeout(10)) )
        pnls.append( mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyPnl" + str(i)).ice_timeout(10)) )
        gpls.append( mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyGpl" + str(i)).ice_timeout(10)) )
    f = file("preloadNotify.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            eid=uid % 10
            #print eid, uid
            try:
                #rpls[eid].getCount(uid, -1, True);
                pnls[eid].getCount(uid, -1, True);
                #gpls[eid].getCount(uid, -1, True);
            except:
                print eid, uid
                traceback.print_exc()
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

