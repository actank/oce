import sys,smtplib,poplib
import time
from datetime import timedelta, date
from email.MIMEText import MIMEText
from email.MIMEMultipart import MIMEMultipart
from mid_config import MidConfig
yesterday = str(date.today()-timedelta(1))

class MidMail(object):
	
	def send(self, msg, toaddress = "zhaohui.tang@opi-corp.com"):
		smtpserver = "smtp.opi-corp.com"
		fromaddress = "zhaohui.tang@opi-corp.com"
		msgRoot = MIMEMultipart('related')
		msgRoot['Subject'] = yesterday +'  AdQuality System Report'
		msgRoot['From'] = fromaddress
		msgRoot['To'] = toaddress
		msgRoot.preamble = 'This is a multi-part message in MIME format.'
		msgAlternative = MIMEMultipart('alternative')
		msgRoot.attach(msgAlternative)
		html='<html><body>' + msg + '</body></html>'
		msgText = MIMEText(html, 'html', 'utf-8')
		msgAlternative.attach(msgText)
		
		s = smtplib.SMTP(smtpserver)
		s.login("zhaohui.tang@opi-corp.com","tang170500")
		s.sendmail(fromaddress, toaddress, msgRoot.as_string())
		
	
	def sendError(self, errorkey):
		sendmsg = errorkey
		midconfig = MidConfig()
		errormsg = midconfig.getErrorMsg()
		if(errorkey in errormsg):
			sendmsg = errormsg[errorkey]
		self.send(sendmsg)
	
	def sendLobbyStat(self, msg, *to):
		for p in to:
			self.send(msg, p)
			
	def sendSuccess(self):
		self.send("success!")
		
