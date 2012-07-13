#! /usr/bin/env python
import sys
sys.path.append('/data/xce')
sys.path.append('/data/xce/IcePy-3.3.0/python')
import traceback, Ice, time
import random
Ice.loadSlice("-I. --all ./Util.ice  ./AdQuality.ice")
from xce.ad import *


class AdQualityAdapter(object):
  def __init__(self, index):
    self.index = index
    self.ic = None
    try:
      self.ic =  Ice.initialize(["--Ice.Default.Locator=Ad/Locator:default -h 10.3.17.119 -p 14800", "--Ice.Trace.Network=0", "--Ice.MessageSizeMax=2048000"])
      #elf.ic =  Ice.initialize(["--Ice.Default.Locator=Ad/Locator:default -h 10.3.18.186 -p 14800", "--Ice.Trace.Network=0", "--Ice.MessageSizeMax=204800000", "--Ice.Warn.Connections=1"])
      name = "M@AdQuality%d" %self.index
      self.proxy = AdQualityManagerPrx.checkedCast(self.ic.stringToProxy(name))
    except Exception, e:
      print "AdQualityAdapter : " + str(e)
      traceback.print_exc()

  def GetQualitys(self, user_key):
  	return self.proxy.GetQualitys(user_key)

  def update(self, stat_map):
  	self.proxy.update(stat_map)
  def reload(self):
  	self.proxy.reload()

  def GetLastUpdateTime(self):
  	return self.proxy.GetLastUpdateTime()

  def __del__(self):
    if self.ic:
    	try:
    		self.ic.destroy()
    	except:
    		traceback.print_exc()

