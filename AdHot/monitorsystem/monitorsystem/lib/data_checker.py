
import rrdtool
import logging
import time

import threading
import urllib, urllib2

log = logging.getLogger(__name__)

class DataCheckerThead(threading.Thread):
  def __init__(self):
      threading.Thread.__init__(self)
      return;
  
  def run(self):
        while True:
          log.debug("------------------->Check Data Thread Start!!")
          DataCheckerThead.checkData();
          time.sleep(10);
  
  @staticmethod
  def checkData():
    waring_list = list()
#    chartname = "totalResRatio_FeedMemcProxy"
    chartname = "firstPageGetTimes_FeedNews"
    sevice_name = "FeedNews"
    
    for i in range(0, 20):
      rrd_path = "/data/xce/monitor/data/" + str(chartname) + str(i) + ".rrd";
      res = rrdtool.fetch(rrd_path, "AVERAGE", "--start", "now-300", "--end", "now");
      waring_flag = True;
       	
      
      if(res):
          start_time = res[0][0];
          end_time = res[0][1];  
          step = res[0][2];
           
          record_num = len(res[2]) - 1
           
          for j in range(0, record_num):
              value = res[2][j][0];
              if(value != None and value != 0):
                waring_flag = False
                break;
	      if(value==0 and i==17):
    		log.debug("value0: %s",str(i))
                waring_flag = False
		break;
              
          if(waring_flag == True):    
            service = str(sevice_name) + str(i)
            waring_info = str(service) + "_get_0_times_maybe_it_is_blocked!!"
            waring_list.append(waring_info);
            log.debug("------------------->%s Warning Info Found!!", str(service))
    
    #end for        
    log.debug("------Enter Middle part")

    if(len(waring_list) == 20):
      log.debug("Service has problem!!")
      msg = "FeedNews_has_problem!!!"
      DataCheckerThead.warning(msg)

    elif(len(waring_list) < 20 and len(waring_list) > 0):
      for info in waring_list:
        log.debug(info);
        msg = str(info);
        DataCheckerThead.warning(msg)

    del waring_list[:]     
  
  @staticmethod
  def sendMsg(number, message):
    log.debug("Send Msg!!")
    loginUrl = 'http://10.22.198.81:2000/receiver'
    _str = urllib.urlencode({'number': number, 'message': message})
    _response = urllib2.urlopen(loginUrl, _str)
  
  @staticmethod
  def warning(message):
    #wujiang
    DataCheckerThead.sendMsg(15801638536, message);
   
    #fei.yuan
    DataCheckerThead.sendMsg(13552482899, message);
    
    #jingwei.ye
    DataCheckerThead.sendMsg(13810334135, message);
    
