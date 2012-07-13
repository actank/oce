#! /usr/bin/env python
import sys, traceback, Ice
import os,sys
import time
import string
Ice.loadSlice("-I../UtilSlice/slice -I../OceSlice/slice --all  -I/opt/Ice-3.2/slice ../UtilSlice/slice/Util.ice ../UtilSlice/slice/ObjectCache.ice ../OceSlice/slice/Feed.ice")

import MyUtil 
import mop.hi.svc.feed
if len(sys.argv)<2:
	userId=int(raw_input("please input a userid"))
else:
	userId=int(sys.argv[1])
ISOTIMEFORMAT='%Y-%m-%d %X'
status = 0
ic = None
ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])
#ic = Ice.initialize(["--Ice.Default.Locator=XiaoNei/Locator:default -h 10.2.8.46 -p 12000:default -h 10.2.8.46 -p 12001:default -h 10.2.8.46 -p 12002"])
#:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"])

serverName="FN@FeedNews"+str(userId%100)
#serverName="FN@FeedNewsTest"

descriptor = mop.hi.svc.feed.FeedNewsManagerPrx.checkedCast(ic.stringToProxy(serverName))
fq=descriptor.getFeedSeq(userId,"",False,0,-1)
#fq=descriptor.getFeedSeq(userId,"",True,0,-1)
print "total:",fq.total," unknown:",fq.unknowns
for i in fq.data:
	
	print i.feedId,"  ",i.ownerId,"  ",i.type,"   ",i.unknown,"   ",time.strftime(ISOTIMEFORMAT,time.localtime(float(i.timestamp))),"  ","    ","    ",i.title 

#print fq
status = 1
if ic:
	try:
		ic.destroy()
	except:
		traceback.print_exc()
		status = 1
sys.exit(status)



