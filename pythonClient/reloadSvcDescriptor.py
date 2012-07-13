import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all -I/opt/Ice-3.2/slice ../UtilSlice/slice/Util.ice ../UtilSlice/slice/SvcDescriptor.ice")
import MyUtil
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h ocetest -p 12000"])
    #:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    print "initialize ok"
    descriptor = MyUtil.SvcDescriptorPrx.checkedCast(ic.stringToProxy("SD@SvcDescriptor"))
    print "reload start ..."
    descriptor.reload()
    print "reload end"

except:
    print "except"
    traceback.print_exc()
    status = 1
if ic:
    try:
        ic.destroy()
    except:
        traceback.print_exc()
        status = 1
sys.exit(status)

