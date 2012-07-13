import sys, traceback, Ice, time
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all -I/opt/Ice-3.2/slice ../UtilSlice/slice/Util.ice ../OceSlice/slice/Importer.ice")
import MyUtil
import com.xiaonei.svc.importer
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry.kaixin.com -p 12000"])
    #:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    manager = com.xiaonei.svc.importer.ImporterManagerPrx.checkedCast(ic.stringToProxy("M@Importer"))
    owner = com.xiaonei.svc.importer.OwnerInfo(200353733, "xiaonei-test@hotmail.com", "tester!")
    manager.sendRequest(owner);
    print "Request send"
    time.sleep(20)
    response = manager.getResponse(owner)
    print response
    buddies = response.buddies
    if response.status.code == 200 or response.status.code==302:
        print "Invoke OK."
        status = 0
    else:
        status = response.status.code
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

