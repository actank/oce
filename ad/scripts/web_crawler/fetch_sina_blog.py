#! /usr/bin/python
# -*- coding: utf8 -*-
# 中文注释不会导致warning

import sys
sys.path.append('./')

import socket
import urllib2
from BeautifulSoup import *
from urlparse import urljoin
import MySQLdb

import html_extractor
import url_utf8_getter


class UrlExtractorMap:
  def __init__(self):
    self.site_patterns_ = {}

  def AddExtractor(self, site, key, extractor):
    self.site_patterns_.setdefault(site, {})
    self.site_patterns_[site][key] = extractor

  def GetExtractors(self, url):
    for site in self.site_patterns_.keys():
      if url.find(site) >= 0:
        return self.site_patterns_[site]
    return {}

class Crawler:
  def Crawl(self, init_page, url_extractor_map):
    page = init_page
    url_getter = url_utf8_getter.Utf8UrlGetter()

    while page and len(page) > 0:
      extractors = url_extractor_map.GetExtractors(page)
      if len(extractors) == 0:
        print "url %s neglected" % (page)
        break
      print 'Fetching page', page, '...'
      html = url_getter.GetHtml(page, 8)
      if len(html) <= 0:
        print "url %s empty." % (page)
        continue

      soup = BeautifulSoup(html)
      res = {}
      for (key, ex) in extractors.items():
        res[key] = ex.Extract(soup)

      if 'title' in res.keys() and 'content' in res.keys():
        print res['title'][0], res['content'][0]
        if len(res['prev_href']) > 0:
          page = res['prev_href'][0]
        else:
          page = ''

      #for key in res.keys():
      #  print key, res[key][0]
      # if key == 'prev_href':
      #   if len(res[key]) > 0: # ==2 表示到最后一页
      #     page = res[key][0]
      #   else:
      #     page = ''
      #break

def GetPrevHref(soup):
  if soup.name != 'a':
    soups = soup.findAll('a')
  if len(soups) < 2:
    return None
  soup = soups[0]
  if soup.name == 'a':
    for attr in soup.attrs:
      if attr[0] == 'href':
        return attr[1]
  return None

if __name__ == '__main__':

  extractor_map = UrlExtractorMap()

  extractor_map.AddExtractor("http://blog.sina.com.cn", "title",
      html_extractor.HtmlExtractor('h2', {"class" : "titName SG_txta"}, html_extractor.GetTextOnly))
  extractor_map.AddExtractor("http://blog.sina.com.cn", "content",
      html_extractor.HtmlExtractor('div', {"class" : "articalContent  "}, html_extractor.GetTextOnly))
  extractor_map.AddExtractor("http://blog.sina.com.cn", "category",
      html_extractor.HtmlExtractor('td', {"class" : "blog_tag"}, html_extractor.GetTextOnly))
  extractor_map.AddExtractor("http://blog.sina.com.cn", "timestamp", 
      html_extractor.HtmlExtractor('span', {"class" : "time SG_txtc"}, html_extractor.GetDateTime))
  extractor_map.AddExtractor("http://blog.sina.com.cn", "prev_href", 
      html_extractor.HtmlExtractor('div', {"class" : "articalfrontback SG_j_linedot1 clearfix"}, GetPrevHref))

  crawler = Crawler()
  # lichi 
  # init_url = 'http://blog.sina.com.cn/s/blog_4a315bad01017vd5.html'
  # danbin168
  # init_url = 'http://blog.sina.com.cn/s/blog_4a78b4ee010189t3.html'
  # lbw : 高崖水库, caijing
  # init_url = 'http://blog.sina.com.cn/s/blog_492508d501017exz.html'
  # 大学生活
  init_url = 'http://blog.sina.com.cn/s/blog_4c7425c601017p11.html'
  # init_url = 'http://blog.sina.com.cn/s/blog_4a61db900100pch9.html'
  # init_url = 'http://blog.sina.com.cn/s/blog_48dcdda201017jrp.html'
  # init_url = 'http://blog.sina.com.cn/s/blog_48b1e29801017vx0.html'
  crawler.Crawl(init_url, extractor_map)

