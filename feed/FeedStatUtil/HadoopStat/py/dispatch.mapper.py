#!/usr/bin/env python
import sys
import types
import os
import re
current_path = str(os.getcwd())
sys.path.append(current_path)

#cl@105142708#249791720 2
#105142708#249791720 click reply recv

d_map = {}

def HandleRecv_new(segs):
	if(len(segs) <=2):
		return
	actor = int(segs[2])
	stype = int(segs[3])
	tmp = int(stype) /100
	if(tmp  == 29 ):
		return
	if actor not in d_map:	
		d_map[actor] = {}
	if stype not in d_map[actor]:
		d_map[actor][stype] = 0
	d_map[actor][stype] += 1
	#print actor+"#"+stype+"\t1"

def HandleRecv(segs):
    	if(len(segs) <=2):
        	return
	actor = int(segs[0])
	stype = int(segs[1])
	tmp = int(stype) /100
	if(tmp  == 29 ):
		return
	d_map[actor][stype] += 1
	#print actor+"#"+stype+"\t1"


for line in sys.stdin:
	if(line):
		try:
			line = line.strip()
			segs = line.split(" ")
			if len(segs) > 1 and segs[0][0] == "d":
				HandleRecv_new(segs)
			else:
				pass
				#HandleRecv(segs)
		except:
			pass
for actor,sc in d_map.iteritems():
	for stype,c in sc.iteritems():
		print str(actor)+"#"+str(stype)+"\t" + str(c)


