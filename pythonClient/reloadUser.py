#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/User.ice ../OceSlice/slice/UserConfig.ice ../OceSlice/slice/UserNetwork.ice")
import MyUtil
import mop.hi.svc.model
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"])
    #ic = Ice.initialize(sys.argv)
    users = []
    nets = []
    config = []
    for i in range(0, 100):
        print "Initializing proxy for ending with", i
        users.append( mop.hi.svc.model.UserManagerPrx.checkedCast(ic.stringToProxy("UM@UserN" + str(i))) )
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        #users.append( mop.hi.svc.model.UserManagerPrx.checkedCast(ic.stringToProxy("UM@UserN" + str(i))) )
        nets.append( mop.hi.svc.model.NetworkManagerPrx.checkedCast(ic.stringToProxy("NM@UserNetworkN" + str(i))) )
        config.append( mop.hi.svc.model.UserConfigManagerPrx.checkedCast(ic.stringToProxy("UCM@UserConfig" + str(i))) )
    #uid=int(200353733)
    #users[uid % 10].reloadUser(200353733)
    f = file("reloadUser.txt")
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        str = line.split(",")
        for i in str:
            uid=int(i)
            euid=uid % 100
            enid=uid % 10
            print euid, uid
            try:
                users[euid].reloadUser(uid)
            except:
                xxx=0#traceback.print_exc()
            users[euid].reloadUserBasic(uid)
            users[euid].reloadUserDesc(uid)
            users[euid].reloadUserVisitCount(uid)
            #print users[eid].getUserDesc(uid)
            #nets[enid].reloadNetworkSeq(uid);
            #nets[enid].reloadHighSchoolInfoSeq(uid);
            #nets[enid].reloadUniversityInfoSeq(uid);
            #nets[enid].reloadWorkplaceInfoSeq(uid);
            #nets[enid].reloadCollegeInfoSeq(uid);
            #config[enid].reload(uid);
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

