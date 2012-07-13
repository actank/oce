import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/User.ice ../OceSlice/slice/Dispatch.ice")
import MyUtil
import mop.hi.svc.model
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    obj = mop.hi.svc.dispatch.DispatcherPrx.checkedCast(ic.stringToProxy("D@DispatchPnl"))
    #obj.redispatch(149226224, 153499307);
    # not finished obj.redispatch(151134812, 152838079)
    obj.redispatch(152838079, 153499307)

    #obj = mop.hi.svc.dispatch.DispatcherPrx.checkedCast(ic.stringToProxy("D@DispatchGpl"))
    #obj.redispatch(10079834, 10156378);
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

