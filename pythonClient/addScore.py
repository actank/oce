import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/User.ice ../OceSlice/slice/UserNetwork.ice")
import MyUtil
import mop.hi.svc.model
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    #ic = Ice.initialize(sys.argv)
    users = []
    nets = []
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        users.append( mop.hi.svc.model.UserManagerPrx.checkedCast(ic.stringToProxy("UM@User" + str(i))) )
        nets.append( mop.hi.svc.model.NetworkManagerPrx.checkedCast(ic.stringToProxy("NM@UserNetwork" + str(i))) )
    #uid=int(200353733)
    #users[uid % 10].reloadUser(200353733)
    f = file("addScore.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            eid=uid % 10
            print eid, uid
            users[eid].incExp(uid, 100)
            print users[eid].getUser(uid)
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

