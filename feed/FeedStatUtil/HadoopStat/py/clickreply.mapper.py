#!/usr/bin/env python
import sys
import types
import os
import re
current_path = str(os.getcwd())
sys.path.append(current_path)

#cl@105142708#249791720 2
#105142708#249791720 click reply recv

testers = set([239556923,221489137,238489851,240599893,128487631,225092473,241610785,220678672,322856518,34344507,343518722,347348947,347349486,347735846,200000032])
def CheckType(type):
	return True
	tmp = type / 100
	return tmp != 29
def CheckToid(id):
	#return id % 10 < 10 or id in testers 
	#return id % 20 == 5 or id % 20 == 6
	return id / 100 % 10 == 1 or id in testers 
test = True 

mdict={}
def Insert(to,featureid,cl,rp):
	if not mdict.has_key(to):
		mdict[to] = {}
	if featureid not in mdict[to]:
		mdict[to][featureid]=[cl,rp]
		return
	mdict[to][featureid][0]+=cl
	mdict[to][featureid][1]+=rp

def HandleClick(segs):
	#segs = segs[0:4]
	[feedid,toid,ip,url] =segs
	if toid == "0":
		return
	#if test and not CheckToid(int(toid)):
	#	return
	if(re.search("ref=hotnewsfeed", url)):
		m = re.search(r"(?<=sfet=)\d+",url)
		if(m):
			stype = m.group(0);
			if(not CheckType(int(stype))):
				return
			Insert(int(toid),-int(stype),1,0)
		n = re.search(r"(?<=ff_id=)\d+",url)
		if(n):
			actor = n.group(0)
			#print toid+"\t"+actor+ "\t1\t0"
			Insert(int(toid),int(actor),1,0)
def HandleReply(segs):
    #[toid,source,stype,actor] =segs
    [toid,actor,stype,source] =segs
    if(not CheckType(int(stype))):
        return
    #if test and not CheckToid(int(toid)):
    #    return
    #print  toid+"\t"+actor + "\t0\t1"
    #print  toid+"\t-"+stype + "\t0\t1"

def HandleReply_new(segs):
	#[toid,source,stype,actor] =segs
	[type,seconds,toid,actor,stype,source,feed] =segs
	if(not CheckType(int(stype))):
		return
	#if test and not CheckToid(int(toid)):
	#	return
	Insert(int(toid),int(actor),0,1)
	Insert(int(toid),-int(stype),0,1)

def HandleRecv(segs):
	if(len(segs) <=2):
		return
	actor = segs[0]
	stype = segs[1]
	for i in range(2, len(segs)):
		print segs[i]+"\t"+actor+"\t0\t0\t1"
		#print segs[i]+"\t-"+stype+"\t0\t0\t1"


for line in sys.stdin:
	if(line):
		try:
			line = line.strip()
			segs = line.split(" ")
			if(segs[0] == 'r'):
				HandleReply_new(segs)
			elif len(segs) >= 4 and segs[3][0] == "h":
				HandleClick(segs)
			elif(segs[0][-1] == 'y'):
				segs[0]=segs[0][0:-5]
				HandleReply(segs)

			#else:
			#	HandleRecv(segs)
		except:
			pass

for toid,fc in mdict.items():
	for feature,count in fc.items():
		print str(toid)+"\t"+str(feature)+"\t"+str(count[0])+"\t"+str(count[1])


