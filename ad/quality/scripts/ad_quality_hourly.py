import os
import sys
sys.path.append('/data/xce/zhaohui.tang/python/util')
from mid_db import MidDb
from util import Util
import MySQLdb
import time
from logger import *
from mid_mail import MidMail
sqlstrs = []

logger = Logger("log_h", "/data/xce/zhaohui.tang/python/log", "hourly")
middb = MidDb()
middb.initDbConnection('10.3.22.69', 'work@RR', 'Geeker4ZolZ', 'adn')
#middb.initDbConnection('10.2.19.161', 'xce', 'xce1234', 'adn')

midmail = MidMail()
senderr = 0
senderr_msg = ""

util = Util()
class StatElement(object):
	def __init__(self, user_key, group_id, db_time):
		counts= {}
		counts["pv_count"] = 0
		counts["click_count"] = 0
		self.counts = counts
		self.user_key = user_key
		self.group_id = group_id
		self.db_time = db_time
	def add(self, mp):
		if("pvlog" in mp):
			self.counts["pv_count"] += 1
		elif("clicklog" in mp):
			self.counts["click_count"] += 1

	def insertIntoDb(self):
		try:
			sql = "insert into ad_quality_basic (user_key, group_id, pv_count, click_count, time) values(%s, %s, %s, %s, '%s') on duplicate key update user_key= values(user_key) "%(self.user_key, self.group_id, self.counts["pv_count"], self.counts["click_count"], self.db_time)
                        logger.log(sql)
			middb.doInsert(sql)
		except:
			global senderr
			global senderr_msg
			senderr = 1
			senderr_msg = "ad_quality_system err : xce@t8.xntalk.d.xiaonei.com:/data/xce/Ad/python ==> insert db error"
                        logger.info("%s, %s, %s insert error\n" %(self.user_key, self,group_id, self.db_time))



data = {}

def insert(ele):
	if(("user_key" in ele) and ("group_id" in ele) and ("db_time" in ele)):
		key = "%s\t%s"%(ele["user_key"], ele["group_id"])
		if(key not in data):
			data[key] = StatElement(ele["user_key"], ele["group_id"], ele["db_time"])
		data[key].add(ele)
		#sql += "on duplicate key update pv_count = "
	else:
		print "error"

logger.info("Enter ad_quality_hourly.py")

path = "/data/xce/Ad/log/AdLogS0/stat"
#ans = ["/data/xce/Ad/log/AdLogS0/stat/stat.2010-12-08-16"]#util.getLastHourLogs(path)
#ans = ["stat.2010-12-06-11"]#util.getLastHourLogs(path)
ans = util.getLastHourLogs(path)
for filename in ans:
  print filename
for filename in ans:
        file = open(filename, "r")
        time = filename.split(".")[1]
        tlist = time.split("-")
        for line in file:
                if(len(line) <= 1):
                	continue
                mp = util.makeDictByLog(line)
                mp["db_time"] = ("%s-%s-%s %s:00:00") %(tlist[0], tlist[1], tlist[2], tlist[3])
                insert(mp)
        file.close()
        for ele in data:
        	data[ele].insertIntoDb()
        logger.info("%s is done" %filename)
middb.closeDbConnection()
if(senderr == 1):
	midmail.send(senderr_msg, "zhaohui.tang@opi-corp.com")

logger.info("Exit ad_quality_hourly.py")
