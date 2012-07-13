#! /usr/bin/env python
import sys, traceback, Ice, time, datetime
Ice.loadSlice("-I/opt/Ice/slice -I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice ../../OceSlice/slice/RFeed.ice")
from xce.feed import *
import unittest
import MySQLdb
status = 0
ic = None
ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300", "--Ice.Trace.Network=1"])
name = "FCM@FeedContent_reply0"



class FeedContentTestCase(unittest.TestCase):
    def _insertContent(self):
        """
        +-------+-----------+----------+----------+
        | owner | album     | id       | body     |
        +-------+-----------+----------+----------+
      1 | 61782 | 201897075 | 24739989 | test 1   | 
      2 | 61782 | 250453740 | 24629538 | test 1   | 
        | 61782 | 250453740 | 24629559 | test 2   | 
        | 61782 | 250453740 | 24629577 | test 3   | 
      3 | 61782 | 254733276 | 24629413 | test old | 
        | 61782 | 254733276 | 24629433 | test new | 
      4 | 61782 | 260597232 | 24604736 | test     |
        +-------+-----------+----------+----------+
        """
        param = {
            "feed":None,
            "source":None,
            "psource":1,
            "type":701,
            "actor":61782,
            "time":"CURRENT_TIMESTAMP",
            "prop":1,
            "xml":"\"xml data\"",
            "comment":0,
            "older_reply_id":-1,
            "older_reply":"\'\'",
            "newer_reply_id":-1,
            "newer_reply":"\'\'"
            }
        db=MySQLdb.connect(host="10.22.206.57",user="xndev",passwd="rebornlOM",db="feed")
        c=db.cursor()
        c.execute("delete from feed_content where actor=61782;")
        param["feed"] = 1
        param["source"] = 201897075
        param["comment"] = 1
        param["older_reply_id"] = 24739989
        param["older_reply"] = "\'test 1\'"
        param["newer_reply_id"] = -1
        param["newer_reply"] = "\'\'"
        sql = "insert into feed_content values \
( %(feed)s,%(source)s,%(psource)s,%(type)s,%(actor)s,%(time)s,%(prop)s,%(xml)s,\
%(comment)s,%(older_reply_id)s,%(older_reply)s,\
%(newer_reply_id)s,%(newer_reply)s );" % param
        c.execute(sql)
        param["feed"] = 2
        param["source"] = 250453740
        param["comment"] = 3
        param["older_reply_id"] = 24629538
        param["older_reply"] = "\'test 1\'"
        param["newer_reply_id"] = 24629577
        param["newer_reply"] = "\'test 3\'"
        sql = "insert into feed_content values \
( %(feed)s,%(source)s,%(psource)s,%(type)s,%(actor)s,%(time)s,%(prop)s,%(xml)s,\
%(comment)s,%(older_reply_id)s,%(older_reply)s,\
%(newer_reply_id)s,%(newer_reply)s );" % param
        c.execute(sql)
        param["feed"] = 3
        param["source"] = 254733276
        param["comment"] = 2
        param["older_reply_id"] = 24629413
        param["older_reply"] = "\'test old\'"
        param["newer_reply_id"] = 24629433
        param["newer_reply"] = "\'test new\'"
        sql = "insert into feed_content values \
( %(feed)s,%(source)s,%(psource)s,%(type)s,%(actor)s,%(time)s,%(prop)s,%(xml)s,\
%(comment)s,%(older_reply_id)s,%(older_reply)s,\
%(newer_reply_id)s,%(newer_reply)s );" % param
        c.execute(sql)
        param["feed"] = 4
        param["source"] = 260597232
        param["comment"] = 1
        param["older_reply_id"] = 24604736
        param["older_reply"] = "\'test\'"
        param["newer_reply_id"] = -1
        param["newer_reply"] = "\'\'"
        sql = "insert into feed_content values \
( %(feed)s,%(source)s,%(psource)s,%(type)s,%(actor)s,%(time)s,%(prop)s,%(xml)s,\
%(comment)s,%(older_reply_id)s,%(older_reply)s,\
%(newer_reply_id)s,%(newer_reply)s );" % param
        c.execute(sql)
        c.close()
        db.commit()
    def setUp(self):
        self._insertContent()
        self._proxy = FeedContentManagerPrx.checkedCast(ic.stringToProxy(name))
    def testreply(self):
        maxid = 30000000
        reply = FeedReply()
        reply.id = 1
        reply.reply = "test 0"
        self._proxy.addFeedReply(1,reply)
        reply.id = maxid-4
        reply.reply = "test 1"
        self._proxy.addFeedReply(4,reply)
        reply.id = 2
        reply.reply = "test 0"
        self._proxy.addFeedReply(3,reply)
        reply.id = maxid-3
        reply.reply = "test max"
        self._proxy.addFeedReply(3,reply)
        reply.id = 3
        reply.reply = "test 0"
        self._proxy.addFeedReply(2,reply)
        reply.id = maxid-2
        reply.reply = "test 4"
        self._proxy.addFeedReply(2,reply)
        contents = self._proxy.getFeedDict([1,2,3,4])
        self.assertEqual(contents[1].commentCount,2)#--
        self.assertEqual(contents[1].replySeq[0].id,1)
        self.assertEqual(contents[1].replySeq[0].reply,"test 0")
        self.assertEqual(contents[1].replySeq[1].id,24739989)
        self.assertEqual(contents[1].replySeq[1].reply,"test 1")
        self.assertEqual(contents[2].commentCount,5)#--
        self.assertEqual(contents[2].replySeq[0].id,3)
        self.assertEqual(contents[2].replySeq[0].reply,"test 0")
        self.assertEqual(contents[2].replySeq[1].id,maxid-2)
        self.assertEqual(contents[2].replySeq[1].reply,"test 4")
        self.assertEqual(contents[3].commentCount,4)#--
        self.assertEqual(contents[3].replySeq[0].id,2)
        self.assertEqual(contents[3].replySeq[0].reply,"test 0")
        self.assertEqual(contents[3].replySeq[1].id,maxid-3)
        self.assertEqual(contents[3].replySeq[1].reply,"test max")
        self.assertEqual(contents[4].commentCount,2)#--
        self.assertEqual(contents[4].replySeq[0].id,24604736)
        self.assertEqual(contents[4].replySeq[0].reply,"test")
        self.assertEqual(contents[4].replySeq[1].id,maxid-4)
        self.assertEqual(contents[4].replySeq[1].reply,"test 1")

        self._proxy.removeFeedReply(1,1)
        content = self._proxy.getFeedDict([1])[1]
        self.assertEqual(content.commentCount,1)#--
        self.assertEqual(content.replySeq[0].id,24739989)
        self.assertEqual(content.replySeq[0].reply,"test 1")
        
        self._proxy.removeFeedReply(2,3)
        content = self._proxy.getFeedDict([2])[2]
        self.assertEqual(content.commentCount,4)#--
        self.assertEqual(content.replySeq[0].id,24629538)
        self.assertEqual(content.replySeq[0].reply,"test 1")
        self.assertEqual(content.replySeq[1].id,maxid-2)
        self.assertEqual(content.replySeq[1].reply,"test 4")
        
        self._proxy.removeFeedReply(2,maxid-2)
        content = self._proxy.getFeedDict([2])[2]
        self.assertEqual(content.commentCount,3)#--
        self.assertEqual(content.replySeq[0].id,24629538)
        self.assertEqual(content.replySeq[0].reply,"test 1")
        self.assertEqual(content.replySeq[1].id,24629577)
        self.assertEqual(content.replySeq[1].reply,"test 3")
        
        self._proxy.removeFeedReply(4,maxid-4)
        content = self._proxy.getFeedDict([4])[4]
        self.assertEqual(content.commentCount,1)#--
        self.assertEqual(content.replySeq[0].id,24604736)
        self.assertEqual(content.replySeq[0].reply,"test")

        
if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(FeedContentTestCase("testreply"))
    runner = unittest.TextTestRunner()
    runner.run(suite)
