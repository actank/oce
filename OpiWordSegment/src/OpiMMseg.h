#ifndef __OpiMMseg_H__
#define __OpiMMseg_H__
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "WordSegmentation.h"
using namespace std;

class  OpiMMseg  {

public:
        /*读入配置文件（按顺序包含词典文件、停用词、人名词典和词典DoubleArray索引文件），ex:configure.txt*/
        OpiMMseg(string configure_file);
        ~OpiMMseg();
        /*输入待分词字符串，返回分词后的结果*/
        vector<string> & Segment(const string& query, const string& input_segment_type);
        /*输入待分词文本文件和分词后结果保存的文件*/
        void  SegmentFile(const string for_segment_file_path, const string input_segment_type, const string written_file_path);

private:
        string unigram_dict;
        string stop_words_dict;
        string person_name_dict;
        string unigram_dict_index;
        string segment_type;
        Dict * DictForSeg;
        WordSegmentation * segment_engine;
};
#endif
