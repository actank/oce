#! /usr/bin/python
# -*- coding: utf8 -*-
# 中文注释不会导致warning

import socket
from BeautifulSoup import *
from urlparse import urljoin
import MySQLdb

def GetDateTime(soup):
  date = ''
  time = ''
  text = str(soup)
  date_pat = re.compile('\d{4}-\d{1,2}-\d{1,2}')
  g = date_pat.search(text)
  if g: date = g.group()

  time_pat = re.compile('\d{1,2}:\d{1,2}')
  g = time_pat.search(text)
  if g: time = g.group()

  return "%s %s" % (date, time)

def GetHref(soup):
  if soup.name != 'a':
    soup = soup.find('a')
  if soup.name == 'a':
    for attr in soup.attrs:
      if attr[0] == 'href':
        return attr[1]
  return None

# 从html页面中提取文字（不带标签）
def GetTextOnly(soup):
  SPACE = '&nbsp;'
  v = soup.string
  if v == None:
    c = soup.contents
    result = ''
    for t in c:
      subtext = GetTextOnly(t)
      if subtext[len(subtext) - len(SPACE):] == SPACE:
        subtext = subtext[0 : len(subtext) - len(SPACE)]

      if len(result) > 0 and result[-1] != ' ':
        result += ' '
      subtext = subtext.replace("\r", " ")
      subtext = subtext.replace("\n", " ")
      subtext = subtext.replace("\t", " ")
      result += subtext
    return result
  else:
    # 过滤无信息的标签
    if v.parent.name in ('script', 'noscript', 'style'):
      return ''
    return v.strip()


class HtmlExtractor:
  def __init__(self, node_name, node_attrs, node_handler = None):
    self.node_name_ = node_name
    self.node_attrs_ = node_attrs
    self.node_handler_ = node_handler

  def Extract(self, soup):
    matched = soup.findAll(self.node_name_, self.node_attrs_)
    if len(matched) <= 0:
      print "no matched node", self.node_name_, self.node_attrs_
    if self.node_handler_:
      return [self.node_handler_(m) for m in matched]
    else:
      return [m for m in matched]

if __name__ == '__main__':
  doc = ['<html><head><title>Page title</title></head>',
         '<body><p id="firstpara" align="center"><a href="http://www.sina.com.cn/">sina</a>This is paragraph <b>one</b>.',
         '<p id="secondpara" align="blah">This is paragraph <b>two</b>.',
         '</html>']
  soup = BeautifulSoup(''.join(doc))
  extracor = HtmlExtractor('p', {"id" : "firstpara", "align" : "center"}, GetHref)
  print extracor.Extract(soup)

  extracor = HtmlExtractor('p', {"id" : "secondpara", "align" : "blah"}, GetTextOnly)
  print extracor.Extract(soup)

