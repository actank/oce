#!/bin/bash
source ~/.bashrc
source ./vars.sh
datetime=`/bin/date -d "$1 days ago" +\%Y-\%m-\%d`
echo "===================BEGIN============$datetime============="
echo "datetime: " ${datetime}
YEAR=`date +%Y`
today=`date -d today +%Y-%m-%d`
begintime=`date`
START=$(date +%s)

TASKNAME="dispatch"
#SCRIPTDIR="./script/"

LOCAL_OUTPUT="/home/jingwei.ye/data/${TASKNAME}"
REMOTE_OUTPUT_DIR="/user/jingwei.ye/stat/task/${TASKNAME}/output/"${datetime}"/"

MANYDAYS=`python GetDays.py 1 8`
DISPATCH_INPUT="/log_in/business/feed/dispatchto.${MANYDAYS}.bz2"
echo "MANYDAYS:" MANYDAYS


#${HADOOP} dfs -rmr ${REMOTE_OUTPUT_DIR}/${datetime}/*
#${HADOOP} dfs -mkdir ${REMOTE_OUTPUT_DIR}

MAPPER=../py/dispatch.mapper.py
REDUCER=../py/dispatch.reducer.py
AllInput=" -input $DISPATCH_INPUT "
echo "MAPPER: " $MAPPER
echo "REDUCER: " $REDUCER

TMPDIR=${LOCAL_OUTPUT}/${TASKNAME}.${datetime}.tmp
FINAL=${LOCAL_OUTPUT}/${TASKNAME}.${datetime}
echo "TMPDIR:" $TMPDIR
echo "FINAL:" $FINAL
rm -r $TMPDIR
rm -r $FINAL
mkdir $TMPDIR

$HADOOP dfs -rmr ${REMOTE_OUTPUT_DIR}
${HADOOP} jar ${STREAM}  \
	-Dmapred.reduce.tasks=1 \
	${AllInput} \
	-output ${REMOTE_OUTPUT_DIR} \
	-mapper ${MAPPER}  -reducer ${REDUCER}  -file ${MAPPER} -file ${REDUCER}
echo 'hadoop job OK!'

echo "Get the output into the TMPDIR ${TMPDIR} ok!"

${HADOOP} dfs -get ${REMOTE_OUTPUT_DIR}/part-00*   $TMPDIR
cat  ${TMPDIR}/part* > $FINAL

#echo "get the output into the  ${local_output}   ok!"
#${HADOOP} dfs -get ${REMOTE_OUTPUT_DIR} ${local_output}/"dispatch."${datetime}
#/usr/kerberos/bin/kinit -kt /etc/krb5.keytab
#scp ${local_output}/"dispatch."${datetime}/part-00000 xce@fdb11.xce.d.xiaonei.com:~/StatUtil/data/dispatch2/dispatch.${datetime}
#rm ${local_output}/"dispatch."${datetime}/part-00000 


END=$(date +%s)
echo "the begintime: $begintime"
echo "the endtime: `date`"
DIFF=$(( $END - $START ))
echo "Take "$DIFF" seconds"
echo "===================END============$datetime============="

