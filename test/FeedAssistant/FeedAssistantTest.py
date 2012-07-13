#! /usr/bin/env python
import sys, traceback, types, Ice, time
Ice.loadSlice("-I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice \
            ../../OceSlice/slice/RFeed.ice")

from xce.feed import *


#Test Class
class FeedAssitantTest():
    def __init__(self):
        self.iceModule = None
        self.assistant = None
        self.iceModule = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300",
                              "--Ice.Trace.Network=1"])
        self.assistant = FeedAssistantPrx.checkedCast(self.iceModule.stringToProxy("FA@FeedAssistant0"))
     
    def removeFeedById(self, feedId):
        try:
            self.assistant.removeFeedById(feedId)
        except:
            traceback.print_exc()
        
    def removeFeedBySource(self, source, stype):
        try:
            self.assistant.removeFeedBySource(source, stype)
        except:
            traceback.print_exc()
        
    def removeFeedByParent(self, parent, ptype):
        try:
            self.assistant.removeFeedByParent(parent, ptype)
        except:
            traceback.print_exc()
        
    def removeFeed(self, source, stype, actor):
        try:
            self.assistant.removeFeed(source, stype, actor)
        except:
            traceback.print_exc()
        
    def removeFeedReply(self, source, stype, actor, replyId):
        try:
            self.assistant.removeFeedReply(source, stype, actor, replyId)
        except:
            traceback.print_exc()
        
    def readNews(self, user, feedid):
        try:
            self.assistant.readNews(source, user, feedid)
        except:
            traceback.print_exc()
        
    def readAllNews(self, user):
        try:
            self.assistant.readNews(user)
        except:
            traceback.print_exc()
    
    def readMini(self, user, feedid):
        try:
            self.assistant.readNews(user)
        except:
            traceback.print_exc()
    
    def dispatch(self, seed):
        try:
            self.assistant.dispatch(seed)
        except:
            traceback.print_exc()
            
    def addFeedReply(self, source, stype, actor, replyId, reply):
        try:
            self.assistant.addFeedReply(source, stype, actor, replyId, reply)
        except:
            traceback.print_exc()
       
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
  
    
    def __finalize__(self):
        try:
            self.iceModule.destroy()
        except:
            traceback.print_exc()
            
            
        

#main Test Logic
testInstance = FeedAssitantTest()
seed = FeedSeed()
for i in range(0, 20):
    seed.source = i
    seed.parent = i
    seed.ptype = i
    seed.type = i
    seed.feed = i
    seed.actor = i
    seed.time = i
    print(seed.time)
    testInstance.dispatch(seed)

testInstance.removeFeedBySource(19, 19)    
testInstance.removeFeedByParent(18, 18)
testInstance.removeFeedById(17)
testInstance.removeFeed(16, 16, 16)

seed = FeedSeed()
seed.source = 15
seed.parent = 10000
seed.ptype = 15
seed.type = 10000
seed.feed = 10000
seed.actor = 10000
seed.time = 10000

testInstance.removeFeedBySource(15, 15)

seed.source = 10000
seed.parent = 14
seed.ptype = 14
seed.type = 10000
seed.feed = 10000
seed.actor = 10000
seed.time = 10000

testInstance.removeFeedByParent(14, 14)
 
testInstance.__finalize__()
sys.exit(1) 
    
                              
