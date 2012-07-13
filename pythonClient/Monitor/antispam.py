#! /usr/bin/env python
import sys, traceback, Ice, os
Ice.loadSlice("-I/opt/Ice-3.2/slice -I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice ../../UtilSlice/slice/ObjectCache.ice ../../OceSlice/slice/XFeed.ice")
import MyUtil
#import mop.hi.svc.model
import xce.xfeed
status = 0
ic = None
cmd1="wget -q -O /dev/null \"http://10.22.198.81:2000/receiver?number="
cmd2="&message="
try:
	ic = Ice.initialize(["--Ice.Default.Locator=XiaoNeiAntiSpam/Locator:tcp -h XiaoNeiAntiSpamRegistry.xiaonei.com -p 4061"])
	f = file("antispams.xn")
	while True:
		line = f.readline()
		if len(line) == 0:
			break
		string = line.split()
		try:
			if len(string) == 2:
				ic.stringToProxy(string[0]+"@"+string[1]).ice_ping()
			else:
				ic.stringToProxy("SC@"+string[0]).ice_ping()
			print "ok",line
		except:
			traceback.print_exc()
			print "error",line
			os.system(cmd1+"15811391645"+cmd2+line+" xiaonei antispam SOS\"")
	f.close()
	ic = Ice.initialize(["--Ice.Default.Locator=KaiXinAntiSpam/Locator:tcp -h KaiXinAntiSpamRegistry.kaixin.com -p 4061"])
	f = file("antispams.kx")
	while True:
		line = f.readline()
		if len(line) == 0:
			break
		string = line.split()
		try:
			if len(string) == 2:
				ic.stringToProxy(string[0]+"@"+string[1]).ice_ping()
			else:
				ic.stringToProxy("SC@"+string[0]).ice_ping()
			print "ok",line
		except:
			traceback.print_exc()
			print "error",line
			os.system(cmd1+"15811391645"+cmd2+line+" kaixin antispam SOS\"")
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
