import MySQLdb
import sys, traceback, Ice, time, datetime
Ice.loadSlice("-I/opt/Ice/slice -I../../UtilSlice/slice -I../../OceSlice/slice --all ../../UtilSlice/slice/Util.ice ../../OceSlice/slice/FeedReply.ice ../../OceSlice/slice/BuddyCache.ice")
from xce.feed import *
import unittest

ic = Ice.initialize(["--Ice.Default.Locator=XceFeed/Locator:tcp -h 10.3.18.186 -p 14300", "--Ice.Trace.Network=1"])
name = "RM@FeedReply1"



def getFeedReply():
    proxy = FeedReplyManagerPrx.checkedCast(ic.stringToProxy(name))
    proxy.reload()
    reply = proxy.getOldFeedReply(61782,260597232,701)#actor source stype
    print "reply.id=%d\t reply.reply=%s" % (reply.id,reply.reply)
    
class FeedContentTestCase(unittest.TestCase):
    def setUp(self):
        self._proxy = FeedReplyManagerPrx.checkedCast(ic.stringToProxy(name))
    def testOneReply(self):#260597232 1 reply "test"    
        reply = self._proxy.getOldFeedReply(61782,260597232,701)
        self.assertEqual(reply.reply,"test")
        reply = self._proxy.getNewFeedReply(61782,260597232,701)
        self.assertEqual(reply.reply,"test")
        reply = self._proxy.getFeedReplyPair(61782,260597232,701)
        self.assertEqual(len(reply),1)
        self.assertEqual(reply[0].reply,"test")
    def testTwoReply(self):#254733276 2 reply "test old","test new"
        reply = self._proxy.getOldFeedReply(61782,254733276,701)
        self.assertEqual(reply.reply,"test old")
        reply = self._proxy.getNewFeedReply(61782,254733276,701)
        self.assertEqual(reply.reply,"test new")
        reply = self._proxy.getFeedReplyPair(61782,254733276,701)
        self.assertEqual(len(reply),2)
        self.assertEqual(reply[0].reply,"test old")
        self.assertEqual(reply[1].reply,"test new")
    def testThreeReply(self):#250453740 3 reply "test 1","test 2","test 3"
        reply = self._proxy.getOldFeedReply(61782,250453740,701)
        self.assertEqual(reply.reply,"test 1")
        reply = self._proxy.getNewFeedReply(61782,250453740,701)
        self.assertEqual(reply.reply,"test 3")
        reply = self._proxy.getFeedReplyPair(61782,250453740,701)
        self.assertEqual(len(reply),2)
        self.assertEqual(reply[0].reply,"test 1")
        self.assertEqual(reply[1].reply,"test 3")
    
if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(FeedContentTestCase("testOneReply"))
    suite.addTest(FeedContentTestCase("testTwoReply"))
    suite.addTest(FeedContentTestCase("testThreeReply"))
    runner = unittest.TextTestRunner()
    runner.run(suite)

    
