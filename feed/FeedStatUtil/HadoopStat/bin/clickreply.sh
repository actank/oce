#!/bin/bash
source ~/.bashrc
source ./vars.sh
datetime=`/bin/date -d "$1 days ago" +\%Y-\%m-\%d`
echo "===================BEGIN============$datetime============="
#datetime=$1
echo "date:" ${datetime}
YEAR=`date +%Y`
today=`date -d today +%Y-%m-%d`
begintime=`date`
START=$(date +%s)

TASKNAME="clickreply"
SCRIPTDIR="../py/"

LOCAL_OUTPUT="/home/jingwei.ye/data/${TASKNAME}"
REMOTE_OUTPUT_DIR="/user/jingwei.ye/stat/task/${TASKNAME}/output/"${datetime}"/"
MANYDAYS=${datetime}
MANYDAYS=`python GetDays.py 1 8`
echo "MANYDAYS:" $MANYDAYS
CLICK_INPUT="/log_in/active_track/access/$YEAR/access.${MANYDAYS}.bz2"
CHEWEN_CLICK_INPUT="/log_in/business/chewen/feedState.${MANYDAYS}.bz2"
REPLY_INPUT="/log_in/business/feed/reply.${MANYDAYS}.bz2*"
#REPLY_INPUT="/user/xce/LogData/logger/feed/reply.${MANYDAYS}*"

MAPPER=../py/clickreply.mapper.py
REDUCER=../py/clickreply.reducer.py
AllInput=" -input $CLICK_INPUT -input $REPLY_INPUT -input $CHEWEN_CLICK_INPUT "
#AllInput=" -input $CHEWEN_CLICK_INPUT "
#AllInput=" -input $REPLY_INPUT"
#AllInput=" -input $CLICK_INPUT "
echo "MAPPER: " $MAPPER
echo "REDUCER: " $REDUCER
echo "CLICK_INPUT: " $CLICK_INPUT
echo "REPLY_INPUT: " $REPLY_INPUT
echo "AllInput:" $AllInput

TMPDIR=${LOCAL_OUTPUT}/${TASKNAME}.${datetime}.tmp
FINAL=${LOCAL_OUTPUT}/${TASKNAME}.${datetime}
echo "TMPDIR:" $TMPDIR
echo "FINAL:" $FINAL
rm -r $TMPDIR
mkdir $TMPDIR

${HADOOP} dfs -rmr ${REMOTE_OUTPUT_DIR}
${HADOOP} jar ${STREAM}  \
	-Dmapred.reduce.tasks=15 \
	${AllInput} \
	-output ${REMOTE_OUTPUT_DIR} \
	-mapper ${MAPPER}  -reducer ${REDUCER}  -file ${MAPPER} -file ${REDUCER}

echo 'hadoop job OK!'

#echo "get the output into the TMPDIR ${TMPDIR} ok!"

${HADOOP} dfs -get ${REMOTE_OUTPUT_DIR}/part-00*   $TMPDIR &&
echo "TMPDIR:" $TMPDIR
cat  ${TMPDIR}/part* > $FINAL &&
echo "FINAL:" $FINAL

END=$(date +%s)
echo "the begintime: $begintime"
echo "the endtime: `date`"
DIFF=$(( $END - $START ))
echo "Take "$DIFF" seconds"
echo "===================END============$datetime============="
