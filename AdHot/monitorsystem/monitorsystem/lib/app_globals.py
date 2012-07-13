"""The application's Globals object"""

from beaker.cache import CacheManager
from beaker.util import parse_cache_config_options

import logging
import time
import threading

from  data_checker import DataCheckerThead 

log = logging.getLogger(__name__)

#def sayhello():
#  log.debug("Say Hello!! ")
#  global t        #Notice: use global variable!
#  t = threading.Timer(5.0, sayhello)
#  t.start()


class Globals(object):
    """Globals acts as a container for objects available throughout the
    life of the application

    """
    warning_list = list()
    def __init__(self, config):
        """One instance of Globals is created during application
        initialization and is available during requests via the
        'app_globals' variable

        """
        self.cache = CacheManager(**parse_cache_config_options(config))
        self.warning_list = list();
#        DataChecker.runChecker();
        self.startChecker();
        log.debug("Globals init------- ")
        
#        t = threading.Timer(5.0, sayhello)
#        t.start()
  
    def startChecker(self):
      t = DataCheckerThead();
      t.setDaemon(True);
      t.start();
    