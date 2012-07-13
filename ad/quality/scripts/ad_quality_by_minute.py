import os
import sys
sys.path.append('/data/xce/zhaohui.tang/python/util')
from util import Util
import time
import gzip
import re 
from mid_mail import MidMail
from AdQualityAdapter import *

midmail = MidMail()
util = Util()

stat_map = {}

class StatElement(object):
  def __init__(self, user_key, group_id, time):
    self.pv_count = 0
    self.click_count= 0
    self.user_key = user_key
    self.group_id = group_id
    self.time = time
  def add(self, mp):
    print self.pv_count
    if("pvlog" in mp):
      self.pv_count += 1
    elif("clicklog" in mp):
      self.click_count += 1
  def insertIntoMap(self, stat_map):
    stat = AdStat()
    stat.userKey = self.user_key 
    #rint stat.userKey
    stat.groupId = self.group_id
    #rint stat.groupId
    stat.pvCount = self.pv_count
    stat.clickCount = self.click_count
    stat.time = self.time
    #rint stat.pvCount
    #rint stat.clickCount
    #rint stat.time
    #seq.append(stat)
    if (stat.userKey in stat_map):
      stat_map[stat.userKey].append(stat)
    else:
			seq = []
			seq.append(stat)
			stat_map[stat.userKey] = seq

data = {}

def insert(ele):
  if(("user_key" in ele) and ("group_id" in ele)):
    key = "%s\t%s"%(ele["user_key"], ele["group_id"])
    if(key not in data):
      data[key] = StatElement(ele["user_key"], long(ele["group_id"]), ele["time"])
    data[key].add(ele)
  else:
    print "error"

path = "/data/xce/Ad/log/AdLogS0/stat_log"
adapter = AdQualityAdapter(0)
last_update_time = "-1"
#last_update_time = "2010-12-10 13:37:00"
try:
  last_update_time = adapter.GetLastUpdateTime()
except:
  print "adapter.Get last update time error"
  traceback.print_exc()
  midmail.send("quality adapter get last update time error", "zhaohui.tang@opi-corp.com")
  sys.exit(0)

print last_update_time
if re.match('\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d', last_update_time) is None:
  print "last update time format error"
  sys.exit(0)

last_update_time = time.strptime(last_update_time, "%Y-%m-%d %H:%M:%S")
begin_minute = time.localtime(time.mktime(last_update_time) + 60) # 从上次更新时间的下一分开始
end_minute = time.localtime(time.time() - 60) # 结束时间为当前时间的前一分钟

if time.mktime(end_minute) - time.mktime(begin_minute) > 60 * 60: #检查上次更新时间是否过长，一小时前
  errlog = "last update time is %s, which is one hour before" %last_update_time
  midmail.send(errlog, "zhaohui.tang@opi-corp.com")
  sys.exit(0)
  
last_update_time = time.strftime("%Y-%m-%d %H:%M:00", end_minute)  #更改last update time 为本次更新时间
print last_update_time

logs = util.getLogsByLastUpdateTime(begin_minute, end_minute, path)
for filename in logs:
  print filename
  try:
      file = gzip.open(filename, "r")
      for line in file:
        if(len(line) <= 1):
          continue
        mp = util.makeDictByLog(line)
        mp["time"] = last_update_time
        insert(mp)
      file.close()
      print "%s is done." %filename
  except:
    errlog = "deal file %s error" %filename
    traceback.print_exc()
    midmail.send(errlog, "zhaohui.tang@opi-corp.com")

for ele in data:
  data[ele].insertIntoMap(stat_map)

#or key in stat_map:
# print "---------------------------------------"
# print key 
# print stat_map[key]

print "the stat_map size = %d" %len(stat_map)
try:
  adapter.update(stat_map)
except:
  errlog = "adapter.update error"
  traceback.print_exc()
  midmail.send(errlog, "zhaohui.tang@opi-corp.com")
print "Done"
sys.exit(0)
