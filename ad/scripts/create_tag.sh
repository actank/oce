export SVN_EDITOR=vim
echo "请输入tag的名字，如："
echo "ad_engine_1_0_001_A_20101027"
read tagname
echo "请输入注释："
read message
svn copy http://svn.d.xiaonei.com/oce/main_project/trunk/ad http://svn.d.xiaonei.com/oce/main_project/tags/ad/$tagname -m "$message"
#svn copy http://svn.d.xiaonei.com/oce/main_project/trunk/ad http://svn.d.xiaonei.com/oce/main_project/tags/ad/ad_engine_1_0_001_A_20101027 -m "2010.10.27启动新版引擎重构"

