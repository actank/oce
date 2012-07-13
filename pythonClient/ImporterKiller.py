import os, sys, time

def getImporterServicePID():
	pss = os.popen('ps aux | grep Importer').read().split('\n')
	p = None
	for ps in pss:
		if ps.find('-server') > 0:
			p = ps
	if p == None:
		return -1
	pid = int(p.split()[1])
	return pid

oldPid = getImporterServicePID()
for i in range(0, 3):
	pid = getImporterServicePID()
	if pid == oldPid:
		os.system('kill ' + str(pid))
		print 'kill ImporterService ' + str(pid)
		time.sleep(1)
	else:
		break
if pid == oldPid:
	os.system('kill -9 ' + str(pid))
	print 'kill -9 ImporterService ' + str(pid)
