#! /usr/bin/env python
import sys, traceback, Ice, time
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../OceSlice/slice/TalkCommon.ice ../OceSlice/slice/OnlineCenter.ice ../OceSlice/slice/Presence.ice")
from talk.online import *
from com.xiaonei.talk import *

status = 0
ic = None
try:
	userid = 267044580#229752194
	count = 10
	ic = Ice.initialize(["--Ice.Default.Locator=XNTalk/Locator:default -h 10.22.204.111 -p 12000:default"])
	for i in range(count):
		name = "M@OnlineCenter%d" % (i,)
		print name;
		proxy = OnlineManagerPrx.checkedCast(ic.stringToProxy(name))	
		#print "onlinecenter"
		avatars = proxy.getUserAvatars(userid, 6)
		if(userid%count == i and len(avatars)>0):
				logintime = time.localtime(avatars[0].stamp)
				print logintime
				#jid = avatars[0].jid
				#print jid
				#proxy.offline(jid)
		print avatars

	presence = PresenceManagerPrx.checkedCast(ic.stringToProxy("M@Presence"))	
	print "presence"
	print presence.getOnlineUserJidSeq(userid)
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

