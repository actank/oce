#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<fstream>
#include "WordSegmentation.h"
using namespace std;

void usage (char **argv) {
  cerr << "Opi-Corp MM Segment 1.0\n"
       << endl << endl;
  cerr << "usage> " << argv[0] << " [Input] [-u filename] [-f filename] [-o filename] " << endl << endl;
  cerr << "options:" << endl;
  cerr << " Input\tDirectly input the sentences to be segmented" << endl;
  cerr << " -u\tName of dictionary containing the Chinese words (def.= unigram.txt)" << endl;
  cerr << " -f\tName of file containing the sentences to be segmented (def.= corpus.txt)" << endl;
  cerr << " -o\tName of file where the segmented sentences are written (def.= SegmentResults.txt)" << endl;
  cerr << endl;
  return;
}
int main(int argc, char ** argv) 
{
  std::string Input,ForSegmentFile="";
  std::string UnigramDict="";
  std::string WrittenFile="./data/SegmentResults.txt";
  clock_t start,finish;
  double duration;
  if(argc>1)  {
    for(int k=1; k<argc; k++)  {
      if(argv[k][0] != '-')  break;
      else if(argv[k][1] == 'u')  {
        UnigramDict = argv[++k];
      }
      else if(argv[k][1] == 'f')  {
        ForSegmentFile = argv[++k];
      }
      else if(argv[k][1] == 'o')  {
        WrittenFile = argv[++k];
      }
      else usage(argv);
   }
   if(ForSegmentFile.length()==0)  usage(argv);
   else  {  //根据命令行读入待分词文件
     Dict * DictForSeg=new Dict(UnigramDict);
     std::cerr<<"开始读入词典..."<<endl;
     if(UnigramDict=="")  DictForSeg->Construct();
     else  {
       DictForSeg->GenerateSequencedDict();
       DictForSeg->Construct();
     }
     if(DictForSeg->Construct())  {
        std::cerr<<"成功读入词典！"<<endl;
        std::cerr<<"开始读入单字词字典..."<<endl;
        std::cerr<<"成功读入字典!"<<endl;
        ifstream input_file(ForSegmentFile.c_str());
        ofstream output_file(WrittenFile.c_str());
        WordSegmentation * SegmentEngine=new WordSegmentation(DictForSeg);
        cerr<<"开始处理中..."<<endl;int NO=1;
        int length,ProcessIndex=0,sign;
        input_file.seekg(0, ios::end);
        length = input_file.tellg();
        input_file.seekg(0, ios::beg);
        char * buffer= new char [length+1];
        input_file.read(buffer,length);
        buffer[length]=0;
        std::string temp=(std::string)buffer;
        std::string sentence;
        std::string results="";
        start = clock();
        while(ProcessIndex<length)  {
          sign=temp.find("\n",ProcessIndex);
          if(sign>=ProcessIndex)  {
             sentence=temp.substr(ProcessIndex,sign-ProcessIndex);
             ProcessIndex=sign+1;
             SegmentEngine->ProcessSegment(sentence);
             results+=SegmentEngine->SegmentedResult;//潜在的可能造成内存增长，具体应用要改
             results+="\n";
         }  
       } 
       output_file<<results;
       finish=clock();
       duration=(double)(finish-start)/CLOCKS_PER_SEC;
       cerr<<"分词耗时: "<<duration*1000<<"ms"<<endl;
       output_file.close();
       delete buffer;
       delete SegmentEngine;
     }
     else  usage(argv);
     delete DictForSeg; 
    }
  }
  else  {  //对直接输入的语句进行分词
    cin>>Input;
    Dict * DictForSeg = new Dict(UnigramDict);
    std::cerr<<"开始读入词典..."<<endl;
    if(DictForSeg->Construct())  {
       std::cerr<<"成功读入词典！"<<endl;
       std::cerr<<"开始读入单字词字典..."<<endl;
       std::cerr<<"成功读入字典！"<<endl;
       WordSegmentation * SegmentEngine = new WordSegmentation(DictForSeg);
       cerr<<"开始处理中..."<<endl;
       start=clock();
       SegmentEngine->ProcessSegment(Input);
       finish=clock();
       duration=(double)(finish-start)/CLOCKS_PER_SEC;
       cerr<<SegmentEngine->SegmentedResult<<endl;
       cerr<<"分词耗时："<<duration<<endl;
       delete SegmentEngine;
    }
    else usage(argv);
    delete DictForSeg;
  }
  return 0;
}
