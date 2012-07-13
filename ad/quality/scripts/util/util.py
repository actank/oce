import os
import sys
import glob
import time
import fcntl

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
    log_list = glob.glob("stat.*")
    readfiles = self.getReadyFiles()
    for logs in log_list:
      log_time = logs.split(".")[1]
      if(log_time in readfiles):
        print "read file : " + log_time
        ans.append(logs)
    self.updateLastReadTime(self.getBeforNHour(1))
    return ans
    os.chdir(self.now_path)

  def getLogsByLastUpdateTime(self, begin_minute, end_minute, path = "/data/xce/Ad/log/AdLogS0/ad_stat"):
    ans = []
   #last_update_time = time.strptime(last_update_time, "%Y-%m-%d %H:%M:%S")
   #begin_minute = time.localtime(time.mktime(last_update_time) + 60)
   #end_minute = time.localtime(time.time() - 60)
    while (time.mktime(begin_minute) < time.mktime(end_minute)):
      tstr = time.strftime("%Y-%m-%d-%H-%M-%S", begin_minute)
      tlist = tstr.split("-")
      ans.append(("%s/%s/%s/%s/%s/stat_val_%s_%s_%s_%s_%s.log.gz") %(path, tlist[0], tlist[1], tlist[2], tlist[3], tlist[0], tlist[1], tlist[2], tlist[3], tlist[4]))
      begin_minute = time.localtime(time.mktime(begin_minute) + 60)
    return ans

  def makeDictByLog(self, log):
    ans = {}
    log.strip()
    kv = log.split("\t")
    for ele in kv:
      list = ele.split("=")
      ans[list[0]] = list[1]
    return ans
  def getCountOfThisProcess(self, process_name):
    count = 0;
    pscmd = "ps aux"
    procs = os.popen(pscmd).read()
    procarr = procs.split("\n")
    for line in procarr:
      #rint line
      if line.find(process_name) != -1:
        count += 1
    return count

  def EnterQualityMinutely(self):
    result = False
    f = open('/data/xce/zhaohui.tang/python/util/AD_QUALITY_MINUTELY_STATUS', 'r+w')
    fcntl.flock(f, fcntl.LOCK_EX)
    if f.readline() == 'True':
      result = True
    else:
      f.write('True')
    fcntl.flock(f,fcntl.LOCK_UN)
    f.close()
    return result

  def ExitQualityMinutely(self):
    f = open('/data/xce/zhaohui.tang/python/util/AD_QUALITY_MINUTELY_STATUS', 'w')
    fcntl.flock(f, fcntl.LOCK_EX)
    f.write('')
    fcntl.flock(f,fcntl.LOCK_UN)
    f.close()

  
