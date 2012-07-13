#!/usr/bin/env python
# encoding: utf-8

import sys
import re
sys.path.append('/home/muyuwei/collective-intelligence/')

import math
from operator import itemgetter
import mmseg
from doc_freq import GetDocFreqency

def DeviationRate(vec):
  total = 0.0
  for v in vec:
    total += v
  average = total / len(vec)
  variance = 0.0
  for v in vec:
    variance += (v - average) ** 2
  return (variance ** 0.5) / total

class DocCatProbClassifier:
  def __init__(self, cats):
    self.stop_words_ = set()
    self.cat_word_weight_ = {}
    self.cat_id_name_map_ = {-1 : 'unknown'}

    word_cat_prob = {}
    idx = 0
    cat_count = len(cats)
    for cat in cats:
      f = open(cat + '.df')
      for line in f:
        values = line.split()
        df = int(values[0])
        doc_count = int(values[2])
        word = values[3]
        word_cat_prob.setdefault(word, [0.0 for x in range(cat_count)])
        word_cat_prob[word][idx] = 100.0 * df / doc_count
      self.cat_id_name_map_[idx]  = cat
      idx += 1

    for i in range(cat_count):
      self.cat_word_weight_[i] = {}

    for word, dfv in word_cat_prob.items():
      deviation_rate = DeviationRate(dfv)
      max_df = 0.0
      for df in dfv:
        if df > max_df:
          max_df = df

      if max_df > 0.2 and deviation_rate >= 0.68:
        for i in range(len(dfv)):
          #print dfv[i],
          if dfv[i] > 0.0:
            self.cat_word_weight_[i][word] = dfv[i]
        #print word, 'deviation ', deviation_rate
      else:
        self.stop_words_.add(word)

  def Classify(self, text):
    text = re.sub(r'\d', r' ', text)
    text_words = [w for w in mmseg.seg_txt(text)]
    #print text_words
    category = -1
    max_weight = 0.0
    best_unknown_weight = 0.0

    for cat_id, word_weights in self.cat_word_weight_.items():
      #print '---------------------------'
      weight = 0.0
      unknown_weight = 0.0
      for word in text_words:
        if word in word_weights:
          w = word_weights[word] * (3 ** ((len(word) - 1) / 3))
          weight += w
          unknown_weight -= len(word) * 0.6
          #print word, w
        else:
          if word not in self.stop_words_:
            unknown_weight += len(word) * 1.0
            #print word, 'unknown'
          pass

      if weight > max_weight and unknown_weight < 0.0:
        max_weight = weight
        category = cat_id
        best_unknown_weight = unknown_weight
 
    # print text, ':', category, self.cat_id_name_map_[category], max_weight, best_unknown_weight
    return self.cat_id_name_map_[category]

if __name__ == '__main__':
  cats = ('food', 'clothing', 'finance', 'history', 'english')
  classifier = DocCatProbClassifier(cats)
  ads = ['伍度精品男装店', '皙肤泉祛痘挤痘', '俏佳人宝贝衣橱', '高雅气质女鞋,凉鞋打折季', 
      '卡尼尔美白',
      '三国演义',
      '岳飞是抗金英雄',
      '你想吃什么',
      '外汇黄金投资找我',
      '卸妆油49元特卖',
      '尚透社，时尚透了',
      '看三国玩三国',
      '买韩国服装',
      '新款女装上架！！',
      '经典款斜挎男包',
      '少看水浒，老看三国',
      '丰胸洁肤祛痘脱毛',
      '我想去逛街',
      '淘你所爱',
      '天堂伞9.99元起',
      '绿瘦减肥胶囊',
      '美白精华无效退款',
      '明星的不老秘密',
      '祛皱祛痘祛黑头',
      '卡扎菲邀俄中印投资石油业 西方慎对介入',
      '环球企业家]卫哲：黯然谢幕',
      '魔力挺内衣一穿就挺，越穿越挺，解决女性胸部6大问题：扁平、萎缩、下垂、附乳、外扩、含胸',
      '赫衣伦魔束衣 1拉，2铲，3锁定，塑造S形完美身段，减腰收腹、美背减脂、拢胸防垂、翘臀美腿',
      '"健美猪"真相："瘦肉精"猪肉进入双汇公司',
      '住建部将定期复审保障房家庭',
      '明天北京天气：晴转多云，气温高温 13℃/低温 0℃，无持续风向，微风',
      '这是什么类型',
      '玉兰油美白防晒套',
      '北京时间3月15日早间消息，苹果3月11日向欧洲商标局申请获得Apple Corps公司的青苹果Logo，两家公司长达20余年的商标权之争有望画上句号。',
      '当传统武侠遇上时尚，结果会是怎样？《天龙八部2》第四章“飞龙在天”3月18日即将开启豪气公测，全球8000万注册玩家，与7组新增服务器，共同迎接龙腾九天的武侠飞行时代，而新服专属的“飞龙在天探宝之旅”详细计划也于今日首次曝光，包含Louis Vuitton皮包与Iphone4的时尚大奖，将于公测之日起等待幸运儿的即时秒杀!',
      '梦幻系花园发型 打造复古70风潮',
      '6款初春百搭发型 适合所有脸型',
      '想出国，跟我走！',
      '一五旬辽宁籍男子伙同他人从天津抵忻后，刚下火车便窜至忻府区砸碎汽车玻璃实施盗窃。近日，该男子雷某被忻府区公安局作出行政拘留15日的处罚。3月12日12时许，忻府区公安局纪检书记刘国兴途经七一北路一超市附近时，发现两名中年男子神色']
  # ads = ['三岁女儿照顾高位截瘫父亲！ @电影工厂 ：实拍：德州乐陵，三岁女儿照顾高位截瘫父亲！我真没忍住。。哭的稀里哗啦。。看到小女孩倒水的时候就彻底崩溃了。。（打开这个视频之前，请先准备好纸巾！） http://sinaurl.cn/hbDGTx 原文转发(2216) | 原文评论(493) 立 生2233 想一想我们这些八零后三岁时在干嘛了…是在父母的环抱里撒娇、没有自己的思想、只知道哭、笑、…可在她的世界里承担了太多太多…看着她那小巧的身体和那双大眼睛…我哭了…我在这里祝福她健健康康快快乐乐的长大…加油']
  # for ad in ads:
  #   print classifier.Classify(ad), ad

  f = open('t.txt', 'r')
  for line in f:
    print classifier.Classify(line), line

