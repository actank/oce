#! /usr/bin/env python
import sys, traceback, Ice, time, datetime
Ice.loadSlice("-I/opt/Ice/slice -I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice ../../OceSlice/slice/RFeed.ice")
from xce.feed import *
import unittest
import MySQLdb
status = 0
ic = None
ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300", "--Ice.Trace.Network=1"])
name = "FCM@FeedContent0"


    
def initDB():
    db=MySQLdb.connect(host="10.22.206.57",user="xndev",passwd="rebornlOM",db="feed")
    c=db.cursor()
    c.execute(
        """
        TRUNCATE TABLE feed_content;\
        INSERT INTO feed_content VALUES\
        (1,1,1,1,1,CURRENT_TIMESTAMP,1,'test xml feedid = 1',0,-1,'',-1,''),\
        (2,2,1,2,2,CURRENT_TIMESTAMP,1,'test xml feedid = 2',0,-1,'',-1,''),\
        (3,3,1,3,3,CURRENT_TIMESTAMP,1,'test xml feedid = 3',0,-1,'',-1,''),\
        (4,4,1,4,4,CURRENT_TIMESTAMP,1,'test xml feedid = 4',0,-1,'',-1,''),\
        (5,5,1,5,5,CURRENT_TIMESTAMP,1,'test xml feedid = 5',0,-1,'',-1,''),\
        (6,6,1,6,6,CURRENT_TIMESTAMP,1,'test xml feedid = 6',0,-1,'',-1,''),\
        (7,7,1,7,7,CURRENT_TIMESTAMP,1,'test xml feedid = 7',0,-1,'',-1,''),\
        (8,8,1,8,8,CURRENT_TIMESTAMP,1,'test xml feedid = 8',0,-1,'',-1,''),\
        (9,9,1,9,9,CURRENT_TIMESTAMP,1,'test xml feedid = 9',0,-1,'',-1,'');
        """)
    c.close()
    db.commit()

def deleteContent(feedId):
    sql = "delete from feed_content where feed="+str(feedId)
    db=MySQLdb.connect(host="10.22.206.57",user="xndev",passwd="rebornlOM",db="feed")
    c=db.cursor()
    c.execute(sql)
    c.close()
    db.commit()
    
def insertContent(content):
    param = {
        "feed":content.feed,
        "source":content.source,
        "psource":1,
        "type":content.type,
        "actor":content.actor,
        "time":"CURRENT_TIMESTAMP",
        "prop":1,
        "xml":"\"xml data\"",
        "comment":0,
        "older_reply_id":-1,
        "older_reply":"\'\'",
        "newer_reply_id":-1,
        "newer_reply":"\'\'"
        }
    sql = "insert into feed_content values \
( %(feed)s,%(source)s,%(psource)s,%(type)s,%(actor)s,%(time)s,%(prop)s,%(xml)s,\
%(comment)s,%(older_reply_id)s,%(older_reply)s,\
%(newer_reply_id)s,%(newer_reply)s );" % param
    db=MySQLdb.connect(host="10.22.206.57",user="xndev",passwd="rebornlOM",db="feed")
    c=db.cursor()
    c.execute(sql)
    c.close()
    db.commit()
    
class FeedContentTestCase(unittest.TestCase):
    def setUp(self):
        self._proxy = FeedContentManagerPrx.checkedCast(ic.stringToProxy(name))
    def _createContent(self,id):
        feedContent = FeedContent()
        feedContent.feed = id
        feedContent.source = id%10
        feedContent.actor = id
        feedContent.type = id
        return feedContent
    def _createReply(self,id):
        reply = FeedReply()
        reply.id = id;
        reply.reply = "reply id = "+str(id)
        return reply
    def _compareContent(self,con1,con2):
        if con1.feed != con2.feed:
            return False
        if con1.source != con2.source:
            return False
        if con1.actor != con2.actor:
            return False
        if con1.type != con2.type:
            return False
        return True 
    def _compareReply(self,re1,re2):
        if re1.id != re2.id:
            return False
        if re1.reply != re2.reply:
            return False
        return True
    def _getCheck(self,idSet,result):
        if len(idSet) != len(result):
            return False
        for id in idSet:
            if result[id] == None:
                return False
        return True
    def _getCheckNULL(self,idSet,result):
        if len(idSet) != len(result):
            return False
        for id in idSet:
            if result[id] != None:
                return False
        return True
    def get_miss(self):
        feeds = self._proxy.getFeedDict(range(10,15))
        print feeds
    def add_get_content(self):
        for id in range(1,10):
            self._proxy.addFeedContent(self._createContent(id))  # test addFeedContent
        feeds = self._proxy.getFeedDict(range(1,10))             # test getFeedDict
        self.assertEqual(self._getCheck(range(1,10),feeds),True)    
        feeds = self._proxy.getFeedDict(range(1,10))             # test getFeedDict
        self.assertEqual(self._getCheck(range(1,10),feeds),True)
    def get_content(self):
        feeds = self._proxy.getFeedDict(range(1,10))             # test getFeedDict
        self.assertEqual(self._getCheck(range(1,10),feeds),True)
    def add_remove_content(self):
        for id in range(1,10):
            self._proxy.addFeedContent(self._createContent(id))  # test addFeedContent
        feeds = self._proxy.getFeedDict(range(1,10))          # test getFeedDict
        self.assertEqual(self._getCheck(range(1,10),feeds),True)
        for id in range(1,10):
            self.assertEqual(self._compareContent(self._createContent(id),feeds[id]),True)
        self._proxy.removeFeed(5)                             # test removeFeed
        feeds = self._proxy.getFeedDict([5])                 
        self.assertEqual(self._getCheckNULL([5],feeds),True)
        feeds = self._proxy.getFeedDict(range(1,5))
        self.assertEqual(self._getCheck(range(1,5),feeds),True)
        feeds = self._proxy.getFeedDict(range(6,10))
        self.assertEqual(self._getCheck(range(6,10),feeds),True)
        feeds = self._proxy.getFeedDict(range(1,10))
        self.assertEqual(len(feeds),9)
        self._proxy.removeFeeds(range(1,5))                   # test removeFeeds
        feeds = self._proxy.getFeedDict(range(6,10))
        self.assertEqual(self._getCheck(range(6,10),feeds),True)
        feeds = self._proxy.getFeedDict(range(1,5))
        self.assertEqual(self._getCheckNULL(range(1,5),feeds),True)
        self._proxy.removeFeeds(range(1,10))
        feeds = self._proxy.getFeedDict(range(1,10))
        self.assertEqual(self._getCheckNULL(range(1,10),feeds),True)
    def add_reply(self):
        self._proxy.addFeedContent(self._createContent(10)) #  add feed 10        feed:10
        self._proxy.addFeedReply(10,self._createReply(3))      #  add reply 3 to 10  feed:10-3 
        feeds = self._proxy.getFeedDict([10])
        self.assertEqual(len(feeds[10].replySeq),1)
        self.assertEqual(self._compareReply(feeds[10].replySeq[0],self._createReply(3)),True)
        self._proxy.addFeedReply(10,self._createReply(5))      #  add reply 5 to 10  feed:10-3-5
        feeds = self._proxy.getFeedDict([10])
        self.assertEqual(len(feeds[10].replySeq),2)
        self.assertEqual(self._compareReply(feeds[10].replySeq[0],self._createReply(3)),True)
        self.assertEqual(self._compareReply(feeds[10].replySeq[1],self._createReply(5)),True)
        self._proxy.addFeedReply(10,self._createReply(4))      #  add reply 4 to 10  feed:10-3-5
        feeds = self._proxy.getFeedDict([10])
        self.assertEqual(len(feeds[10].replySeq),2)
        self.assertEqual(self._compareReply(feeds[10].replySeq[0],self._createReply(3)),True)
        self.assertEqual(self._compareReply(feeds[10].replySeq[1],self._createReply(5)),True)
        self._proxy.addFeedReply(10,self._createReply(6))      #  add reply 6 to 10  feed:10-3-6
        feeds = self._proxy.getFeedDict([10])
        self.assertEqual(len(feeds[10].replySeq),2)
        self.assertEqual(self._compareReply(feeds[10].replySeq[0],self._createReply(3)),True)
        self.assertEqual(self._compareReply(feeds[10].replySeq[1],self._createReply(6)),True)
        self._proxy.addFeedReply(10,self._createReply(1))       # add reply 1 to 10  feed:10-3-6
        feeds = self._proxy.getFeedDict([10])
        self.assertEqual(len(feeds[10].replySeq),2)
        self.assertEqual(self._compareReply(feeds[10].replySeq[0],self._createReply(1)),True)
        self.assertEqual(self._compareReply(feeds[10].replySeq[1],self._createReply(6)),True)
        self._proxy.addFeedContent(self._createContent(11))  # add content 11
        self._proxy.addFeedReply(11,self._createReply(10))      # add reply 10 to 11 feed:11-10
        feeds = self._proxy.getFeedDict([11])
        self.assertEqual(len(feeds[11].replySeq),1)
        self.assertEqual(self._compareReply(feeds[11].replySeq[0],self._createReply(10)),True)
        self._proxy.addFeedReply(11,self._createReply(1))       # add reply 1 to 11  feed:11-1-10
        feeds = self._proxy.getFeedDict([11])
        self.assertEqual(len(feeds[11].replySeq),2)
        self.assertEqual(self._compareReply(feeds[11].replySeq[0],self._createReply(1)),True)
        self.assertEqual(self._compareReply(feeds[11].replySeq[1],self._createReply(10)),True)
        self._proxy.removeFeed(10) 
        self._proxy.removeFeed(11) 
    def remove_reply(self):
        feedId = 100
        self._proxy.addFeedContent(self._createContent(feedId))
        self._proxy.addFeedReply(feedId,self._createReply(3))
        self._proxy.addFeedReply(feedId,self._createReply(2))       # feed: feedId-2-3
        feeds = self._proxy.getFeedDict([feedId])
        self.assertEqual(len(feeds[feedId].replySeq),2)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[0],self._createReply(3)),True)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[1],self._createReply(2)),True)
        self._proxy.removeFeedReply(3)                          # remove reply 3  feed: feedId-2
        feeds = self._proxy.getFeedDict([feedId])
        self.assertEqual(len(feeds[feedId].replySeq),1)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[0],self._createReply(2)),True)
        self._proxy.addFeedReply(feedId,self._createReply(3))       # feed: feedId-2-3
        feeds = self._proxy.getFeedDict([feedId])
        self.assertEqual(len(feeds[feedId].replySeq),2)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[0],self._createReply(3)),True)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[1],self._createReply(2)),True)
        self._proxy.removeFeedReply(2)                          # feed: feedId-3
        feeds = self._proxy.getFeedDict([feedId])
        self.assertEqual(len(feeds[feedId].replySeq),1)
        self.assertEqual(self._compareReply(feeds[feedId].replySeq[0],self._createReply(3)),True)
        self._proxy.removeFeed(feedId) 
    def same_pos(self):
        pool_size = 10000
        id1 = 51
        id2 = pool_size+id1
        insertContent(self._createContent(id1))
        insertContent(self._createContent(id2))
        get = self._proxy.getFeedDict([id1,id2])
        self.assertEqual(len(get),2)
        print "====="
        print get
        print "====="
        print get[id2]
        print self._createContent(id2)
        print get[id1]
        print self._createContent(id1)
        self.assertEqual(self._compareContent(get[id2],self._createContent(id2)),True)
        self.assertEqual(self._compareContent(get[id1],self._createContent(id1)),True)
        deleteContent(id1)
        deleteContent(id2)
        self._proxy.removeFeeds([id1,id2]) 
        
        
        
if __name__ == '__main__':
    initDB()
    suite = unittest.TestSuite()
    suite.addTest(FeedContentTestCase("add_get_content"))
    suite.addTest(FeedContentTestCase("get_content"))
    suite.addTest(FeedContentTestCase("get_miss"))
#    suite.addTest(FeedContentTestCase("add_reply"))
#    suite.addTest(FeedContentTestCase("remove_reply"))
    suite.addTest(FeedContentTestCase("same_pos"))
    runner = unittest.TextTestRunner()
    runner.run(suite)
