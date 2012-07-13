#! /usr/bin/env python
import sys, traceback, Ice 
Ice.loadSlice("-I/opt/Ice-3.3/slice -I../UtilSlice/slice -I../OceSlice/slice --all ../UtilSlice/slice/Util.ice ../OceSlice/slice/PVCountDemo.ice")
from Demo import *

index = raw_input('Input the index of the server node: ')

status = 0
ic = None
try:
	ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:default -h 10.3.18.186 -p 14300", "--Ice.Trace.Network=1"])
	name = "PVCountDemo@PVCountDemo%s" %(index)
	print "name: %s" %(name)
	prx = PVCountDemoPrx.checkedCast(ic.stringToProxy(name))	
except:
	traceback.print_exc()
	status = 1
	sys.exit(status)

def testAdd(userId, pvCount):
	prx.add(int(userId), int(pvCount))
def testUpdate(userId, pvCount):
	prx.update(int(userId), int(pvCount))
def testRemove(userId):
	prx.remove(int(userId))
def testGet(userId):
	return prx.getPVCountByUserId(int(userId))

print '\tSelect the operation:'
print '\tAdd------------输入0'
print '\tUpdate---------输入1'
print '\tRemove---------输入2'
print '\tGet------------输入3（默认）'
print '\tExit-----------输入exit'
try:
	while True:
		print '-----------------------------------------------------'
		operation = raw_input('Input the operation: ')
		if operation == '0':
			print '###Test Add###: '
			userId = raw_input('Input userId: ')
			pvCount = raw_input('Input pvCount: ')
			testAdd(userId, pvCount);
		elif operation == '1':
			print '###Test update###: '
			userId = raw_input('Input userId: ')
			pvCount = raw_input('Input pvCount: ')
			testUpdate(userId, pvCount);
		elif operation == '2':
			print '###Test remove###: '
			userId = raw_input('Input userId: ')
			testRemove(userId);
		elif operation == '3' or operation == '':
			print '###Test get###: '
			userId = raw_input('Input userId: ')
			print "The pvCount is: %d" %(testGet(userId))
		elif operation == 'exit':
			if ic:
			    try:
				ic.destroy()
			    except:
				traceback.print_exc()
				status = 1
			sys.exit(status)
		else:
			print "Input error, please input again!!!"
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
