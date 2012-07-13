# -*- coding: utf8 -*-
import logging

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from feedtpladmin.lib.base import BaseController, render

from feedtpladmin.model.stypeconfig import StypeConfig
from feedtpladmin.model.dataversion import DataVersion
from feedtpladmin.model.versiontplmapping import VersionTplMapping
from feedtpladmin.model.tplview import TplView

from feedtpladmin.model.applynew import ApplyNewFeed
from feedtpladmin.model.applyversion import ApplyNewVersion
from feedtpladmin.model.applytpl import ApplyNewTpl
from feedtpladmin.model.applyrollback import ApplyRollback
from feedtpladmin.model.applypublish import ApplyPublish


from feedtpladmin.lib.base import Session
from sqlalchemy import func, distinct

import sys
import os
sys.path.append("/usr/lib/python2.6/site-packages/")

import neo_cgi
import neo_util
import neo_cs
import urllib2
import time
import smtplib

log = logging.getLogger(__name__)

class FeedadminController(BaseController):
  def __before__(self):
    self.stype_config_ = Session.query(StypeConfig)
    self.version_tpl_mapping_ = Session.query(VersionTplMapping)
    self.data_version_ = Session.query(DataVersion)
    self.tpl_views_ = Session.query(TplView)
    self.apply_feeds_ = Session.query(ApplyNewFeed)
    self.apply_versions_ = Session.query(ApplyNewVersion)
    self.apply_tpls_ = Session.query(ApplyNewTpl)
    self.apply_rollbacks_ = Session.query(ApplyRollback)
    self.apply_publishs_ = Session.query(ApplyPublish)

  def GetStypeList(self):
    rsp = '['
    count = 0
    for o in self.stype_config_.all():
      if count > 0:
        rsp += ','
      rsp += '{"stype":' + str(o.stype) 
      rsp += ', "persist_body":' + str(o.is_persist_content)
      rsp += ', "persist_feeddb":' + str(o.is_persist_feeddb)
      rsp += ', "news_merge_type":' + str(o.news_merge_type)
      rsp += ', "mini_merge_type":' + str(o.mini_merge_type)
      rsp += ', "push_feed_flags":' + str(o.push_flag)
      rsp += ', "weight":' + str(o.weight)
      rsp += ', "lifetime":' + str(o.lifetime)
      rsp += ', "description":"' + o.description + '"'
      rsp += '}'

      count += 1
    rsp += ']'
    return rsp

  def GetStypeIds(self):
    ids = Session.query(StypeConfig.stype)
    rsp = '['
    count = 0
    for o in ids:
      if count > 0:
        rsp += ','
      rsp += str(o[0])
      count += 1
    rsp += ']'
    return rsp

  def UpdateStypeVersionUsingId(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])
    tpl_id = int(request.params['tpl_id'])
    if stype > 0 and version > 0 and tpl_id > 0:
      ver = self.data_version_.filter_by(stype = stype, version = version).first()
      if ver:
        ver.using_tpl_id = tpl_id
      else:
        ver = DataVersion(stype, version, 0, tpl_id, 0)
        Session.add(ver)
      Session.commit()
      return '保存成功' + str(ver.using_tpl_id)
    return '参数有错误'

  def GetStypeVersionInfo(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])

    if stype > 0 and version > 0:
      ver = self.data_version_.filter_by(stype = stype, version = version).first()
      if ver:
        keys_escaped = ver.keys_xml.replace('"', '\\"');
        return '{"status":' + str(ver.status) + \
            ',"using_id":' + str(ver.using_tpl_id) + \
            ',"test_id":' + str(ver.test_tpl_id) + \
            ',"news_merge_by":"' + str(ver.news_merge_by) + '"' + \
            ',"mini_merge_by":"' + str(ver.mini_merge_by) + '"' + \
            ',"source_by":"' + str(ver.source_by) + '"' + \
            ',"psource_by":"' + str(ver.psource_by) + '"' + \
            ',"actor_by":"' + str(ver.actor_by) + '"' + \
            ',"dispatch_expr":"' + str(ver.dispatch_expr) + '"' + \
            ',"keys_xml":"' + keys_escaped + '"' + \
            '}'
    return '{"status":0, "using_id":-1,"test_id":-1, "news_merge_by":"", "mini_merge_by":"", "source_by":"", "psource_by":"", "actor_by":"", "dispatch_expr":""}'

  def UpdateStypeVersionTestId(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])
    tpl_id = int(request.params['tpl_id'])
    if stype > 0 and version > 0 and tpl_id > 0:
      ver = self.data_version_.filter_by(stype = stype, version = version).first()
      if ver:
        ver.test_tpl_id = tpl_id
      else:
        ver = DataVersion(stype, version, tpl_id, 0)
        Session.add(ver)
      Session.commit()
      return '保存成功' + str(ver.test_tpl_id)
    return '参数有错误'

  def UpdateStypeVersionStatus(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])
    status = int(request.params['status'])
    if stype > 0 and version > 0:
      ver = self.data_version_.filter_by(stype = stype, version = version).first()
      if ver:
        ver.status = status
        Session.commit()
        return '保存成功' + str(ver.status)
      else:
        return '版本 ' + str(version) + '不存在'
    return '参数有错误'

  def GetStypeVersions(self):
    stype = -1
    if "stype" in request.params:
      stype = int(request.params['stype'])
    versions = self.data_version_.filter_by(stype = int(request.params['stype']))
    ret = '['
    count = 0
    for v in versions:
      if count > 0:
        ret += ','
      ret += '{"version":' + str(v.version)
      ret += ',"test_tpl_id":' + str(v.test_tpl_id)
      ret += ',"using_tpl_id":' + str(v.using_tpl_id)
      ret += ',"status":' + str(v.status) + '}'
      count += 1
    ret += ']'
    return ret

  def GetStypeVersionTpls(self):
    stype = -1
    version = -1
    if "stype" in request.params:
      stype = int(request.params['stype'])
      if "version" in request.params:
        version = int(request.params['version'])

    tpls = self.version_tpl_mapping_.filter_by(stype = stype, version = version)
    ret = '['
    count = 0
    for v in tpls:
      if count > 0:
        ret += ','
      ret += str(v.tpl_id)
      count += 1
    ret += ']'
    return ret

  def StypeConfigEdit(self):
    stype = -1
    from_version_id = 0
    apply_id = 0
    if "stype" in request.params:
      stype = int(request.params['stype'])
      if "apply_id" in request.params and "from_version_id" in request.params:
        apply_id = int(request.params['apply_id'])
        from_version_id = int(request.params['from_version_id'])

    return render('/feed-config-edit.mako', {"stype" : stype, "apply_id" : apply_id, "from_version_id" : from_version_id})

  def StypeConfigCreate(self):
    apply_id = 0
    if "apply_id" in request.params:
      apply_id = int(request.params['apply_id'])

    return render('/feed-config-create.mako', {"apply_id" : apply_id})

  def ApplyNewFeed(self):
    return render('/apply-new-feed.mako')

  def ApplyNewVersion(self):
    return render('/apply-new-version.mako')

  def ApplyNewTpl(self):
    return render('/apply-new-tpl.mako')

  def ApplyRollback(self):
    return render('/apply-rollback.mako')

  def ApplyPublish(self):
    return render('/apply-publish.mako')

  def FeedKeysEdit(self):
    stype = -1
    version = -1
    is_user = 0 
    apply_id = 0 

    from_tpl_id = 0
    if "stype" in request.params:
      stype = int(request.params['stype'])
      if "version" in request.params:
        version = int(request.params['version'])
        if "apply_id" in request.params and "from_tpl_id" in request.params:
          apply_id = int(request.params['apply_id'])
          from_tpl_id = int(request.params['from_tpl_id'])

    if "is_user" in request.params:
      is_user = int(request.params['version'])

    return render('/feed-keys-edit.mako', 
        {"stype" : stype, 
         "version" : version, 
         "from_tpl_id" : from_tpl_id, 
         "apply_id" : apply_id, 
         "is_user" : is_user})

  def FeedTplEdit(self):
    stype = -1
    version = -1
    tpl_id = -1
    if "stype" in request.params:
      stype = int(request.params['stype'])
    if "version" in request.params:
      version = int(request.params['version'])
    if "tpl_id" in request.params:
      tpl_id = int(request.params['tpl_id'])
    return render('/feed-tpl-edit.mako', {"stype" : stype, "version" : version, "tpl_id" : tpl_id})
    return "xxx"

  def GetStypeConfig(self):
    cfg = self.stype_config_.filter_by(stype=int(request.params['stype'])).first()
    if not cfg:
      cfg = StypeConfig(stype = int(request.params['stype']), 
          weight = 1, 
          type = int(request.params['stype']) / 100, 
          ptype = 0, 
          description = '', 
          push_feed_flags = 0, 
          news_merge_type = 0,
          mini_merge_type = 0,
          update_time_on_merge = 0,
          custom_expr = 0,
          persist_body = 0,
          persist_feeddb = 0,
          lifetime = 0,
          daily_quota = 0,
         
          news_merge_by = '', 
          mini_merge_by = '', 
          source_by = '', 
          psource_by = '', 
          actor_by = '', 
          dispatch_expr = '')

    return render('/feed-config.mako', {
        "stype" : cfg.stype, 
        "type" : cfg.type,
        "weight" : cfg.weight,
        "desc" : cfg.description,
        "ptype" : cfg.ptype,
        "save_content" : cfg.is_persist_content,
        "save_feed_db" : cfg.is_persist_feeddb,
        "news_merge_type" : cfg.news_merge_type,
        "mini_merge_type" : cfg.mini_merge_type,
        "push_flags" : cfg.push_flag,
        "custom_expr" : cfg.is_custom_expr,
        "update_time_on_merge" : cfg.is_update_time,
        "lifetime" : cfg.lifetime,
        "daily_quota" : cfg.daily_quota
       }
    )

  def SaveStypeConfig(self):
    cfg = self.stype_config_.filter_by(stype=int(request.params['stype'])).first()
    if not cfg:
      cfg = StypeConfig(stype = int(request.params['stype']), 
          weight = int(request.params['weight']), 
          type = int(request.params['type']), 
          ptype = int(request.params['ptype']), 
          description = request.params['title'], 
          push_feed_flags = int(request.params['push_flags']), 
          news_merge_type = int(request.params['news_merge_type']),
          mini_merge_type = int(request.params['mini_merge_type']),
          update_time_on_merge = int(request.params['update_time_on_merge']),
          custom_expr = int(request.params['custom_expr']),
          persist_body = int(request.params['save_content']),
          persist_feeddb = int(request.params['save_feed_db']),
          lifetime = int(request.params['lifetime']),
          daily_quota = int(request.params['daily_quota']))

      Session.add(cfg)
      if 'apply_id' in request.params:
        apply_id = int(request.params['apply_id'])
        self.FeedApplyHandled(apply_id)
    else:
      if int(request.params['is_new']) > 0:
        return '{"status":1, "desc":"新建子类型保存失败(已经存在)"}'

      cfg.weight = request.params['weight'] 
      cfg.type = int(request.params['type'])
      cfg.ptype = int(request.params['ptype'])
      cfg.description = request.params['title']
      cfg.push_feed_flags = int(request.params['push_flags'])
      cfg.news_merge_type = int(request.params['news_merge_type'])
      cfg.mini_merge_type = int(request.params['mini_merge_type'])
      cfg.update_time_on_merge = int(request.params['update_time_on_merge'])
      cfg.custom_expr = int(request.params['custom_expr'])
      cfg.persist_body = int(request.params['save_content'])
      cfg.persist_feeddb = int(request.params['save_feed_db'])
      cfg.lifetime = int(request.params['lifetime'])
      cfg.daily_quota = int(request.params['daily_quota'])

    Session.commit()
    return '{"status":0, "desc":"保存成功"}'

  def AddStypeVersion(self):
    stype = int(request.params['stype'])
    max_ver = Session.query(func.max(DataVersion.version)).filter_by(stype = stype).scalar() or 0
    vc = DataVersion(stype, max_ver + 1, 0, 0)
    Session.add(vc)
    Session.commit()
    return str(max_ver + 1)
    
  def AddVersionTpl(self):
    max_id = Session.query(func.max(VersionTplMapping.tpl_id)).scalar() or 1
    tpl_id = max_id + 1
    feed_key = VersionTplMapping(tpl_id = tpl_id,
        stype = request.params['stype'],
        version = request.params['version'])
    Session.add(feed_key)

    Session.commit()
    return str(tpl_id)

  def SaveVersionKeys(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])

    if stype <= 0 or version <= 0:
      return "参数错误"
    o = self.data_version_.filter_by(stype=stype, version=version).first()
    if o:
      o.keys_xml = request.params['kl']
      o.news_merge_by = request.params['news_merge_by']
      o.mini_merge_by = request.params['mini_merge_by']
      o.source_by = request.params['source_by']
      o.psource_by = request.params['psource_by']
      o.actor_by = request.params['actor_by']
      o.dispatch_expr = request.params['dispatch_expr']
      Session.commit()
      return '保存成功'
    else:
      return '该版本不存在'

  def GetFeedKeys(self):
    tpl_id = int(request.params['id'])
    if tpl_id > 0:
      o = self.data_version_.filter_by(tpl_id = tpl_id, stype=int(request.params['stype']), version=int(request.params['version'])).first()
      return o.keys_xml
    else:
      return ''

  def GetVersionKeys(self):
    stype = int(request.params['stype'])
    version = int(request.params['version'])

    if stype > 0 and version > 0:
      o = self.data_version_.filter_by(stype=stype, version=version).first()
      return o.keys_xml
    else:
      return ''

  def SaveTemplate(self):
    sk = TplView(request.params['tpl_id'], 
        view = request.params['view'],
        template = request.params['tpl'])
    Session.merge(sk)
    Session.commit()
    return '保存模板成功2'

  def RemoveTemplate(self):
    tpl_id = int(request.params['tpl_id'])
    view = int(request.params['view'])
    if tpl_id <= 0 or view < 0:
      return '参数错误'

    tpl = self.tpl_views_.filter_by(tpl_id = tpl_id, view = view).first()

    Session.delete(tpl)
    Session.commit()

    return '删除模板成功'

  def GetTemplates(self):
    tpl_id = int(request.params['tpl_id'])
    if tpl_id <= 0:
      return '参数错误'

    tpls = self.tpl_views_.filter_by(tpl_id = tpl_id)

    count = 0
    rsp = '{'
    for o in tpls:
      print o.view, o.template
      if count > 0:
        rsp += ','
      rsp += '"' + str(o.view) + '":"' + o.template.replace('"', '\\"') + '"'
      count += 1
    rsp += '}'
    return rsp

  def ValidateTemplate(self):
    tpl = request.params['tpl']
    hdf = neo_util.HDF()  # create an HDF dataset
    cs = neo_cs.CS(hdf)
    try:
      cs.parseStr(urllib2.unquote(str(tpl)))
      cs.render()
    except neo_util.ParseError, e:
      return "错误:",str(e)
    return '语法检查通过'

  def ApplyNewFeedSubmit(self):
    max_id = Session.query(func.max(ApplyNewFeed.apply_id)).scalar() or 0

    pm_emails = request.params['pm_emails']
    pm_names = request.params['pm_names']
    stype_desc = request.params['stype_desc']
    a = ApplyNewFeed(max_id + 1,
        pm_names = pm_names, 
        pm_emails = pm_emails, 
        dev_names = request.params['dev_names'], 
        dev_emails = request.params['dev_emails'], 
        stype_desc = stype_desc, 
        stype_text = request.params['stype_text'], 
        push_news = int(request.params['push_news']), 
        push_mini = int(request.params['push_mini']), 
        news_merge_desc = request.params['news_merge_desc'], 
        mini_merge_desc = request.params['mini_merge_desc'], 
        lifetime = int(request.params['lifetime']), 
        status = 0,
        apply_time = int(time.time())
        )
    Session.add(a)
    Session.commit()

    self.SendMail('yuwei.mu@renren-inc.com', pm_emails, '申请创建新鲜事类型', pm_names + ':' + stype_desc)
    return '申请保存成功'

  def SendMail(self, sender, receivers, title, body):
    try:
      message = 'From: ' + sender + '\r\n'
      message += 'To: ' + receivers + '\r\n'
      message += 'MIME-Version: 1.0\r\n'
      message += 'Content-type: text/html; charset=UTF-8\r\n'
      message += 'Subject: ' + title + '\r\n'
      message += '\r\n'
      message += body

      smtpObj = smtplib.SMTP('smtp.renren-inc.com')
      smtpObj.sendmail(sender, receivers, message)         
      print "Successfully sent email"
    except SMTPException:
      print "Error: unable to send email"

  def ApplyNewFeedList(self):
    return render('/apply-new-feed-list.mako')

  def GetApplyFeedItem(self):
    apply_id = request.params['apply_id']
    a = self.apply_feeds_.filter_by(apply_id = apply_id).first()

    if a:
      ret = '{"apply_id":' + str(a.apply_id)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"pm_emails":"' + a.pm_emails + '"'
      ret += ',"dev_names":"' + a.dev_names.replace('"', '\\"') + '"'
      ret += ',"dev_emails":"' + a.dev_emails + '"'
      ret += ',"stype_desc":"' + a.stype_desc.replace('"', '\\"') + '"'
      ret += ',"stype_text":"' + a.stype_text.replace('"', '\\"') + '"'
      ret += ',"push_news":' + str(a.push_news)
      ret += ',"push_mini":' + str(a.push_mini)
      ret += ',"news_merge_desc":"' + a.news_merge_desc.replace('"', '\\"') + '"'
      ret += ',"mini_merge_desc":"' + a.mini_merge_desc.replace('"', '\\"') + '"'
      ret += ',"lifetime":' + str(a.lifetime)
      ret += ',"status":' + str(a.status)
      ret += '}'
      return ret
    return '{}'

  def FeedApplyHandled(self, apply_id):
    a = self.apply_feeds_.filter_by(apply_id = apply_id).first()
    if a:
      a.status = 1
      Session.commit()

  def GetApplyFeedList(self):
    applies = self.apply_feeds_.all()
    count = 0
    ret = '['
    for a in applies:
      if count > 0:
        ret += ','
      ret += '{"apply_id":' + str(a.apply_id)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"stype_desc":"' + a.stype_desc.replace('"', '\\"') + '"'
      ret += ',"status":' + str(a.status)
      ret += '}'
      count += 1
    ret += ']'
    return ret

  def ApplyNewVersionSubmit(self):
    max_id = Session.query(func.max(ApplyNewVersion.apply_id)).scalar() or 0

    pm_emails = request.params['pm_emails']
    pm_names = request.params['pm_names']
    version_desc = request.params['version_desc']
    a = ApplyNewVersion(max_id + 1,
        stype_id = int(request.params['stype_id']),
        from_version_id = int(request.params['from_version_id']),
        pm_names = pm_names, 
        pm_emails = pm_emails, 
        dev_names = request.params['dev_names'], 
        dev_emails = request.params['dev_emails'], 
        version_desc = version_desc, 
        version_text = request.params['version_text'], 
        status = 0,
        apply_time = int(time.time())
        )
    Session.add(a)
    Session.commit()

    # self.SendMail('yuwei.mu@renren-inc.com', pm_emails, '申请创建新鲜事类型', pm_names + ':' + stype_desc)
    return '申请保存成功'

  def ApplyNewVersionList(self):
    return render('/apply-new-version-list.mako')

  def GetApplyVersionList(self):
    applies = self.apply_versions_.all()
    count = 0
    ret = '['
    for a in applies:
      if count > 0:
        ret += ','
      ret += '{"apply_id":' + str(a.apply_id)
      ret += ',"stype_id":' + str(a.stype_id)
      ret += ',"from_version_id":' + str(a.from_version_id)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"version_desc":"' + a.version_desc.replace('"', '\\"') + '"'
      ret += ',"status":' + str(a.status)
      ret += '}'
      count += 1
    ret += ']'
    return ret

  def VersionApplyHandled(self):
    a = self.apply_versions_.filter_by(apply_id = int(request.params['apply_id'])).first()
    if a:
      a.status = 1
      Session.commit()

    return ''

  def ApplyNewTplSubmit(self):
    max_id = Session.query(func.max(ApplyNewTpl.apply_id)).scalar() or 0

    pm_emails = request.params['pm_emails']
    pm_names = request.params['pm_names']
    tpl_desc = request.params['tpl_desc']
    a = ApplyNewTpl(max_id + 1,
        stype_id = int(request.params['stype_id']),
        version = int(request.params['version']),
        from_tpl_id = int(request.params['from_tpl_id']),
        pm_names = pm_names, 
        pm_emails = pm_emails, 
        dev_names = request.params['dev_names'],
        dev_emails = request.params['dev_emails'],
        tpl_desc = tpl_desc,
        tpl_text = request.params['tpl_text'],
        status = 0,
        apply_time = int(time.time())
        )
    Session.add(a)
    Session.commit()

    # self.SendMail('yuwei.mu@renren-inc.com', pm_emails, '申请创建新鲜事类型', pm_names + ':' + stype_desc)
    return '申请保存成功'

  def ApplyNewTplList(self):
    return render('/apply-new-tpl-list.mako')
  
  def GetApplyTplList(self):
    applies = self.apply_tpls_.all()
    count = 0
    ret = '['
    for a in applies:
      if count > 0:
        ret += ','
      ret += '{"apply_id":' + str(a.apply_id)
      ret += ',"stype_id":' + str(a.stype_id)
      ret += ',"version":' + str(a.version)
      ret += ',"from_tpl_id":' + str(a.from_tpl_id)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"tpl_desc":"' + a.tpl_desc.replace('"', '\\"') + '"'
      ret += ',"status":' + str(a.status)
      ret += '}'
      count += 1
    ret += ']'
    return ret

  def TplApplyHandled(self):
    a = self.apply_tpls_.filter_by(apply_id = int(request.params['apply_id'])).first()
    if a:
      a.status = 1
      Session.commit()

    return ''

  def ApplyRollbackList(self):
    return render('/apply-rollback-list.mako')

  def GetApplyRollbackList(self):
    applies = self.apply_rollbacks_.all()
    count = 0
    ret = '['
    for a in applies:
      if count > 0:
        ret += ','
      ret += '{"apply_id":' + str(a.apply_id)
      ret += ',"stype_id":' + str(a.stype_id)
      ret += ',"version":' + str(a.version)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"rollback_desc":"' + a.rollback_desc.replace('"', '\\"') + '"'
      ret += ',"status":' + str(a.status)
      ret += '}'
      count += 1
    ret += ']'
    return ret

  def ApplyRollbackSubmit(self):
    max_id = Session.query(func.max(ApplyRollback.apply_id)).scalar() or 0

    pm_emails = request.params['pm_emails']
    pm_names = request.params['pm_names']
    rollback_desc = request.params['rollback_desc']
    a = ApplyRollback(max_id + 1,
        stype_id = int(request.params['stype_id']),
        version = int(request.params['version']),
        tpl_id = int(request.params['tpl_id']),
        pm_names = pm_names, 
        pm_emails = pm_emails, 
        dev_names = request.params['dev_names'], 
        dev_emails = request.params['dev_emails'], 
        rollback_desc = rollback_desc, 
        status = 0,
        apply_time = int(time.time())
        )
    Session.add(a)
    Session.commit()

    # self.SendMail('yuwei.mu@renren-inc.com', pm_emails, '申请创建新鲜事类型', pm_names + ':' + stype_desc)
    return '申请保存成功'


  def ApplyPublishList(self):
    return render('/apply-publish-list.mako')

  def GetApplyPublishList(self):
    applies = self.apply_publishs_.all()
    count = 0
    ret = '['
    for a in applies:
      if count > 0:
        ret += ','
      ret += '{"apply_id":' + str(a.apply_id)
      ret += ',"stype_id":' + str(a.stype_id)
      ret += ',"version":' + str(a.version)
      ret += ',"pm_names":"' + a.pm_names.replace('"', '\\"') + '"'
      ret += ',"publish_desc":"' + a.publish_desc.replace('"', '\\"') + '"'
      ret += ',"status":' + str(a.status)
      ret += '}'
      count += 1
    ret += ']'
    return ret

  def ApplyPublishSubmit(self):
    max_id = Session.query(func.max(ApplyPublish.apply_id)).scalar() or 0

    pm_emails = request.params['pm_emails']
    pm_names = request.params['pm_names']
    publish_desc = request.params['publish_desc']
    a = ApplyPublish(max_id + 1,
        stype_id = int(request.params['stype_id']),
        version = int(request.params['version']),
        tpl_id = int(request.params['tpl_id']),
        pm_names = pm_names, 
        pm_emails = pm_emails, 
        dev_names = request.params['dev_names'], 
        dev_emails = request.params['dev_emails'], 
        publish_desc = publish_desc, 
        status = 0,
        apply_time = int(time.time())
        )
    Session.add(a)
    Session.commit()

    # self.SendMail('yuwei.mu@renren-inc.com', pm_emails, '申请创建新鲜事类型', pm_names + ':' + stype_desc)
    return '申请保存成功'

  def GenreateCode(self):
    items = urllib2.unquote(request.params['hdf']).split(';')
    # .split(';')
    hdf = neo_util.HDF() 
    print items
    for item in items:
      print item
      v = item.split('=')
      if (len(v) == 2):
        hdf.setValue(v[0], v[1])

    cs = neo_cs.CS(hdf)
    cs.parseFile("./feedtpladmin/public/feed-java-code.cs") # parse a file from disk
    gen_code = cs.render()


    url_path = '/FeedBuilder_' + request.params['stype'] + '_' + request.params['version'] + '.java'

    f = open("./feedtpladmin/public" + url_path, "w")

    f.write(gen_code)
    f.close()

    return url_path

