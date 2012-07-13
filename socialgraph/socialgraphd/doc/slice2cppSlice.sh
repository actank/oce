#!/bin/sh

export ICE_HOME=/opt/Ice-3.3

for ice in `find $HOME/cell/cxx -name "*.ice"`
do
	cd `dirname $ice`
	#echo `basename $ice`
	$ICE_HOME/bin/slice2cpp -I.. `basename $ice`
	cd -	
done
