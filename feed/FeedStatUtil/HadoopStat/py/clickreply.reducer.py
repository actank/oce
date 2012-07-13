#!/usr/bin/env python
#!/usr/bin/env python
#cl@105142708#249791720 2
#105142708#249791720 click reply recv

import sys
res = {}
(last_key, oldc) = (None,0)
(last_u, d) = (None,{})
for line in sys.stdin:
	(u,f,c,r) =line.strip().split("\t")
	key = u + "\t" + f
	f = int(f)
	if last_u and last_u != u :
		for k in sorted(d.keys()):
			print "%s\t%s\t%s\t%s"%(last_u,k,d[k][0],d[k][1])
		#for k,v in d.iteritems():
		#	print "%s\t%s\t%s\t%s"%(last_u,k,v[0],v[1])
		#(last_key, oldc,oldr) = (key,int(c),int(r))
		last_u = u
		d = {}
		d[f] = [int(c),int(r)]
	else:
		last_u = u
		d[f] = [d.get(f,[0,0])[0] + int(c) ,d.get(f,[0,0])[1]+ int(r)]
		#(last_u, d) = (key,oldc+int(c))

if last_u:
	for k,v in d.iteritems():
		print "%s\t%s\t%s\t%s"%(last_u,k,v[0],v[1])
	#print "%s\t%s"	% (last_key,oldc)
	#print "%s %s"	% (last_key,last_c)
