import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all -I/opt/Ice-3.2/slice ../UtilSlice/slice/Util.ice ../UtilSlice/slice/DbDescriptor.ice")
import MyUtil
import com.xiaonei.xce
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry.xiaonei.com -p 12000"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry.kaixin.com -p 12000"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.2.8.45 -p 12000"])
    #ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.2.8.25 -p 12000"])
    print "initialize ok"
    descriptor = com.xiaonei.xce.DbDescriptorPrx.checkedCast(ic.stringToProxy("DCS@DbDescriptor"))
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

