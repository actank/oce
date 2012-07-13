#!/usr/bin/env python

import sys, traceback, Ice, IceGrid

Ice.loadSlice('-I.. DbDescriptor.ice')
from com.xiaonei import xce

def menu():
  print """
usage:
t: send greeting as twoway
s: shutdown server
x: exit
?: help
"""

def _print(d):
  if xce.DbInstance == type(d):
    print '<DbInstance>', d.name, d.type, d.timestamp, "w:", d.wserver, "r:", d.rservers

class NotifyToMe(xce.DbObserver):
  def updated(self, instance, current=None):
		_print(instance)


def main():
  ic = Ice.initialize(["--Ice.Default.Locator=IceGrid/Locator:default -h 10.2.18.183 -p 4061"])

  id = ic.stringToProxy("DbDescriptor@TheShop1")
  dd = DbDescriptorPrx.checkedCast(id)

  # adp = ic.createObjectAdapter('testob')

  # dd.subscribe('*', 

#  dd.reload()
#  return
  pass





if __name__ ==  "__main__":
  main()
