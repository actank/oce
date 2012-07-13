import os
import sys
sys.path.append('/data/xce/zhaohui.tang/python/util')
from util import Util
from logger import *
import time
import gzip
import re 
from AdQualityAdapter import *

util = Util()
logger = Logger("log_m", "/data/xce/zhaohui.tang/python/log", "minutely")

if util.EnterQualityMinutely():  # if running return True,else  change the status to be running
  log = "The last is also running, so exit!"
  logger.error(log)
  sys.exit(0)

stat_map = {}

class StatElement(object):
  def __init__(self, user_key, group_id, time):
    self.pv_count = 0
    self.click_count= 0
    self.user_key = user_key
    self.group_id = group_id
    self.time = time
  def add(self, mp):
    if("pvlog" in mp):
      self.pv_count += 1
    elif("clicklog" in mp):
      self.click_count += 1
  def insertIntoMap(self, stat_map):
    stat = AdStat()
    stat.userKey = self.user_key 
    stat.groupId = self.group_id
    stat.pvCount = self.pv_count
    stat.clickCount = self.click_count
    stat.time = self.time
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
    logger.info("This line don't has user_key or group_id, error")

adapter = AdQualityAdapter(0)

###################################get last update time######################################################
last_update_time = "-1"
#ast_update_time = "2010-12-11 10:43:00"
try:
  last_update_time = adapter.GetLastUpdateTime()
except:
  logger.error("quality adapter get last update time error", traceback)
  util.ExitQualityMinutely() # exit change the status
  sys.exit(0)

logger.info("The last update time : %s" %last_update_time)
if re.match('\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d', last_update_time) is None:
  logger.info("the last update time don't match format '%Y-%m-%d %H:%M:%S' error")
  util.ExitQualityMinutely() # exit change the status
  sys.exit(0)

last_update_time = time.strptime(last_update_time, "%Y-%m-%d %H:%M:%S")
begin_minute = time.localtime(time.mktime(last_update_time) + 60) # 从上次更新时间的下一分开始
end_minute = time.localtime(time.time() - 60) # 结束时间为当前时间的前一分钟
logger.info("the begin_minute is : %s, and the end_minute is : %s" %(begin_minute, end_minute))

if time.mktime(end_minute) - time.mktime(begin_minute) > 60 * 60: #检查上次更新时间是否过长，一小时前
  logger.error("last update time is %s, which is one hour before" %last_update_time)
  util.ExitQualityMinutely() # exit change the status
  sys.exit(0)
  
last_update_time = time.strftime("%Y-%m-%d %H:%M:00", end_minute)  #更改last update time 为本次更新时间
logger.info("the last update time now is : %s" %last_update_time)

###################################分析log统计数据######################################################
path = "/data/xce/Ad/log/AdLogS0/stat_log"
logs = util.getLogsByLastUpdateTime(begin_minute, end_minute, path)

for filename in logs:
  logger.info(filename)

for filename in logs:
  #rint filename
  try:
      file = gzip.open(filename, "r")
      for line in file:
        if(len(line) <= 1):
          continue
        mp = util.makeDictByLog(line)
        mp["time"] = last_update_time
        insert(mp)
      file.close()
      logger.info("%s is done." %filename)
  except:
    logger.error("deal file %s error: ", traceback)

###################################send update request######################################################
try:
  for ele in data:
    data[ele].insertIntoMap(stat_map)

  logger.info("the stat_map size = %d" %len(stat_map))
  adapter.update(stat_map)
except:
  logger.error("adapter.update error: ", traceback)
  util.ExitQualityMinutely() # exit change the status
  sys.exit(0)

print "All Done"

util.ExitQualityMinutely() # exit change the status
sys.exit(0)
