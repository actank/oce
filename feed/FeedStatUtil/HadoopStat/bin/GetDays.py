import commands
import sys 
begin = int(sys.argv [1])
end =int(sys.argv [2])
out = "{" 
for i in range(begin,end):
  cmd = "echo `date -d \""+str(i)+" days ago\" +\%Y-\%m-\%d`"
  thisday = commands.getoutput(cmd)
  out = out+ thisday+","
out += '}' 
print out
