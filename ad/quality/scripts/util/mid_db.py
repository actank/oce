import os
import sys
sys.path.append('/data/xce/zhaohui.tang/python/util/python_mysql')
import MySQLdb

class MidDb(object):	
		
	def initDbConnection(self, HOST, USER, PASSWD, DB):
		#self.con = MySQLdb.connect(host = 'im3.db.d.xiaonei.com', user = 'xndev', passwd = 'rebornlOM', db = 'stat')
		self.con = MySQLdb.connect(host = HOST, user = USER, passwd = PASSWD, db = DB)
		self.cursor = self.con.cursor()
		
	def closeDbConnection(self):
		self.cursor.close()
		self.con.close()
	
	def doInsert(self, sql):
		 self.cursor.execute(sql)
		 self.con.commit()
	
	def doSelect(self, sql):
		self.cursor.execute(sql)
		return self.cursor.fetchall()

#middb = MidDb()
#middb.initDbConnection('im3.db.d.xiaonei.com', 'xndev', 'rebornlOM', 'stat')
#print middb.doSelect('show tables;')
#middb.closeDbConnection()
	
	

