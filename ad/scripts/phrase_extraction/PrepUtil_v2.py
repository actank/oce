from __future__ import division
from pymmseg import mmseg
import os
import pickle
import math
import glob
 

class WordPrepUtil(object):
    def __init__(self):
        self.wordSegFlag = False
        self.idfMethod = 'userIndependent'
        self.segInMemo = False
        self.invInMemo = False
        self.segLst = dict()
        self.invLst = dict()
        self.TFIDFLst = dict()
        #self.mmseg = mmseg
        mmseg.dict_load_defaults()

    def Winit(self,wordSegFlag,idfMethod):
        self.wordSegFlag = wordSegFlag
        self.idfMethod = idfMethod
        return self
        
    def Release(self,x):
        if x == 'segLst':
            self.segInMemo = False
            self.segLst = dict()
        elif x == 'invLst':
            self.invInMemo = False
            self.segLst = dict()
        return self

    def Load(self,x,inputfold):
        with open(outputfold + '/' + x + '.txt','rb') as f:
            try:                
                if x == 'segLst':
                    self.segLst = pickle.load(f)
                    loadFileFlag = True
                    self.segInMemo = True
                elif x == 'invLst':
                    self.invLst = pickle.dump(f)
                    loadFileFlag = True
                    self.invInMemo = True
            except:
                loadFileFlag = False
                self.Release(x)
                
        if loadFileFlag == False:
            print ('Error: WordPrepUtil.Write: dump error')
        return loadFileFlag                                    
            
    def Write(self,x,outputfold):
        with open(outputfold + '/' + x + '.txt','wb+') as f:
            try:                
                if x == 'segLst':
                    pickle.dump(self.segLst,f)
                    dumpFileFlag = True
                    #self.segInMemo = True
                elif x == 'invLst':
                    pickle.dump(self.invLst,f)
                    dumpFileFlag = True
                    #self.invInMemo = True
            except:
                dumpFileFlag = False
        if dumpFileFlag == False:
            print ('Error: WordPrepUtil.Write: dump error')
        return dumpFileFlag                    

    def GetUidByFile(self,inString,method = 'default'):
        if method == 'default':
            return ''
            
                
    def GenWordSeg(self,inputFile):
        with open(inputFile) as f:
            rawText = f.read()        
        segRes = mmseg.Algorithm(rawText)
        self.segFID = inputFile
        self.segLst = list()
        for w in segRes:
            if not w.text.isalnum():
                self.segLst.append(w.text)                
        self.segInMemo = True        
        return self
    
    def GenWordSegString(self,ss,FID):        
        rawText = ss        
        segRes = mmseg.Algorithm(rawText)
        self.segFID = FID
        self.segLst = list()
        for w in segRes:
            if not w.text.isalnum():
                self.segLst.append(w.text)                
        self.segInMemo = True        
        return self
    

    def GenAdContentInvLst(self,outputFold=''):
        if self.segInMemo == True:
            #print 'filename is ' + self.segFID
            for word in self.segLst:
                #print 'word is ' + word.decode('utf8')
                if word not in self.invLst:
                    try:
                        self.invLst[word] = {self.segFID:1}
                    except:
                        return self
                else:
                    try:
                        self.invLst[word][self.segFID] = 1
                    except:
                        return self
            self.invInMemo = True
            return self
        else:            
            print('Error: WordPrepUtil.GenWordInvLst: segLst not in memo') 
        
    def GenWordInvLst(self,outputFold=''):
        if self.segInMemo == True:            
            for word in self.segLst:
                if word not in self.invLst:                        
                    self.invLst[word] = 1                    
                else:
                    self.invLst[word] += 1
            self.invInMemo = True
            return self
        else:            
            print('Error: WordPrepUtil.GenWordInvLst: segLst not in memo')

    def GenAdContentTFIDF(self):        
        self.TFIDFLst = dict()    
        for word in self.invLst:                        
            self.TFIDFLst[word] = math.log10(self.adSize/len(self.invLst[word]))            
        return self            
        
    def GenTFIDF(self):            
        self.invLstSize = len(self.invLst)
        self.TFIDFLst = dict()
        if self.idfMethod == 'userIndependent':                            
            for word in self.invLst:
                self.TFIDFLst[word] = math.log(self.invLstSize/len(self.invLst[word]))
        elif self.idfMethod == 'userdependent':
            #the output is different with userIndependent mode
            self.invLstSize = 0            
            for i in self.invLstSize:
                self.invLstSize += len(self.invLstSize[i])
            
            for word in self.invLst:
                self.TFIDFLst[word] = dict()
                for user in self.invLst[word]:
                    self.TFIDFLst[word][user] = math.log(self.invLstSize/self.invLst[word][user])
        return self            

    def GenImportantPhrase(self,idfThresholdMin,idfThresholdMax):
        if self.idfMethod == 'userIndependent':
            self.importPhrase = dict()
            if idfThresholdMax < 0:
                for word in self.TFIDFLst:                
                    if self.TFIDFLst[word] > idfThresholdMin:
                        self.importPhrase[word] = self.TFIDFLst[word]
            else:
                for word in self.TFIDFLst:                
                    if self.TFIDFLst[word] > idfThresholdMin and self.TFIDFLst[word] < idfThresholdMax:
                        self.importPhrase[word] = self.TFIDFLst[word]
            return self.importPhrase
        else:
            print('Error: not supported')
            return dict()

    def GetAdContentInvLst(self,inputFile):
        with open(inputFile) as f:
            rawText = f.readlines()
        idx = 0
        for line in rawText:
            ss = line.split()            
            self.GenWordSegString(ss[-1],str(idx)).GenAdContentInvLst()
            idx += 1
        self.adSize = idx
        self.GenAdContentTFIDF()
        return self
        


class WordFilter(object):
    def __init__(self):
        ''
        self.classSize = dict()
        self.importantWords = dict()
        
    def Winit(self,initWordLst,infold):
        classLst = list()
        subfold = glob.glob(infold + '/*')
        for classfold in subfold:
            classLable = classfold.split('/')
            classLst.append(classLable[-1])
            
        self.wordLst = initWordLst
        self.classLst = classLst
        self.res = dict()
        for i in self.wordLst:
            self.res[i] = dict()
            for c in classLst:
                self.res[i][c] = 0    

    def WinitDict(self,initWordLst,inDict):        
        self.wordLst = initWordLst
        self.classLst = inDict.keys()
        self.res = dict()
        for i in self.wordLst:
            self.res[i] = dict()
            for c in self.classLst:
                self.res[i][c] = 0    
        
    def GetState(self,infold):
        #subfold of infold is class fold and should not contain no fold file
        subfold = glob.glob(infold + '/*')
        for classfold in subfold:
            fileLst = glob.glob(classfold + '/*.txt')
            classLable = classfold.split('/')
            classLable = classLable[-1]
            classSize = len(fileLst)
            self.classSize[classLable] = classSize
            
            for txtFile in fileLst:
                with open(txtFile) as f:
                    rawText = f.read()
                    #print rawText.decode('gbk')
                flag = 0
                try:
                    rawText = rawText.decode('gbk').encode('utf8')
                    #print rawText.decode('utf8')
                except:
                    flag = 1
                if flag == 1:
                    continue
                for word in self.wordLst:                    
                    if rawText.find(word) > -1:
                        self.res[word][classLable] += 1
            for word in self.wordLst:
                self.res[word][classLable] = self.res[word][classLable]/classSize
        return self

    def GetStateDict(self,inDict):                
        for classLable in inDict.keys():            
            classSize = len(inDict[classLable])
            self.classSize[classLable] = classSize
            
            for rawText in inDict[classLable]:                                
                for word in self.wordLst:                    
                    if rawText.find(word) > -1:
                        self.res[word][classLable] += 1
                        
            for word in self.wordLst:
                self.res[word][classLable] = self.res[word][classLable]/classSize                
        return self
                
    def GenImportantWord(self,simpleThreshold,stoplist):
        self.sumSize = 0
        for i in self.classSize:
            self.sumSize += self.classSize[i]        
        for word in self.wordLst:
            v = list()
            for classLable in self.res[word]:
                v.append(self.res[word][classLable])
            #v.sort()
            #mean = self.Mean(v)
            t = self.Cov(v) 
            if t >= simpleThreshold and word not in stoplist:
                self.importantWords[word] = t
                
    def Mean(self,v):
        size = len(v)
        s = 0
        for i in v:
            s += i
        return s/size
            
    def Cov(self,v):
        size = len(v)
        mean = self.Mean(v)
        s = 0
        for i in v:
            s += (i - mean)**2
        return math.sqrt(s)
                         
            
            
            























                
                
                
        
            
        
            
        
        
