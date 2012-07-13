# -*- coding: utf-8 -*-
from PrepUtil_v2 import WordPrepUtil
from PrepUtil_v2 import WordFilter
import os

#get important phrase
wordSegFlag = False
idfMethod = 'userIndependent'
idfThreshold = 1
w = WordPrepUtil()
w.Winit(wordSegFlag,idfMethod)
#w.GenWordSeg('c.txt')
#w.GenWordSeg('c.txt').GenWordInvLst('').GenTFIDF()

#for i in w.segLst:
#    print i.decode('utf8')

#w.GetAdContentInvLst('ad_content.txt').GenTFIDF().GenImportantPhrase(5,7)
w.GetAdContentInvLst('ad_content.txt').GenAdContentTFIDF().GenImportantPhrase(0,2.4)


trainDict = dict()
with open('food.train') as f:
    foodText = f.readlines()

with open('finance.train') as f:
    financeText = f.readlines()

trainDict['food'] = foodText
trainDict['finance'] = financeText

with open('stoplst.txt') as f:
    stopText = f.readlines()

stopLst = list()
for line in stopText:
    ss = line.split()
    word = ss[-1]
    if word[-1] == '\n':
        word = word[0:-1]
    stopLst.append(word)

ft = WordFilter()
ft.WinitDict(w.importPhrase.keys(),trainDict)
#ft.GetState('TanCorpMinTrain').GenImportantWord(0.01)
ft.GetStateDict(trainDict).GenImportantWord(0.01,stopLst)
for i in ft.importantWords:
    print i.decode('utf8') + str(ft.importantWords[i])
    
'''
for i in ft.wordLst:
    if i not in ft.importantWords:
        print i.decode('utf8')
'''

