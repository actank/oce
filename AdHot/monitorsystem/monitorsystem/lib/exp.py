import sys
#sys.path.append("/data/xce/MySQL-python/MySQL-python-1.2.3/build/lib.linux-x86_64-2.4/MySQLdb")
sys.path.append("/data/xce/MySQL-python/MySQL-python-1.2.3/build/lib.linux-x86_64-2.4/")
import MySQLdb

import codecs

class ChartInfo:
  
  def __init__(self):
    self.catagory = ""
    self.chartname = ''
    self.description = ''
    self.num = 0
    self.datasource = ''
    self.limit_value = 0


def GetAllCatagory():
  
  host = "10.2.19.157"
  tablename = "stat_user_config";
  
  conn = MySQLdb.connect(host, 'wu', '') 

  conn.select_db("stat")
  cursor = conn.cursor()
  
    
  sql = "select catagory from " + tablename; 
   
   
  count = cursor.execute(sql)
    
  results = cursor.fetchmany(count)   
  
  
  res = set()
  
  for r in results:
      catagory = r[0]
      res.add(catagory);
  
  return res  
  
    
def GetChartInfoByChartName(chartname):
  host = "10.2.19.157"
  tablename = "stat_user_config";
  
  conn = MySQLdb.connect(host, 'wu', '') 

  conn.select_db("stat")
  cursor = conn.cursor()
  
    
  sql = "select * from " + tablename + " where chartname=" + "\"" + str(chartname) + "\""
  print sql
   
   
  count = cursor.execute(sql)
    
  results = cursor.fetchmany(count)   
  
  
  chartInfo = ChartInfo();
  
  for r in results:
#      print r[0];
#      print r[1];
#      print r[2];
#      print r[3];
      chartInfo.catagory = r[0]
      chartInfo.chartname = r[1]
#      print chartInfo.chartname;
      
      chartInfo.description = unicode(str(r[2]), "utf-8")
      #chartInfo.description = str(r[2])
      chartInfo.num = r[3]
      chartInfo.datasource = r[4]
      chartInfo.limit_value = r[5]
  
  return chartInfo



def GetChartInfo(name):
  
  host = "10.2.16.119"
  tablename = "stat_user_config";
  
#  conn = MySQLdb.connect(host, 'work@RR', 'Geeker4ZolZ')
  conn = MySQLdb.connect(host, 'root', '123') 

  conn.select_db("stat")
  cursor = conn.cursor()
  
    
  sql = "select * from " + tablename + " where catagory=" + "\"" + str(name) + "\""
  print sql
#  sql = "select * from stat_user_config where name=\"FeedNews\""
   
#  sql = "select * from " + tablename 
   
  count = cursor.execute(sql)
    
  results = cursor.fetchmany(count)   
  
#  chartInfo = ChartInfo();
  
  res = list()
  
  for r in results:
#      print r[0];
#      print r[1];
#      print r[2];
#      print r[3];
      chartInfo = ChartInfo();
      chartInfo.catagory = r[0]
      chartInfo.chartname = r[1]
#      print chartInfo.chartname;
      
      #chartInfo.description = unicode(str(r[2]),"utf-8") 
      chartInfo.description = str(r[2])
      chartInfo.num = r[3]
      chartInfo.datasource = r[4]
      chartInfo.limit_value = r[5]
      res.append(chartInfo)
  
  return res
  

def test1():

  infolist = GetChartInfo("FeedNews")
  print len(infolist)
  for info in infolist:
    print info.catagory
    print info.chartname
    print info.datasource
    print info.description
    print info.num
    print info.limit_value
#


def test2():

  info = GetChartInfoByChartName("zeroRatio_FeedNews")
  print info.catagory
  print info.chartname
  print info.datasource
  print info.description
  print info.num
  print info.limit_value


def test3():

  res = GetAllCatagory();
  print len(res)
  for cata in res:
    print cata

  
test1()
#test1()  
#test3()
