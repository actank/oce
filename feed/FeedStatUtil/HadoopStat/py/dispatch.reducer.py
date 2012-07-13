#!/usr/bin/env python
#!/usr/bin/env python
#cl@105142708#249791720 2
#105142708#249791720 click reply recv

import sys
res = {}
(last_key, oldc) = (None,0)
for line in sys.stdin:
	(key,c) =line.strip().split("\t")
	#key = u+"\t"+f
	if last_key and last_key != key:
		print "%s\t%s"	% (last_key.replace("#","\t"),oldc)
		(last_key, oldc) = (key,int(c))
	else:
		(last_key, oldc) = (key,oldc+int(c))

if last_key:
	print "%s\t%s"	% (last_key.replace("#","\t"),oldc)
	#print "%s %s"	% (last_key,last_c)
