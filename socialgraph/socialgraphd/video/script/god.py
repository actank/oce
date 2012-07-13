#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import re
from datetime import date
from datetime import timedelta
import httplib
import urllib

class Share(object):
    def __init__(self):
        pass

class VideoShare(Share):
    def __init__(self,  url_md5, url, user_id, share_id, title, thumb_url):
        self.__md5 = url_md5
        self.__url = url
        self.__uid = user_id
        self.__share_id = share_id
        self.__score = 0
        self.__title = title
        self.__thumb_url = thumb_url.replace("\n", "")
        self.__tag = []

    def md5(self):
        return self.__md5
    
    def url(self):
        return self.__url
    
    def user_id(self):
        return self.__uid
    
    def share_id(self):
        return self.__share_id
        
    def score(self):
        return self.__score
    
    def set_score(self, score):
        self.__score = score
    
    def title(self):
        return self.__title
    
    def set_title(self, t):
        self.__title = t
        
    def thumb_url(self):
        return self.__thumb_url
        
    def set_thumb_url(self, tu):
        self.__thumb_url = tu
    
    def tag(self):
        return self.__tag
    
    def set_tag(self, tag):
        self.__tag = tag
    
  
   

class ShareUrl(object):
    def __init__(self, md5, url, thumb_url, title, view_count, share_count, comment_count, zan_count):
        self.md5 = md5
        self.url = url
        self.thumb_url = thumb_url
        self.title = title
        self.view_count = view_count
        self.share_count = share_count
        self.comment_count = comment_count
        self.zan_count = zan_count

class BaseClassifier(object):
    cate_dict = {} # {word:unique id}
    
    def __init__(self,  share_object):
        self.share = share_object
        
    def Classify(self):
        return None
        
    def GetUrl(self):
        return self.share.url()
        
    def GetTagIndexSeq(self, cate_seq):
        cate_index_seq = []
        if cate_seq is not None:
            for c in cate_seq:
                if c in BaseClassifier.cate_dict:
                    cate_index_seq.append(BaseClassifier.cate_dict[c])
                else:
                    value = str(len(BaseClassifier.cate_dict) + 1)  # The value 0 is not a good cate_index value!
                    BaseClassifier.cate_dict[c] = value
                    cate_index_seq.append(value)
        return cate_index_seq

class EmptyClassifier(BaseClassifier):
    def __init__(self,  share_object):
        super(EmptyClassifier,  self).__init__(share_object)

class YoukuClassifier(BaseClassifier):
    def __init__(self,  share_object):
        super(YoukuClassifier,  self).__init__(share_object)
    
    def Classify(self):
        content = None
        try:
            f = urllib.urlopen(self.GetUrl())
            content = f.read()
        except IOError, e:
            pass
        
        if content is None:
            return None
            
        content = re.sub("[\r\n\t]", "",  content)
        p = "<div class=\"crumbs\">(.*)&gt;&gt;</div>"
        m = re.search(p, content)
        cates = []
        if m is not None:
            p2 = "target.*\">(.*)</a>"
            for c in m.group().split("&gt;&gt"):
                m2 = re.search(p2,  c)
                if m2 is not None:
                    if m2.group(1).strip():
                        cates.append(m2.group(1))
        
        cates = self.GetTagIndexSeq(cates)
        self.share.set_tag(cates)
    
class TudouClassifier(BaseClassifier):
    def __init__(self,  share_object):
        super(TudouClassifier,  self).__init__(share_object)
    
    def Classify(self):
        content = None
        try:
            f = urllib.urlopen(self.GetUrl())
            content = f.read()
        except IOError, e:
            pass
        
        if content is None:
            return None
        
        try:
            content = content.decode("GBK")
        except:
            return None
        lines = content.split("\n")

        c = None
        for line in lines:
            if line.find("vcate") != -1:
                c = line
        
        cates = []
        if c is not None:
            m = re.search(r"<div class=\"vcate\">(.*)</a></li>", c, flags=re.UNICODE)
            if m is not None:
                s = m.group(1).split("</a></li>")[0]
                start = s.rfind(">")
                if start != -1:
                    cates.append(s[start+1:])
        cates = self.GetTagIndexSeq(cates)
        self.share.set_tag(cates)
       
#domains = ("youku.com", "56.com", "ku6.com", "tudou.com", "sina.com.cn", "umiwi.com", "yinyuetai.com")

domain_classifiers = [("youku.com",  YoukuClassifier)]  #, ("tudou.com", TudouClassifier)]


def GetDomain(str):
    pattern = "http://([\w\.]+)/.*"
    m = re.match(pattern,  str)
    if m is not None:
        domain = m.group(1)
        if domain in domains:
            return domain
    return None

       
def GetClassfiers(video_share_dict):
    classifier_seq = []
    if video_share_dict is not None:
        for (k, v) in video_share_dict.items():
            for (domain, classifier) in domain_classifiers:
                if v.url().find(domain) != -1:
                    classifier_seq.append(classifier(v))
                else:
                    classifier_seq.append(EmptyClassifier(v))
    return classifier_seq

def GetFileLines(prefix):
    file_abs_path = os.path.abspath('') + "/" + prefix + "." + GetDataString()
    lines = []
    try:
        file = open(file_abs_path, "r")
        for line in file:
            lines.append(line)
    except IOError, e:
        print e
    return lines

def GetDataString():
    today = date.today()
    duration = timedelta(days = 1)
    return str(today - duration)
    
def WriteTagIndex(cate_dict):
    file_abs_path = os.path.abspath('') + "/tag_index." + GetDataString()
    try:
        file = open(file_abs_path,  "w")
        
        if cate_dict is not None:
            for (k, v) in cate_dict.items():
                file.write("%s\t%s\n" % (k, v))
        file.close()
    except IOError, e:
        print e 

def WriteResultLog(logs):
    file_abs_path = os.path.abspath('') + "/video_share_result." + GetDataString()
    try:
        file = open(file_abs_path,  "w")
        if logs is not None:
            for log in logs:
                file.write("%s\n" % log)
        file.close()
    except IOError, e:
        print e 

def GetUrlObjectDict():
    dict = {}
    lines = GetFileLines("url")
    for line in lines:
        words = line.split("\t")
        if len(words) == 8:
            #url_md5,url,title,thumb_url,view_count,share_count,comment_count,zan_count
            share_url = ShareUrl(words[0], words[1], words[2], words[3], words[4], words[5], words[6], words[7]) 
            dict[words[0]] = share_url
    return dict

def GetVideoShareDict():
    dict = {}
    lines = GetFileLines("share")
    for line in lines:
        words = line.split("\t")
        if len(words) == 6:
            video_share = VideoShare(words[0], words[1], words[2], words[3], words[4], words[5])
            dict[words[0]] = video_share
    return dict

def FilterVideoShare(video_share_dict, share_url_dict):
    if (video_share_dict is not None) and (share_url_dict is not None):
        for k in video_share_dict.keys():
            if share_url_dict.has_key(k):
                video_share_dict[k].set_score(share_url_dict[k].share_count)
            else:
                del video_share_dict[k]

def main():
    #step1:get url object dict
    #step2:get video share dict
    #step3:filter video share dict
    #step4:get classifiers and do Classify method
    #step5:write log
    share_url_dict = GetUrlObjectDict()
    print len(share_url_dict)
    
    video_share_dict = GetVideoShareDict()
    print len(video_share_dict)
    
    FilterVideoShare(video_share_dict, share_url_dict)
    print len(video_share_dict)
    
    classifiers = GetClassfiers(video_share_dict)
    print "len(classifiers):%d" % (len(classifiers))
    for classifier in classifiers:
        classifier.Classify()

    
    logs = []
    for (k, v) in video_share_dict.items():
        #md5,user_id,share_id,score,tags
        md5 = v.md5()
        uid = v.user_id()
        #share_url = "http://share.renren.com/share/%s/%s" % (uid, v.share_id())
        score = v.score()
        tags = "-".join(v.tag())
        title = v.title()
        l_tags = []
        thumb_url = v.thumb_url()
        log = "%s\t%s\t%s\t%s\t%s\t%s\t%s" % (md5, uid, v.share_id(), score, tags, thumb_url,  title)
        #log = "%s\t%s\t%s\t%s\t%s\t%s" % (md5, uid, v.share_id(), score, tags, "->".join(l_tags))
        #print log
        logs.append(log)
        
    WriteResultLog(logs)
    WriteTagIndex(BaseClassifier.cate_dict)
    print 'main() done'
 
if __name__ == '__main__':
    sys.exit(main())
    

