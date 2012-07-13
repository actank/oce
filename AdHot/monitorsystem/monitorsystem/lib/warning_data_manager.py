import sys
import rrdtool

#sys.path.append("/data/xce/MySQL-python/MySQL-python-1.2.3/build/lib.linux-x86_64-2.4/MySQLdb")
sys.path.append("/data/xce/MySQL-python/MySQL-python-1.2.3/build/lib.linux-x86_64-2.4/")
import MySQLdb
import time;

import logging
import codecs


from app_globals import Globals as g

from common import Util 

log = logging.getLogger(__name__)

class WarningInfo:
  def __init__(self):
    self.unixtime = 0;
    self.localtime = "";
    self.chartname = "";
    self.warninginfo = "";


host = "10.3.16.210"
dbname = "stat"
tablename = "monitor_info";
username = "work@RR"
passwd = "Geeker4ZolZ"    
    
    

class WarningDataManager:
  
  def __init__(self):
    self.m_warning_list = list()
    self.new_warning_flag = False;
  
  def unixTime2LocalTime(self, t):
    lt = time.localtime(t);
    if(lt.tm_hour < 10):
      str_hour = "0" + str(lt.tm_hour);
    else:
      str_hour = str(lt.tm_hour);
      
    if(lt.tm_min < 10):
      str_min = "0" + str(lt.tm_min);
    else:
      str_min = str(lt.tm_min);  
        
    return str_hour + ":" + str_min
  
     
  def Compare(self, info):
       return info.unixtime;
  
  def clearLastDayData(self):
    t = time.time()
    lt = time.localtime(t);
    if(lt.tm_hour == 3):
      del g.warning_list[:]
      
  def SaveRecord(self, info, mode):
    if(int(mode) == 0):
      if(self._SaveRecord(g.warning_list, info, 0) == True):
        self.new_warning_flag = True;
    elif(int(mode) == 1):
      self._SaveRecord(self.m_warning_list, info, 1);
       
    return;
   
  def _SaveRecord(self, warning_list, info, mode):
    Max_Warning_List_Len = 15;
                
    if(len(warning_list) == Max_Warning_List_Len and mode == 0):
      warning_list.pop(Max_Warning_List_Len - 1);
    
    if(len(warning_list) == 0):  
      warning_list.insert(0, info);
    
#    print info.chartname + "  " + str(info.localtime) + "  " + str(info.unixtime) 
                  
    for item in warning_list:
      if(item.chartname == info.chartname):
        if(item.unixtime < info.unixtime and item.unixtime > info.unixtime - 5 * 60):
          item.unixtime = info.unixtime;
          item.localtime = info.localtime
#          log.debug("----Branch 1")
          return True;
        elif(item.unixtime < info.unixtime and item.unixtime < info.unixtime - 5 * 60):
          warning_list.insert(0, info);
          return True;
        else:
#          print info.chartname + "  " + str(info.localtime) + "  " + str(info.unixtime)
          return False;
    
    warning_list.insert(0, info);
#    log.debug("----Branch 2")
    return True;   
  
  def getNewWarningFlag(self):
    return self.new_warning_flag;
     
  def _getWarningList(self, pass_time, mode = 0, end = "now"):
    
#    log.debug("len of global list: %d ", len(g.warning_list))
    
#    self.clearLastDayData();
    new_warning_flag = False;
#    host = "10.2.19.157"
#    tablename = "stat_user_config";
#    waring_list = list();
    
    
    conn = MySQLdb.connect(host, username, passwd) 
    conn.select_db(dbname)
    cursor = conn.cursor()
  
    sql = "select chartname, num, limit_value, warning_info from " + tablename;
#    sql = "select chartname, num, limit_value, warning_info from " + tablename + " where chartname=\"feednewsTimeout_FeedNews\"";
   
    count = cursor.execute(sql)
    
    results = cursor.fetchmany(count)   

    for r in results:
      chartname = str(r[0])
      num = int(r[1]);
      warning_value = int(r[2]);
      warning_info = unicode(str(r[3]), "utf-8");
#      warning_info = "";
      
      if(warning_value == 0):
        continue;
      
      for i in range(0, num):
        rrd_path = "/data/xce/monitor/data/" + str(chartname) + str(i) + ".rrd";
        start_time_expr = "now-" + str(pass_time)
        end_time_expr = str(end);
        res = rrdtool.fetch(rrd_path, "AVERAGE", "--start", start_time_expr, "--end", end);
      
        if(res):
          start_time = res[0][0];
          end_time = res[0][1];  
          step = res[0][2];
           
          record_num = len(res[2]) - 1
           
          for j in range(0, record_num):
              value = res[2][j][0];
              if(value == None):
                continue;
              
#              if(chartname.find("dispatchTime_FeedAssistantN") != -1 and i == 7):
#                unixtime = start_time + j * step;
#                localtime = self.unixTime2LocalTime(unixtime);
#                print  str(j) + " " + str(chartname) + str(i) + "  " + localtime + "  " + str(float(value));
 
              if(warning_value > 0 and value > warning_value) or (warning_value < 0 and value < (-warning_value)):
                info = WarningInfo()
                info.chartname = str(chartname);
                info.unixtime = start_time + j * step;
                info.localtime = self.unixTime2LocalTime(info.unixtime);
                info.warninginfo = warning_info 
                self.SaveRecord(info, mode);
                
    if(int(mode) == 0):
      g.warning_list.sort(key = self.Compare);
      g.warning_list.reverse();
      return g.warning_list;
    elif(int(mode) == 1):
      self.m_warning_list.sort(key = self.Compare);
#      self.m_warning_list.reverse();
      return self.m_warning_list;
   
  def getRealTimeWarnings(self):
    t = 60 * 60 * 3 ;
    return self._getWarningList(t);
    
  def getLastNightWarnings(self):
    now_time = int(time.time());
    lt = time.localtime(now_time);
    now_hour = int(lt.tm_hour);
    end = "now"
    
    if(now_hour > 9):
      hour = 9;
      end = "now-" + str((now_hour - hour) * 3600);
      
    passed_time = (4 + now_hour) * 3600;
#    print "passed_time: " + str(passed_time)
#    print "end: " + str(end)
    
    return self._getWarningList(passed_time, 1, end);
  
#  def getFirstLevelInfo(self):
#    d = dict();
#    warning_list = list();
#    
#    for info in g.warning_list:
#      chart_prefix = Util.getChartPrefix(info.chartname)
#      
#      if(d.has_key(chart_prefix) == False):
#        d[chart_prefix] = info.unixtime
#      else:
#        if(info.unixtime > d[chart_prefix]):
#          d[chart_prefix] = info.unixtime  
#        
#    
#    for key in d:
#      new_info = WarningInfo()
#      new_info.chartname = key;
#      new_info.localtime = self.unixTime2LocalTime(d[key])
#      warning_list.append(new_info);
#              
#    return warning_list

#  def SaveRecord(self, info):
#    Max_Warning_List_Len = 30;
#                
#    if(len(g.warning_list) == Max_Warning_List_Len):
#      g.warning_list.pop(Max_Warning_List_Len - 1);
#    
#    if(len(g.warning_list) == 0):  
#      g.warning_list.insert(0, info);
#                  
#    for item in g.warning_list:
#      if(item.unixtime == info.unixtime and item.chartname == info.chartname):
#        return;
#        
#    g.warning_list.insert(0, info);
#    return True;             
#class FirstLevelWaringInfo:

#  def __init__(self):
#    self.localtime = "";
#    self.chartname = "";
#    self.num = 0;    
      
def test1():
  print "begin global list len: " + str(len(g.warning_list))
  manager = WarningDataManager();
  manager.getRealTimeWarnings();
  
  for info in g.warning_list:
    print info.chartname
    print info.localtime
  
  print "Has new Warning: " 
  print "end global list len: " + str(len(g.warning_list))



def test2():
  print "Get last night warings:"
  manager = WarningDataManager();
  res = list(manager.getLastNightWarnings());
  for info in res:
    print info.chartname
    print info.warninginfo
    print info.localtime
  
    
#test2()  