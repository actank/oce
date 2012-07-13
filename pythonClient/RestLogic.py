#! /usr/bin/env python
import sys, traceback, Ice, time
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../OceSlice/slice/RestLogic.ice")
from talk.rest import *
from com.xiaonei.talk.common import *

status = 0
ic = None
try:
	userid = 267044580#229752194
	count = 10
	ic = Ice.initialize(["--Ice.Default.Locator=XNTalk/Locator:default -h 10.3.18.31 -p 12000:default"])
	name = "M@RestLogic0"
	proxy = RestLogicManagerPrx.checkedCast(ic.stringToProxy(name))	
		#print "onlinecenter"
	#res = proxy.addRelation(1111, 1112, 1);
	#res = proxy.addRelation(1111, 1113, 1);
	#res = proxy.addRelation(1111, 1110, 1);
	#res = proxy.addRelation(1111, 1119, 1);
	#res = proxy.cancelRelation(1119, 1111, 1);
	jid = Jid()
	jid.userId = 255233137
	#res = proxy.setStatus(jid, OnlineStat.ONLINE, "(v)status")
	#res = proxy.replyStatus(238489851, 504684889, 255233137, 240599893, "adfa(v)")
	res = proxy.useImNotify(255233137)
	#print res
	#print "login time:"
	#print logintime
	
#ic = Ice.initialize(sys.argv)
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

