#! /usr/bin/env python
import sys, traceback, Ice
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice")
import MyUtil
import MySQLdb
status = 0
ic = None
try:
    ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
    objectcaches = []
    users = []
    for i in range(0, 10):
        print "Initializing proxy for ending with", i
        objectcaches.append( MyUtil.ObjectCachePrx.checkedCast(ic.stringToProxy("SC@FriendDist" + str(i))) )
    #f = file("preloadUser.txt")
    for i in range(0, 10):
        user = []
        users.append(user)
    max = -1
    conn = MySQLdb.connect(host="yi",user="ipo2008", passwd="meiguiyuan",db="fb")
    cursor = conn.cursor()
    while True:
        cursor.execute("select id FROM users where id > "+str(max)+" limit 1000")
        rs = cursor.fetchall()
	if len(rs)==0:
            break
        for row in rs:
            id = row[0]
            if max < id:
                max = id
            users[id%10].append(id)
        for i in range(0, 10):
            try:
                objectcaches[i].reloadObjects(0, users[i])
		print "ok"
            except:
                print "excep"
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

