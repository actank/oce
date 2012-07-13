#!/bin/sh

#echo "file name :" $1

LOGPATH="/data/xntalk/stat/log"


date="log."`date "+%Y%m%d-%H%M"`
echo $date

list=`ls $LOGPATH |grep -E "log\..*"`

for i in $list
do
 #if [ "$i" == "`echo "$i $date"|sort|head -1`" ];then
 if [[ "$i" < "$date" ]];then
 echo "to stat $i"


awk -F'[ []' 'BEGIN{ sql="insert into xntalk_action (userid, time, action) values (-1,now(),\"NULL\")" }
	{ 
	#print $7;
	if ($7 ~/^\/feedback\/.*&.*/){
	split($7,p,"/");
	split(p[3],props,"&");
	#print "A: action="props[1],"   userid="props[2];
	sql=sql" ,("props[2]",'\''" $4" "$5  "'\'', \"" props[1] "\") ";
	}
	else if ($7 ~/^\/feedback\/[^&]*/){
	split($7,p,"/");
	#print "B: action="p[3];
	
	sql=sql" ,(0,'\''" $4" "$5  "'\'', \"" props[3] "\") ";
	}
	} END{ print sql }' "$LOGPATH/$i" | mysql -uipo2008 -pmeiguiyuan -hcat -P3306  --default-character-set=utf8 stat


 mv "$LOGPATH/$i" "$LOGPATH/backup/$i"
 else
 echo $i
 fi 
done


