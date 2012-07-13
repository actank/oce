#
# Translate AdLog's logs (ad_log, pv_log, click_log) to the true format;
# Run once per minute.
#

import os
import sys
import time
import datetime
import glob


para_pool = []

def Deal(para):
	
	para_server_name = para['para_server_name']
	para_server_log_file_title = para['para_server_log_file_title']
	para_storage_dir = para['para_storage_dir']
	para_res_log_file_title = para['para_res_log_file_title']
	para_res_log_file_tail = para['para_res_log_file_tail']
	para_gz = para['para_gz']

	nowpath = "/data/xce/Ad/log/" + para_server_name

	if(os.path.exists(nowpath)):
		os.chdir(nowpath)
		#filelist = glob.glob('pv_val.*')
		filelist = glob.glob(para_server_log_file_title + '.*')
		filelist.sort()
		
		if(os.path.exists(para_storage_dir)):
			pass
		else:
			cmd = "mkdir " + para_storage_dir
			os.system(cmd)
		
		for filename in filelist:
			dirs = []
			logtime = filename.split(".")[1]
			logyear = logtime.split("-")[0]
			logmonth = logtime.split("-")[1]
			logday = logtime.split("-")[2]
			loghour = logtime.split("-")[3]
			logminute = logtime.split("-")[4]
			#charge_log_dir = nowpath + "/bp_pv/" + logyear + "/" + logmonth + "/" + logday  + "/" + loghour
			log_dir = nowpath + "/" + para_storage_dir +"/" + logyear + "/" + logmonth + "/" + logday  + "/" + loghour
			#charge_log_name = "pv_val_"+ logyear + "_" + logmonth +  "_" + logday + "_" + loghour + "_" + logminute + ".log"
			if(para_storage_dir == 'charge_log'):
				log_name = para_res_log_file_title + "_"+ logyear + logmonth + logday + loghour + logminute + "_rec." + para_res_log_file_tail
			else:
				log_name = para_res_log_file_title + "_"+ logyear + "_" + logmonth +  "_" + logday + "_" + loghour + "_" + logminute + "." + para_res_log_file_tail
			dirs.append(logyear)
			dirs.append(logmonth)
			dirs.append(logday)
			dirs.append(loghour)
			path = nowpath + "/" + para_storage_dir
			for dir in dirs:
				path += "/" + dir
				if(os.path.exists(path)):
					pass
				else:
					cmd = "mkdir " + path
					os.system(cmd)
			cmd = "cd " + log_dir + " && >" + log_name
			os.system(cmd)
			read_filename = open(nowpath + "/" + filename)
			write_filename = open(log_dir + "/" + log_name, "w")
			if(para_storage_dir == 'charge_log'):
				write_filename.write("RecStart\n")
			line_number = 1
			for read_line in read_filename:
				if(read_line.find("empty") == -1):
					#print read_line
					if(para_storage_dir == 'charge_log'):
						read_line = str(line_number) + "," + read_line
					write_filename.write(read_line)
					line_number += 1
			if(para_storage_dir == 'charge_log'):
				write_filename.write("RecEnd\n")
			write_filename.close()
			read_filename.close()
			cmd = "cd " + nowpath + " && rm -f " + filename
			os.system(cmd)
			if(para_gz):
				cmd = "cd " + log_dir + " && gzip " + log_name
				os.system(cmd)
				cmd = "cd " + log_dir + " && rm -f " + log_name 
				os.system(cmd)
	else:
		print 'no dir : ' + nowpath


def ParaCheck(para):
	keys = para.keys()
	str = []
	str.append('para_server_name')
	str.append('para_server_log_file_title')
	str.append('para_storage_dir')
	str.append('para_res_log_file_title')
	str.append('para_res_log_file_tail')
	str.append('para_gz')
	for s in str:
		if(s not in keys):
			return False
	return True
	
	
def AddDeal(server_name, server_log_file_title, storage_dir, res_log_file_title, res_log_file_tail, gz):
	para = {}
	para['para_server_name'] = server_name
	para['para_server_log_file_title'] = server_log_file_title
	para['para_storage_dir'] = storage_dir
	para['para_res_log_file_title'] = res_log_file_title
	para['para_res_log_file_tail'] = res_log_file_tail
	para['para_gz'] = gz
	if(ParaCheck(para) == True):
		print 'check ok'
		para_pool.append(para)
	else:
		print 'para check error'
def main():
	AddDeal('AdLog0', 'pv_val', 'bp_pv', 'pv_val', 'log', True)
	AddDeal('AdLog0', 'click_val', 'bp_click', 'click_val', 'log', True)
	AddDeal('AdLog0', 'ad_log', 'charge_log', 'settle_online', 'txt', False)
	for para in para_pool:
		print para
		Deal(para)
if __name__ == "__main__":
	main()





