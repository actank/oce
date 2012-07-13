import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/User.ice ../OceSlice/slice/Dispatch.ice")
import MyUtil
import mop.hi.svc.model
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    obj0r = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyRpl0"))
    obj9r = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyRpl9"))
    obj0p = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyPnl0"))
    obj9p = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyPnl9"))
    obj0g = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyGpl0"))
    obj9g = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyGpl9"))
    #obj3 = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyRpl3"))
    #obj6 = mop.hi.svc.dispatch.NotifierPrx.checkedCast(ic.stringToProxy("N@NotifyRpl6"))
    obj0r.setAllRead(143812810);
    obj9r.setAllRead(175616229);
    obj9r.setAllRead(230959489);
    obj0p.setAllRead(143812810);
    obj9p.setAllRead(175616229);
    obj9p.setAllRead(230959489);
    obj0g.setAllRead(143812810);
    obj9g.setAllRead(175616229);
    obj9g.setAllRead(230959489);

    #obj3.setAllRead(221150043);
    #obj6.setAllRead(222685816);

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

