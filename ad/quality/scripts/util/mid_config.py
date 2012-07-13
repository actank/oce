class MidConfig(object):
	def getMapperPath(self):
		return '/home/liuguoqing/Desktop/VPN/hadoop-0.19.2/XNTALK/mapper/'
		
	def getDirConfig(self):
		import os
		#self.nowpath = str(os.getcwd())
		self.dirlist = [
				#desktop
				'desktop/nginx/output/',
				'desktop/ICE/output/',
				'desktop/apache/output/',
				
				#feed
				
		]
		return self.dir2str
		
	def getOutConfig(self):
		'''
			get a str2list dict means the outstr to [mysql table and col] 
		'''
		self.out2vec = {
			'InstallOK'	:	['desktop', 'xnstat_desktop_subver', 'installcount'  ],
			'UnstallOK'	:	['desktop', 'xnstat_desktop_unsubver', 'unstallcount'  ],
			'version'  	:	['desktop', 'xnstat_desktop_subver', 'xxoo'  ],
		}
		return self.out2vec

	def getErrorMsg(self):
		'''
			get error msg
		'''
		self.errormsg = {
			"hadoop-parttionear"	: 	"hadoop error parttionear error please check the hadoop command!",
			"hadoop-configfile"		: 	"hadoop error the out msg could not match the config file please check the filter object's out and the OutConfig"
			
		}
		
		return self.errormsg
	
	def getWarnMsg(self):
		'''
			get warn msg
		'''
		self.warnmsg = {
		}
		
		return self.warnmsg
