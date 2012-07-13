#! /usr/bin/python
# -*- coding: utf8 -*-
# 中文注释不会导致warning

import codecs
import socket
import urllib2

class Utf8UrlGetter:
  def GetEncoding(self, content):
    pos = content.find('<meta ')
    while pos > 0:
      end = content.find("/>", pos)
      meta = content[pos : end]
      pos = content.find('<meta ', end)
      pattern = 'charset='
      charset_pos = meta.find(pattern)
      if charset_pos > 0:
        end = charset_pos + len(pattern) + 1
        while meta[end].isalpha() or meta[end].isdigit() or meta[end] == '-':
          end += 1
        encoding = meta[charset_pos + len(pattern) : end]
        try:
          if codecs.lookup(encoding):
            return encoding
        except:
          print "error : unsupported encoding %s." % encoding 
          return None
    return None

  def GetHtml(self, url, timeout):
    html = ''
    try:
      socket.setdefaulttimeout(timeout)
      req = urllib2.Request(url)
      c = urllib2.urlopen(req)
      html = c.read()
    except:
      print "error : open url %s failed" % url
      return ''

    encoding = self.GetEncoding(html)
    if encoding and encoding != 'utf-8':
      html = unicode(html, encoding, 'ignore').encode('utf-8', 'ignore')
    return html

if __name__ == '__main__':
  url_getter = Utf8UrlGetter()
  url = 'http://blog.sina.com/lichi'
  html = url_getter.ReadHtml(url, 2)
  print len(html)

