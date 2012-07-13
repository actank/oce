#! /usr/bin/env python
import sys, traceback, types, Ice, time
Ice.loadSlice("-I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice \
            ../../OceSlice/slice/FeedIndex.ice")

from xce.feed import *

#Test Class
class FeedIndexTest():
    def __init__(self):
        self.iceModule = None
        self.indexManger = None
        self.iceModule = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300",
                              "--Ice.Trace.Network=1"])
        self.indexManager = FeedIndexManagerPrx.checkedCast(self.iceModule.stringToProxy("FIM@FeedIndex0"))
        
    def printFeedIndex(self, feedIndex):
        try:
            print("feed:%d,"%feedIndex.feed)
            print('\n')
        except:
            traceback.print_exc()
        
    def printFeedIndexSeq(self, seq):
        try:
            Len = len(seq)
            for i in range(0, Len):
                FeedIndex = seq[i]
                print("feed:%d,"%FeedIndex.feed)
                print('\n')
        except:
            traceback.print_exc()
            
    def printLongSeq(self, seq):
        try:
            Len = len(seq)
            for i in range(0, Len):
                index = seq[i]
                print 'feed been removed'+str(index)+'\n'
        except:
            traceback.print_exc()
            
    def addFeedIndex(self, feed,source,parent,actor,time, stype):
        try:
            feedIndex = FeedIndex()
            feedIndex.feed = feed 
            feedIndex.source = source
            feedIndex.parent = parent
            feedIndex.actor = actor
            feedIndex.time = time
            feedIndex.stype = stype
            self.indexManager.add(feedIndex)
        except:
             traceback.print_exc()
             
    def getBySourceAndStype(self, source, stype, size):
        try:
            feedIndexSeq = self.indexManager.getBySourceAndStype(source,stype,size)
            print 'getBySourceAndStype:'+str(source)+','+str(stype)+','+str(size)+'\n'
            self.printFeedIndexSeq(feedIndexSeq)
        except:
            traceback.print_exc()
            
    def get(self,source,stype,actor):
        try:
            feedIndex = self.indexManager.get(source,stype,actor)
            print 'get: source'+str(source)+',stype:'+str(stype)+',actor:'+str(actor)+'\n'
            self.printFeedIndex(feedIndex)
        except:
            traceback.print_exc()
            
    def removeBySourceAndStype(self,source,stype):
        try:
            indexSeq = self.indexManager.removeBySourceAndStype(source,stype)
            print 'removeBySourceAndStype:'+str(source)+','+str(stype)+'\n'
            self.printLongSeq(indexSeq)
        except:
            traceback.print_exc()
        
    def remove(self,source,stype,actor):
        try:
            feedIndexSeq = self.indexManager.remove(source,stype,actor)
            print 'remove:'+str(source)+','+str(stype)+','+str(actor)+'\n'
            self.printLongSeq(indexSeq)
        except:
            traceback.print_exc()
        
    
    def __finalize__(self):
        try:
            self.iceModule.destroy()
        except:
            traceback.print_exc()
            
            
        

#main Test Logic
testInstance = FeedIndexTest()
#add 10 items
for i in range(0,10):
    testInstance.addFeedIndex(i, i, i, i, i, i)

testInstance.addFeedIndex(10, 1, 1, 1, 1, 1)
testInstance.addFeedIndex(11, 1, 1, 1, 1, 1)
testInstance.addFeedIndex(12, 2, 2, 2, 2, 2)

#Get the lastest one 
testInstance.get(1, 1, 1)

#Get by source and stype
testInstance.getBySourceAndStype(1, 1, 3)
testInstance.getBySourceAndStype(1, 1, 0)
testInstance.getBySourceAndStype(2,2,2)

testInstance.removeBySourceAndStype(2, 2)


testInstance.__finalize__()
sys.exit(1) 
    
                              
