#!/usr/bin/python
#coding:utf-8
import time
import tornado.database
import tornado.options

from tornado.options import define, options

define("mysql_host", default="10.2.18.202:3306", help="dbdesc database host")
define("mysql_database", default="beijing", help="dbdesc database name")
define("mysql_user", default="test", help="dbdesc database user")
define("mysql_password", default="test", help="dbdesc database password")

'''
desc instance;
+-----------+--------------+------+-----+---------+-------+
| Field     | Type         | Null | Key | Default | Extra |
+-----------+--------------+------+-----+---------+-------+
| name      | varchar(255) | NO   | PRI | NULL    |       | 
| type      | varchar(20)  | NO   |     | NULL    |       | 
| timestamp | bigint(21)   | NO   |     | NULL    |       | 
+-----------+--------------+------+-----+---------+-------+
desc server;
+---------------+--------------+------+-----+---------+----------------+
| Field         | Type         | Null | Key | Default | Extra          |
+---------------+--------------+------+-----+---------+----------------+
| id            | bigint(20)   | NO   | PRI | NULL    | auto_increment | 
| type          | varchar(20)  | NO   |     | NULL    |                | 
| database      | varchar(40)  | NO   |     | NULL    |                | 
| host          | varchar(20)  | NO   |     | NULL    |                | 
| port          | int(11)      | NO   |     | NULL    |                | 
| user          | varchar(20)  | NO   |     | NULL    |                | 
| password      | varchar(20)  | NO   |     | NULL    |                | 
| wrflag        | varchar(20)  | NO   |     | NULL    |                | 
| charset       | varchar(20)  | YES  |     | NULL    |                | 
| priority      | varchar(20)  | YES  |     | NULL    |                | 
| instance_name | varchar(255) | NO   | MUL | NULL    |                | 
+---------------+--------------+------+-----+---------+----------------+
desc route;
+---------------+--------------+------+-----+---------+----------------+
| Field         | Type         | Null | Key | Default | Extra          |
+---------------+--------------+------+-----+---------+----------------+
| id            | bigint(20)   | NO   | PRI | NULL    | auto_increment | 
| instance_name | varchar(255) | NO   | MUL | NULL    |                | 
| expression    | varchar(255) | NO   |     | NULL    |                | 
| instance      | varchar(255) | NO   |     | NULL    |                | 
+---------------+--------------+------+-----+---------+----------------+
'''
class Instance:
  def __init__(self):
    self.name = None
    self.type = None
    self.timestamp = None

  def make(self, name, type):
    self.type = type
    self.name = name
    self.timestamp = time.time()

class Server:
  _last_id = 0
  
  def last(self):
    self._last_id = self._last_id+1
    return self._last_id

  def make(self, type, database, host, port, user, password, wrflag, charset, priority, instance_name):
    self.id = self.last()
    self.type = type        
    self.database = database    
    self.host = host        
    self.port = port        
    self.user = user        
    self.password = password    
    self.wrflag = wrflag      
    self.charset = charset     
    self.priority = priority    
    self.instance_name = instance_name

  def __init__(self):
    self.id = None          
    self.type = None        
    self.database = None    
    self.host = None        
    self.port = None        
    self.user = None        
    self.password = None    
    self.wrflag = None      
    self.charset = None     
    self.priority = None    
    self.instance_name = None

class Route:
  _last_id = 0
  
  def last(self):
    _last_id = _last_id+1
    return _last_id

  def make(self, instance_name, expression, instance):
    self.id = self.last()
    self.instance_name = instance_name
    self.expression= expression
    self.instance = instance

  def __init__(self):
    self.id = None
    self.instance_name=None
    self.expression=None
    self.instance=None

class DbOption():
  def __init__(self):
    self.dirty = False
    i = Instance()
    i.make('common', 'singler')
    self.instances=[i]
    self.servers = []
    self.routes = []
    
  def get_instance(self, instance_name):
    for instance in self.instances:
      if instance.name == instance_name:
        return instance
    return None

  def get_instances(self):
    return self.instances

  def insert_instance(self, instance_name, type):
    self.dirty = True
    i = Instance()
    i.make(instance_name, type)
    self.instances.append(i)

  def delete_instance(self, instance_name):
    self.dirty = True
    instance = self.get_instance(instance_name)
    if instance.type == "singler":
      for s in self.servers:
        if s.instance_name == instance_name:
          self.servers.remove(s)
    else:
      for r in self.routes:
        if r.instance_name == instance_name:
          self.routes.remove(r)
    for i in self.instances:
      if i.name == instance_name:
        self.instances.remove(i)
        return

  def update_instance(self, instance_name, newname = None):
    self.dirty = True
    if newname:
      instance = self.get_instance(instance_name)
      if instance.type == "singler":
        for s in self.servers:
          if s.instance_name == instance_name:
            s.instance_name = newname
      else:
        for r in self.routes:
          if r.instance_name == instance_name:
            r.instance_name = newname

      for i in self.instances:
        if i.name == instance_name:
          i.name = newname
          i.timestamp = time.time()
          return
    else:
      for i in self.instances:
        if i.name == instance_name:
          i.timestamp = time.time()
          return

  def get_servers(self, instance_name):
    servers = []
    for s in self.servers:
      if s.instance_name == instance_name:
        servers.append(s)
    return servers

  def get_routes(self, instance_name):
    routes = []
    for r in self.routes:
      if r.instance_name == instance_name:
        routes.append(r)
    return routes

  def delete_server(self,id):
     for s in self.servers:
       if s.id == id:
         self.servers.remove(s)
         self.update_instance(s.instance_name)
         break

  def delete_route(self,id):
     for r in self.routes:
       if r.id == id:
         self.routes.remove(r)
         self.update_instance(r.instance_name)
         break

  def update_route(self, id, expression, instance):
      for r in self.routes:
        if r.id == id:
          r.expression = expression
          r.instance = instance

          self.update_instance(r.instance_name)
          break
    
  def update_server(self, id, type, database, host, port, user,
              password, wrflag, charset, priority):
      for s in self.servers:
        if s.id == id:
          s.type = type        
          s.database = database    
          s.host = host        
          s.port = port        
          s.user = user        
          s.password = password    
          s.wrflag = wrflag      
          s.charset = charset     
          s.priority = priority    

          self.update_instance(s.instance_name)
          break

  def insert_route(self, instance_name, expression, instance):
    r = Route()
    r.make(instance_name, expression, instance)
    self.routes.append(r)
    self.update_instance(instance_name)
  
  def insert_server(self, instance_name, type, database, host, port, user,
              password, wrflag, charset, priority):
    s = Server()
    s.make(type, database, host, port, user, password, wrflag, charset, priority, instance_name)
    self.servers.append(s)
    self.update_instance(instance_name)


