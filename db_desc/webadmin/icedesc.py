#!/usr/bin/python

import time, sys, traceback, Ice, IceGrid

Ice.loadSlice('-I../.. -I/opt/Ice-3.3/slice ../dbdescriptor.ice')
from com.xiaonei import xce

class IceDesc(xce.DbObserver):
  def __init__(self):
    self.ic = Ice.initialize()
    #ic = Ice.initialize(["--Ice.Default.Locator=IceGrid/Locator:default -h 10.2.18.183 -p 4061"])
    if self.ic:
      id = self.ic.stringToProxy("DbDescriptor:tcp -h 10.2.18.183 -p 10000")
      self.prx = xce.DbDescriptorPrx.checkedCast(id)
      self._init_adpter()
      self._callback = []
    else:
      print "error on ice initialize"

  def updated(self, d, current=None):
    print "updated--"
    if xce.DbInstance == type(d):
      print '<DbInstance>', d.name, d.type, d.timestamp, "w:", d.wserver, "r:", d.rservers, "e:", d.routes
      for cb in  self._callback:
        cb(d)

  def _init_adpter(self):
    self.adp = self.ic.createObjectAdapterWithEndpoints('testob', "tcp")
    pr = self.adp.addWithUUID(self)
    self.thepr = xce.DbObserverPrx.uncheckedCast(pr)
    self.adp.activate()

  def add_callback(self, callback):
    if not callback in self._callback:
      self._callback.append(callback)
  
  def del_callback(self, callback):
    if callback in self._callback:
      self._callback.remove(callback)

  def subscribe(self, *arr):
    if arr:
      for a in arr:
        self.prx.subscribe(a, self.thepr)
    else:
      self.prx.subscribe("*", self.thepr)
      print "subscribe--"

  def unsubscribe(self, *arr):
    if arr:
      for a in arr:
        self.prx.unsubscribe(a, self.thepr)
    else:
      self.prx.unsubscribe("*", self.thepr)

  def reload(self):
    self.prx.reload()

  def query(self, name):
    return self.prx.query(name)

#t = time.time()
#  for i in range(10000):
#    prx.subscribe("common",adp)
#  e = time.time() - t;
#print "1w call: " , e
  


if __name__ ==  "__main__":
  d = dbdesc()
  d.reload()
