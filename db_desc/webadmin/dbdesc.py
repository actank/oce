#!/usr/bin/python
#coding:utf-8

import os.path
import re
import json
from dbop import DbOption
from whocall import who_called_me
import tornado.httpserver
import tornado.ioloop
import tornado.options
import tornado.web
import unicodedata
import Ice, IceGrid, traceback, sys
from threading import Timer

from tornado.options import define, options
from icedesc import IceDesc
from com.xiaonei import xce

define("port", default=8888, help="run on the given port", type=int)
class row:
  pass

class Application(tornado.web.Application):

  def __init__(self):
    handlers = [
      (r"/", HomeHandler),
      (r"/edit_instance", EditInstanceHandler),
      (r"/login", AuthLoginHandler),
      (r"/logout", AuthLogoutHandler),
      (r"/edit_route", EditRouteHandler),
      (r"/edit_server", EditServerHandler),
      (r"/pubdesc", PubDescHandler),
      (r"/subdesc", SubDescHandler),
      (r"/reload", ReloadHandler),
      (r"/about", AboutHandler),
    ]
    settings = dict(
      blog_title=u"qirong webadmin",
      template_path=os.path.join(os.path.dirname(__file__), "templates"),
      static_path=os.path.join(os.path.dirname(__file__), "static"),
#  ui_modules={"Entry": EntryModule},
      xsrf_cookies=True,
      cookie_secret="11oETzKXQAIaYdkL5iEmIeJJFuYh7EQnp2XdTP1o/Vo=",
      login_url="/login",
    )
    tornado.web.Application.__init__(self, handlers, **settings)
    self.current_instance = None
    self.db = DbOption()  
    self.bk_servers = None
    self.bk_routes = None
    self.bk_instances = None
    self.ice = IceDesc()

    self.ice.subscribe()
    self._last_id = 0
    
class BaseHandler(tornado.web.RequestHandler):
  
  def last(self):
    self.application._last_id = self.application._last_id+1
    return self.application._last_id

  @property
  def db(self):
    return self.application.db

  @property
  def ice(self):
    return self.application.ice

  def get_current_user(self):
    user_name = self.get_secure_cookie("user")
    if not user_name:
      return None
    if user_name == "qirong":
      return user_name
    else:
      return None

  def set_current_user(self, user_name):
    self.set_secure_cookie("user", user_name)

  def subscribe(self, *arr):
    self.application.ice.subscribe(*arr)

  def reload(self):
    self.application.ice.reload()

  @property
  def bk_instances(self):
    return self.application.bk_instances
  @bk_instances.setter
  def bk_instances(self, value):
    self.application.bk_instances = value

  @property
  def bk_servers(self):
    return self.application.bk_servers
  @bk_servers.setter
  def bk_servers(self, value):
    self.application.bk_servers = value
   
  @property
  def bk_routes(self):
    return self.application.bk_routes
  @bk_routes.setter
  def bk_routes(self, value):
    self.application.bk_routes = value

  @property
  def bk_all_routes(self):
    return self.application.bk_all_routes
  @bk_all_routes.setter
  def bk_all_routes(self, value):
    self.application.bk_all_routes = value

  @property
  def current_instance(self):
    return self.application.current_instance
  @current_instance.setter
  def current_instance(self, value):
    self.application.current_instance = value


class HomeHandler(BaseHandler):
  @tornado.web.authenticated
  def get(self):
    delinstance = self.get_argument("del_instance", None)
    if delinstance:
      print "del instance", delinstance
      self.db.delete_instance(delinstance)
      self.current_instance = None
      self.redirect("/")
      return

    delserver = self.get_argument("del_server", None)
    if delserver:
      print "del server"
      self.db.delete_server(delserver)
      self.redirect("/")
      return

    delroute = self.get_argument("del_route", None)
    if delroute:
      print "del route"
      self.db.delete_route(delroute)
      self.redirect("/")
      return
 
    self.bk_instances = self.db.get_instances(); 
    if not self.bk_instances:
      self.write("no database connect");
      return
    self.bk_all_routes = self.db.get_all_routes();
    self.bk_instances = self.sort_instances(self.bk_instances, self.bk_all_routes)
#    return

    name = self.get_argument("instance",None)
    if name:
      self.current_instance = self.db.get_instance(name)

    context = None
    type = None
    if self.current_instance:
      type = self.current_instance.type
      if type == "singler":
        context = self.db.get_servers(self.current_instance.name)
        self.bk_servers = context
      else:
        context = self.get_routes(self.bk_all_routes, self.current_instance.name)
        self.bk_routes = context

    self.render("home.html", db_dirty=self.db.dirty, instances=self.bk_instances,
                current=self.current_instance, type=type, context=context)

  def get_instance(self, instances, name):
    for i in instances:
      if i.name == name:
        return i

  def get_routes(self, routes, instance_name):
    v = []
    for r in routes:
      if r.instance_name == instance_name:
        v.append(r)
    return v

  def get_sub_instances(self, instances):
    for i in instances:
      self.ice.query(i.name)

  def sort_instances(self, instances, routes):
    res = [];#instances[:];
    for i in instances:
      if i.type == 'router':
        i.ext_count = 0
        res.append(i)
        rs = self.get_routes(routes, i.name)
        i.ext_max = len(rs)
        for r in rs:
          ib = self.get_instance(instances, r.instance)
          if ib:
            r.ext_have = True
            i.ext_count += 1
            ib.ext_have = True
#print ib.__dict__
            res.append(ib)
        
    for i in instances:
      if i.type == 'singler' and not hasattr(i, 'ext_have'):
        res.append(i)
    return res

class EditInstanceHandler(BaseHandler):
  @tornado.web.authenticated
  def get(self):
    insert = self.get_argument("insert", None)
    if insert:
      self.render("edit_instance.html", insert=insert,
                   error_info= None,
                   name= "",
                   type = "")
    else:
      update=self.get_argument("instance", None)
      self.db.update_instance(update)
      self.redirect("/")

  @tornado.web.authenticated
  def post(self):
    
    insert = self.get_argument("insert", None)
    if insert:
      newname = self.get_argument("name", None)
      type = self.get_argument("type")
      if newname: 
        for n in self.bk_instances:
          if n.name == newname:
            self.render("edit_instance.html", insert=insert,
                   error_info="mingzi chongfu",
                   name=self.get_argument("name", ""),
                   type = self.get_argument("type", "")
                   )
            return
        self.db.insert_instance(newname, type)
      else:
         self.render("edit_instance.html", insert=insert,
                   error_info="mingzi bunongkong",
                   name=self.get_argument("name", ""),
                   type = self.get_argument("type", "")
                   )

    else:
        self.db.update_instance(orgname)
    self.redirect("/?instance="+ newname)

class EditRouteHandler(BaseHandler):
  @tornado.web.authenticated
  def get(self):
    insert=self.get_argument("insert", None)
    instance_name = self.get_argument("instance_name", None)
    id = self.get_argument("id", None)
    route = None
    if not insert:
      for r in self.bk_routes:
        if r.id == int(id):
          route = r
          break
      else:
        self.redirect("/") # 异常

    self.render("edit_route.html", insert=insert,
          error_info= "",
          instance_name= instance_name,
          route=route)
  
  @tornado.web.authenticated
  def post(self):
    insert = self.get_argument("insert", None)
    r = row()
    r.expression = self.get_argument("expression", "")
    r.instance = self.get_argument("instance", "")
    instance_name = self.get_argument("instance_name", "")
    r.id = self.get_argument("id", None)

    if not r.expression or not r.instance:
      self.render("edit_route.html", insert=insert,
             error_info= "mouxie zidian wei kong",
             instance_name= instance_name,
             route=r)
      return

    for n in self.bk_all_routes:
      if n.expression == r.expression and n.instance == r.instance and n.id != r.id:
        self.render("edit_route.html", insert=insert,
             error_info= "yu xianyou de guize chongfu",
             instance_name= instance_name,
             route=r)
        return

    if insert:
      self.db.insert_route(instance_name, r.expression, r.instance)
    else:
      self.db.update_route(int(r.id), r.expression, r.instance)
    self.redirect("/")

class ReloadHandler(BaseHandler):
  
  @tornado.web.authenticated
  def get(self):
    self.render("reload.html")
     
  @tornado.web.authenticated
  def post(self):
    xx = self.db.check_valid()
    if xx[0] == False:
      print "error:", xx[1]
      self.render("info.html",title= "can't reload", error= xx[1], db_dirty=self.db.dirty) 
      return

    self.reload()
    self.redirect("/")

class IceDescJson(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, xce.DbInstance):
      i = {}
      i['name'] = o.name
      i['timestamp'] = o.timestamp
      i['type'] = o.type
      i['wserver'] = o.wserver
      i['rservers'] = o.rservers
      i['routes'] = o.routes
      return i

    if isinstance(o, xce.DbServer):
      s = {}
      s['type'] = o.type
      s['database'] = o.database
      s['host'] = o.host
      s['port'] = o.port
      s['charset'] = o.charset
      s['user'] = o.user
      s['password'] = o.password
      return s
      
    if isinstance(o, xce.Route):
      r = {}
      r['expression'] = o.expression
      r['instance'] = o.expression
      return r
    return JSONEncoder.default(self, o)
    
class PubDescHandler(BaseHandler):
  @tornado.web.authenticated
  def get(self):
#x = json.dumps(self.bk_instances, cls = IceDescJson)
#    print x
    self.render("pubdesc.html", db_dirty=self.db.dirty)


class SubDescHandler(BaseHandler):
  _pre = "<script language=JavaScript >parent.push(";
  _post = ")</script> ";

  def push(self, msg):
    self.write(self._pre + str(msg) + self._post)
    print "push:", msg
    self.flush()

  @tornado.web.authenticated
  @tornado.web.asynchronous
  def get(self):
    print "client link--"
    self.callback = self.async_callback(self.on_new_notify)
    self.push("'<h2>wait for reload</h2>'")
    self.ice.add_callback(self.callback)
    self.request.connection.stream.set_close_callback(self.onclose)
#self.finish();

  def onclose(self):
    print "client losting"
    self.ice.del_callback(self.callback)

  def on_new_notify(self, messages):
    if self.request.connection.stream.closed():
      print "client lost---"
      self.ice.del_callback(self.callback)
      return
    x = json.dumps(messages, cls = IceDescJson)
    print "on_new_notify:", x
    self.push(x)
#   self.push(str(messages))

class EditServerHandler(BaseHandler):
  @who_called_me()
  @tornado.web.authenticated
  def get(self):
    insert=self.get_argument("insert", None)
    instance_name = self.get_argument("instance_name", None)
    id = self.get_argument("id", None)
    server = None
    if not insert:
      for s in self.bk_servers:
        if s.id == int(id):
          server = s
          break
      else:
        self.redirect("/") # 异常

    self.render("edit_server.html", insert=insert,
          error_info= "",
          instance_name= instance_name,
          server=server)

  @tornado.web.authenticated
  def post(self):
    s = row()
    insert = self.get_argument("insert", None)
    s.type = self.get_argument("type", "")
    s.database = self.get_argument("database", "")
    s.host = self.get_argument("host", "")
    s.port = self.get_argument("port", "")
    s.user = self.get_argument("user", "")
    s.password = self.get_argument("password", "")
    s.wrflag = self.get_argument("wrflag", None)
    s.charset = self.get_argument("charset", "")
    s.priority = self.get_argument("priority", "")
    umber = self.get_argument("umber", 1)
    s.instance_name = self.get_argument("instance_name", "")
    s.id = self.get_argument("id", None)

    if s.type == "" or s.database == "" or s.host == "" or s.user =="" or s.password == "" or s.charset== "" or umber < 1 or umber > 100:
      self.render("edit_server.html", insert=insert,
          error_info= "gebie zidian cuowu,buneng weikong",
          instance_name= s.instance_name,
          server=s)
      return

    if insert:
      for x in range(umber):
        self.db.insert_server(s.instance_name, s.type, s.database, s.host, s.port,
            s.user, s.password, s.wrflag, s.charset, s.priority)
    else:
      self.db.update_server(int(s.id), s.type, s.database, s.host, s.port,
            s.user, s.password, s.wrflag, s.charset, s.priority)

    self.redirect("/")

class AboutHandler(BaseHandler):  
  def get(self):
    self.render("about.html", last= self.last())

class AuthLoginHandler(BaseHandler):
  def get(self):    
    #self.get_secure_cookie("user", str(author_id))
    self.render("login.html", user=self.current_user)

  def post(self):
    username = self.get_argument("username")
    password = self.get_argument("password")
    if username == "qirong" and password == "dd":
      self.set_current_user(username)
    self.redirect("/")

class AuthLogoutHandler(BaseHandler):
  def get(self):
    self.clear_cookie("user")
    self.redirect(self.get_argument("next", "/"))


class EntryModule(tornado.web.UIModule):
  def render(self, entry):
    return self.render_string("modules/entry.html", entry=entry)


def main():
  tornado.options.parse_command_line()
  http_server = tornado.httpserver.HTTPServer(Application())
  http_server.listen(options.port)
  tornado.ioloop.IOLoop.instance().start()


if __name__ == "__main__":
  main()
