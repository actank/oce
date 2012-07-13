# -*- coding: utf8 -*-
import logging

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from feedtpladmin.lib.base import BaseController, render

from sqlalchemy import *
from sqlalchemy.engine import create_engine
import MySQLdb

from feedtpladmin.model.stypekeys import StypeKeys
from feedtpladmin.model.stypetemplate import StypeTemplate
from feedtpladmin.model.stypeconfig import StypeConfig
from feedtpladmin.lib.base import Session

import sys
sys.path.append("/usr/lib/python2.6/site-packages/")

import neo_cgi
import neo_util
import neo_cs
import urllib2

log = logging.getLogger(__name__)

class HelloController(BaseController):
  def __before__(self):
    # model 加载数据
    self.page_q = Session.query(StypeKeys)
    self.page_config = Session.query(StypeConfig)

  def index(self):
    return render('/hello.mako')

  def index(self):
    return render('/feedtpl.mako')

  def SaveStypeConfig(self):
    cfg = self.page_config.filter_by(stype=int(request.params['stype'])).first()
    if not cfg:
      cfg = StypeConfig(int(request.params['stype']), 
          request.params['title'], 
          int(request.params['weight']), 
          int(request.params['send-news']), 
          request.params['news-merge-by'], 
          int(request.params['news-merge-type']), 
          int(request.params['send-mini']), 
          request.params['mini-merge-by'], 
          int(request.params['mini-merge-type']), 
          request.params['extra-configs'],
          0)
      Session.add(cfg)
    else:
      cfg.title = request.params['title']
      cfg.weight = int(request.params['weight'])
      cfg.send_newsfeed = int(request.params['send-news'])
      cfg.news_merge_by = request.params['news-merge-by']
      cfg.news_merge_type = int(request.params['news-merge-type'])
      cfg.send_minifeed = int(request.params['send-mini'])
      cfg.mini_merge_by = request.params['mini-merge-by']
      cfg.mini_merge_type = int(request.params['mini-merge-type'])
      cfg.extras = request.params['extra-configs']

    Session.commit()
    return 'config保存成功'

  def SaveStypeKeys(self):
    sk = StypeKeys(request.params['stype'], 
        version = request.params['version'], 
        key_list = request.params['kl'])
    #Session.save(sk)
    Session.merge(sk)
    Session.commit()
    return 'save ok, ', request.params['kl']

  def GetStypeList(self):
    rsp = '['
    count = 0
    for o in self.page_config.all():
      if count > 0:
        rsp += ','
      rsp += '{"id":' + str(count) + ', "stype":' + str(o.stype) + ', "title":"' + o.title + '", "state":' + str(o.state) + '}'
      count += 1
    rsp += ']'
    return rsp

  def SetStypeState(self):
    o = self.page_config.filter_by(stype=int(request.params['stype'])).first()
    o.state = int(request.params['state'])
    Session.commit()
    return str(o.state)


  def GetStypeConfig(self):
    o = self.page_config.filter_by(stype=int(request.params['id'])).first()
    res = '{"stype":' + str(o.stype) + ', "title":"' + o.title + '", "weight":' + str(o.weight) + ',"send_newsfeed":' + str(o.send_newsfeed) + ',"news_merge_by":"' + o.news_merge_by + '","news_merge_type":' + str(o.news_merge_type) + ',"send_minifeed":' + str(o.send_minifeed) + ',"mini_merge_by":"' + str(o.mini_merge_by) + '", "mini_merge_type":' + str(o.mini_merge_type) + ', "extras":"' + str(o.extras) + '"}';
    return res

  def GetStypeKeys(self):
    o = self.page_q.filter_by(stype=int(request.params['stype']), version=int(request.params['version'])).first()
    return o.key_list

  def GetStypeVersions(self):
    versions = self.page_q.filter_by(stype=int(request.params['stype']))
    ret = '['
    count = 0
    for o in versions:
      if count > 0:
        ret += ','
      ret += str(o.version)
      count += 1
    ret += ']'
    return ret


  def GetStypeTree(self):
    o = self.page_q.filter_by(id=int(request.params['id'])).first()
    value = ''
    if o:
      value = o.key_list
    keys = value.split(',')
    return render('/get-stype-tree.mako', {'keys' : keys})

  def SaveTemplate(self):
    sk = StypeTemplate(request.params['stype'], 
        version = request.params['version'], 
        view = request.params['view'],
        template = request.params['tpl'])
    Session.merge(sk)
    Session.commit()
    return '保存模板成功2'

  def RemoveTemplate(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])
    view = int(request.params['view'])

    if stype <= 0 or view <= 0:
      return '参数错误'
    try:
      conn = MySQLdb.connect(host="127.0.0.1",user="root",passwd="",db="xfeed_tpl")
      cursor = conn.cursor()
      sql = "DELETE FROM tpl_content WHERE stype=%d AND version=%d AND view=%d" % (stype, version, view)
      print sql 
      cursor.execute(sql)
      conn.commit();
      cursor.close()
      conn.close()
    except:
      return '数据库错误'
    return '删除模板成功'

  def GetStypeTpls(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])
    if stype <= 0:
      return '参数错误'
    engine = create_engine('mysql://root@127.0.0.1/xfeed_tpl?charset=utf8', convert_unicode = True)
    connection = engine.connect()
    result = connection.execute("select view, template from tpl_content where stype=%d AND version=%d;" % (stype, version))

    count = 0
    rsp = '{'
    for row in result:
      print row['view'], row['template']
      if count > 0:
        rsp += ','
      rsp += str('\'' + str(row['view'])) + '\':\'' + str(row['template']).replace('\'', '\\\'') + '\''
      count += 1
    rsp += '}'
    connection.close()
    return rsp

  def ValidateTemplate(self):
    stype = int(request.params['stype'])
    tpl = request.params['tpl']
    if stype <= 0 or not tpl:
      return '请求参数错误'
    hdf = neo_util.HDF()  # create an HDF dataset
    cs = neo_cs.CS(hdf)
    try:
      cs.parseStr(urllib2.unquote(str(tpl)))
      cs.render()
    except neo_util.ParseError, e:
      return "错误:",str(e)
    return '语法检查通过'

