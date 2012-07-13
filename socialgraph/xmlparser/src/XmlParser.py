#! /usr/bin/env python
# -*- coding: utf8

import sys
import os
import time as CT
import commands
import re
#import xml.etree.ElementTree as ET
import elementtree.ElementTree as ET

class Item:
    def __init__(self, time, command):
        self.time_ = time
        self.command_ = command

def check_load_time(time):
    pattern = re.compile('(\d+) (\d+) \S+ \S+ \S+')
    matcher = pattern.match(time)
    minute = matcher.group(1)
    hour = matcher.group(2)
    #输入检查，分钟，小时
    if int(minute) > 59 or int(minute) < 0:
        return 0
    if int(hour) > 23 or int(hour) < 0:
        return 0
    return 1

def check_load_command(command):
    path = commands.getstatusoutput("pwd")
    new_command = ""
    if command.startswith("/") == True:
        new_command = path[1] + command
    else:
        new_command = path[1] + "/" + command
    return new_command


def write_in_crontab(content):
    path = commands.getstatusoutput("pwd")
    tmp_crontab_file = path[1] + "/tmp_crontab_file"
    f = file(tmp_crontab_file, "w")   
    f.write(content)
    f.flush()
    f.close
    crontab_exe = "crontab %s" % (tmp_crontab_file)
    os.system(crontab_exe)
    remove_tmp_file = "rm %s" % (tmp_crontab_file)
    os.system(remove_tmp_file)  
            
def load_xml_file(file_name):
    #获取机器的nickname, 解析出本地机器名
    local_name_list = commands.getstatusoutput("echo $NICKNAME")
    local_name = local_name_list[1]
    pattern = re.compile('(\S+).xce')
    matcher = pattern.match(local_name)
    local_node = matcher.group(1)
    
    #解析xml，获取属于本机要加载的程序
    root = ET.parse(file_name).getroot()
    all_nodes = root.findall('node');
    item_list = []
    for node in all_nodes:
        tmp_node = node.get("name");
        if tmp_node == local_node:      #找出属于本台机器应该加载的业务
            all_subnodes = node.findall('updator-instance')
            for sub in all_subnodes:
                time = sub.get("time")
                command = sub.get("command")
                status = check_load_time(time)      #check时间格式
                if status == 0:
                    sys.stderr.write("error load time %s\n" % (time))
                    sys.exit()
                new_command = check_load_command(command)
                item_list.append(Item(time, new_command))
    return item_list

def create_content(item_list):
    #获取现在crontab中的信息
    tmp_content = commands.getstatusoutput("crontab -l")
    crontab_content = tmp_content[1]
    crontab_content_list = crontab_content.split("\n")

    #获取当前XGProject/bin路径
    tmp_path = commands.getstatusoutput("pwd")
    path = tmp_path[1]
    
    #在已有crontab内容中查找不属于XGProject工程的项
    original_crontab_content = []
    for line in crontab_content_list:
        if line.find(path) < 0:
            original_crontab_content.append(line)

    #拼接成新的crontab内容
    content = ""
    for original_line in original_crontab_content:
        content += original_line + "\n"
    for it in item_list:
        content += it.time_ + " " + it.command_ + "\n"
    return content

if __name__ == '__main__':
    #判断程序参数是否正确 python exe file_path
    if (len(sys.argv) != 2):
        sys.exit();
    xml_file = sys.argv[1]
    
    #判断xml文件是否存在
    if (os.path.exists(xml_file) != True):  
        sys.stderr.write("%s is not existed!\n" % (xml_file))
    
    #有内容修改, svn diff展示xml文件的差别
    svn_checker = 'svn diff %s' % (xml_file)
    svn_status = commands.getstatusoutput(svn_checker)
    if len(svn_status[1]) > 0:
	print svn_status[1]
    	CT.sleep(0.1)
    	var = raw_input("Would you wanna commit? (yes or no): ");
    	if (var == 'no'):
    	    sys.stderr.write("abort committing!\n")
    	    sys.exit();
    	elif (var != 'yes'):
    	    sys.stderr.write("error input!\n")
    	    sys.exit();
    
    #加载xml中的数据进crontab中
    item_list = load_xml_file(xml_file)
    
    #创建crontab内容
    content = create_content(item_list)
    
    #写入crontab
    write_in_crontab(content)

    #有内容修改，提交配置文件
    if len(svn_status[1]) > 0:
    	svn_commit = 'svn ci %s -m ""' % (xml_file)
    	os.system(svn_commit)
