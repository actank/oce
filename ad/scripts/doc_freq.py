#!/usr/bin/env python
# encoding: utf-8

import sys
import mmseg
import math
from operator import itemgetter
#from chinese_word_segment import WordSegment

def GetTermsFrequency(text):
  ret = {}
  for w in mmseg.seg_txt(text):
    w = w.strip()
    if len(w) > 0:
      ret.setdefault(w, 0)
      ret[w] += 1
  return ret

def GetDocList(file_name):
  doc_list = []
  training_file = open(file_name, "r")

  count = 0
  for line in training_file:
    count += 1
    doc_list.append(GetTermsFrequency(line.strip()))
  return doc_list

def GetDocFreqency(fname):
  doc_list = GetDocList(fname)
  doc_freq = {}
  term_freq = {}
  for doc in doc_list:
    doc_size = len(doc)
    for word, freq in doc.items():
      doc_freq.setdefault(word, {})
      doc_freq[word].setdefault("df", 0)
      doc_freq[word].setdefault("tf", 0.0)
      doc_freq[word]['df'] += 1
      # doc_freq[word]['tf'] += freq * 100.0 / doc_size
      doc_freq[word]['tf'] += freq

  return (len(doc_list), doc_freq)

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print "请指定输入文件列表"
    sys.exit(0)
  for arg in sys.argv[1:]:
    doc_freq = GetDocFreqency(arg)
    doc_count = doc_freq[0]
    tf_df = doc_freq[1]
    for item in sorted(tf_df.items(), key=itemgetter(1), reverse=False):
      print "%-8d %-8d %-8d %s" % (item[1]["df"], item[1]["tf"], doc_count, item[0])

