#!/bin/bash
. /etc/profile
NOW=`date "+%Y-%m-%d %H:%M:%S"`
/usr/local/bin/python monitorImporter.py
STATUS=$?
if [ $STATUS -ne 0 ]
then
	echo "$NOW ALARM"
	wget -O /dev/null "http://10.22.198.81:2000/receiver?number=13601258102&message=$STATUS%20MSN%20Importer%20needs%20review%20at%20`date +%Y-%m-%dT%H:%M:%S`"
else
	echo "$NOW Silence"
fi

