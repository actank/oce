import os
import sys
import glob
import time

class Util(object):
	def makeDayStr(self, h):
        	if(h < 10):
                	return "0" + str(h)
        	else:
                	return str(h)
                
	def getBeforNDay(self, n):
		n = n * 24
		return time.strftime('%Y-%m-%d %X',time.localtime(time.time()-n*60*60)).split(' ')[0]
	def getBeforNHour(self, n, format = '%Y-%m-%d-%H'):
		return time.strftime(format ,time.localtime(time.time()-n*60*60))
	def getReadyFiles(self):
		ans = []
		timestamp = time.time()
		if(self.now_hour <= self.lastreadtime):
			return []
		elif(self.now_hour > self.lastreadtime):
			tmp = 0
			while(True):
				timestr = time.strftime('%Y-%m-%d-%H', time.localtime(timestamp - tmp * 60 * 60))
				if(timestr == self.lastreadtime):
					break
				ans.append(timestr)
				tmp += 1
			return ans
	def updateLastReadTime(self, t):
		file = open("/data/xce/zhaohui.tang/python/util/LastReadTime", "w")
		file.write("%s\n" % (t))
		file.close()
	def getLastReadTime(self):
		self.now_hour = self.getBeforNHour(0)
		path = "/data/xce/zhaohui.tang/python/util/LastReadTime"
		if(os.path.exists(path)):
			file = open(path, "r")
			for line in file:
				time = line.strip()
			file.close()
			print "last read time : %s" % time
			return time
		else:
			t = self.getBeforNHour(0)
			self.updateLastReadTime()
	def getLastHourLogs(self, path = "/data/xce/Ad/log/AdLogS0/stat"):
		ans = []
		self.log_path = path
		self.now_path = os.getcwd()
		self.lastreadtime = self.getLastReadTime()
		os.chdir(path)
		log_list = glob.glob("stat.2010-11-25-09")
		print log_list
		#readfiles = self.getReadyFiles()
		#for logs in log_list:
		#	log_time = logs.split(".")[1]
		#	if(log_time in readfiles):
		#		print "read file : " + log_time
		#		ans.append(logs)
		#self.updateLastReadTime(self.getBeforNHour(1))
		return log_list
		os.chdir(self.now_path)

	def makeDictByLog(self, log):
		ans = {}
		log.strip()
		kv = log.split("\t")
		for ele in kv:
			list = ele.split("=")
			ans[list[0]] = list[1]
		return ans
