#-*- coding:utf-8 -*- 
import sys 
import time
import StringIO

from logger import *

def printxx(trace):
  trace.print_exc()
  
logger = Logger("xx", "./", "hourly")

try:
  raise NameError, 'HiThere' 
except:
  #rintxx(traceback)
  #p = StringIO.StringIO()
  #raceback.print_exc(file=fp)
  #essage = fp.getvalue()
  #print message
  logger.error("xx", traceback)

