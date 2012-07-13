#! /usr/bin/env python
import sys
sys.path.append('/data/xce')
sys.path.append('/data/xce/IcePy-3.3.0/python')
import traceback, Ice, time
import random
from AdQualityAdapter import *

print '\t功能选择：'
print '\tGetLastUpdateTime-----输入0（默认）'
print '\tGetQualitys-----------输入1'
print '\treload----------------输入2'
print '\t退出------------------输入exit'

moduleNO = raw_input('请选择功能模块: ')

adapter = AdQualityAdapter(0)

if moduleNO == '0' or moduleNO == '':
  print '-----------------------GetLastUpdateTime------------------------------------'
  print "The last update time is: %s" %adapter.GetLastUpdateTime()
  
elif moduleNO == '1':
  print '-----------------------GetQualitys------------------------------------------'
  user_key = raw_input('请选择输入user_key: ')
  print adapter.GetQualitys(user_key)
  
elif moduleNO == '2':
  print '-----------------------reload-----------------------------------------------'
  print "reloading ... ..."
  adapter.reload()
  print "reload Done" 
elif moduleNO == 'exit':
  exit()
else:
  print "输入错误，请重新输入！"
  exit()


