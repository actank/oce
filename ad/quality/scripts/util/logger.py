#! /usr/bin/env python
import sys
import traceback, time
import StringIO
from mid_mail import MidMail

midmail = MidMail()

class Logger(object):
  def __init__(self, name = "log", path = "/data/xce/zhaohui.tang/log", type = "hourly"):
    self.name = name 
    self.path = path
    self.type = type
    if type == "hourly":
      self.log_file = open(path + "/" + name + "." + time.strftime("%Y-%m-%d-%H"), "a")
    elif type == "minutely":
      self.log_file = open(path + "/" + name + "." + time.strftime("%Y-%m-%d-%H-%M"), "a")
      

  def info(self, str):  # log and print
    print str
    self.log_file.write(str + "\n")

  def log(self, str):
    self.log_file.write(str + "\n")

  def error(self, str):
    midmail.send(str, "zhaohui.tang@opi-corp.com")
    self.log_file.write(str + "\n")
    
  def error(self, str, trace):
    fp = StringIO.StringIO()
    trace.print_exc(file=fp)
    message = "%s: %s" %(str, repr(fp.getvalue()))
    print message
    midmail.send(message, "zhaohui.tang@opi-corp.com")
    self.log_file.write(message + "\n")

  def __del__(self):
    if self.log_file:
      try:
        self.log_file.close()
      except:
        traceback.print_exc()

