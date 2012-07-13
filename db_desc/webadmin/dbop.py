#!/usr/bin/python
#coding:utf-8
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
class DbOption():
  def __init__(self):
    self.db = tornado.database.Connection(
            host=options.mysql_host, database=options.mysql_database,
            user=options.mysql_user, password=options.mysql_password)
    self.dirty = False
    pass

  def check_valid(self):
    instances = self.get_instances()
    for i in instances:
      if i.type == 'singler':
        wcount = 0
        servers = self.get_servers(i.name)
        for w in servers:
          flag = w.wrflag.lower()
          if flag == 'wr' or flag == 'w':
            wcount += 1
        if wcount != 1: 
          return [False, i.name + " not or many writed db"]

      elif i.type == 'router':
          routes = self.get_routes(i.name)
          for r in routes:
            rs = self.get_instance(r.instance)
            if not rs or rs.type != 'singler':
              return [False, i.name + " expresion error"]
      else:
           return [False, i.name + " error instance type"]
    return [True, ""]


  def get_instance(self, instance_name):
    return self.db.get("select * from instance where name=%s", instance_name)

  def get_instances(self):
    return self.db.query("select * from instance")

  def insert_instance(self, instance_name, type):
    self.dirty = True
    return self.db.execute("insert instance (name, type, timestamp) values"
                            "(%s, %s, UNIX_TIMESTAMP())",
                            instance_name, type);

  def delete_instance(self, instance_name):
    self.dirty = True
    instance = self.get_instance(instance_name)
    if instance.type == "singler":
     self.db.execute("delete from server where instance_name=%s", instance_name)
    else:
      self.db.execute("delete from route where instance_name=%s", instance_name)
    self.db.execute("delete from instance where name=%s", instance_name)

  def update_instance(self, instance_name, newname = None):
    self.dirty = True
    if newname:
      instance = self.get_instance(instance_name)
      if instance.type == "singler":
        self.db.execute("update server set instance_name=%s where instance_name=%s", newname, instance_name)
      else:
        self.db.execute("update route set instance_name=%s where instance_name=%s", newname, instance_name)

      return self.db.execute("update instance set name=%s, timestamp=UNIX_TIMESTAMP() where name=%s"
                            ,newname ,instance_name)
    else:
      return self.db.execute("update instance set timestamp=UNIX_TIMESTAMP() where name=%s"
                            ,instance_name)

  def get_servers(self, instance_name):
    return self.db.query("select * from server where instance_name=%s", instance_name)

  def get_routes(self, instance_name):
    return self.db.query("select * from route where instance_name=%s", instance_name)

  def get_all_routes(self):
    return self.db.query("select * from route")

  def delete_server(self,id):
     instance_name = self.db.get("select instance_name from server where id=%s",id)
     self.db.execute("delete from server where id=%s", id)
     self.update_instance(instance_name.instance_name)

  def delete_route(self,id):
     instance_name = self.db.get("select instance_name from route where id=%s",id)
     self.db.execute("delete from route where id=%s", id)
     self.update_instance(instance_name.instance_name)

  def update_route(self, id, expression, instance):
    self.db.execute("update route set expression=%s, instance=%s where id=%s",
        expression, instance, id) 
    instance_name = self.db.get("select instance_name from route where id=%s",id)
    self.update_instance(instance_name.instance_name)

  def update_server(self, id, type, database, host, port, user,
              password, wrflag, charset, priority):
    self.db.execute("update server set type=%s, `database`=%s,"
              "host=%s, port=%s, user=%s, password=%s,"
            "wrflag=%s, charset=%s, priority=%s where id=%s",
                      type, database, host, port, user,         
                password, wrflag, charset, priority, id)
    instance_name = self.db.get("select instance_name from server where id=%s",id)
    self.update_instance(instance_name.instance_name)

  def cover_routes(self, instance_name, routes):
  # 应该放到事务里
    self.db.execute("delete from route")
    for route in routes:
      self.insert_route(instance_name, route['excpression'], route['instance'])
    self.update_instance(instance_name)


  def cover_servers(self, instance_name, servers):
    self.db.execute("delete from server")
    for server in servers:
      self.db.execute("insert server (instance_name, type, database," 
                      "host, port, user, psssword,"
                     "wrflag, charset, priority)" 
                      "values(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)",
                      server[instance_name], server[type], server[database],
                      server[host], server[port], server[user], server[password],
                      server[wrflag], server[charset], server[priority])
    self.update_instance(instance_name)

  def insert_route(self, instance_name, expression, instance):
    self.db.execute("insert route (instance_name, expression, instance) value (%s, %s, %s)",
                      instance_name, expression, instance ) 
    self.update_instance(instance_name)
  
  def insert_server(self, instance_name, type, database, host, port, user,
              password, wrflag, charset, priority):
    self.db.execute("insert server (instance_name, type, `database`,"
              "host, port, user, password,"
              "wrflag, charset, priority) value"
              "(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)",
                      instance_name, type, database, host, port, user,         
                password, wrflag, charset, priority)
    self.update_instance(instance_name)


