#!/bin/bash

user=`whoami`
date=`date "+%Y-%m-%d %H:%M:%S"`
hostname=`hostname`
LANG=C

if [ -d .git ]; then
  revision=`git log -1 --pretty=format:"%H"`
else
  revision=`svn info | sed -n -e 's/Last Changed Rev: \(.*\)/\1/p'`
fi

echo "@xce.util.tools.VersionAnnotation(
  revision=\"$revision\",
  build=\"$hostname\",
  user=\"$user\",
  date=\"$date\")
"


